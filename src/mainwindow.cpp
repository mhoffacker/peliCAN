/*
 *peliCAN
 *Copyright (C) 2014-2015 Michael Hoffacker, mhoffacker3@googlemail.com
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "aboutdialog.h"
#include "libcandbc/dbcReader.h"
#include "ccanmessage.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    can = NULL;

    rt_model = new raw_tree_model(this);

    ui->treeView->setModel(rt_model);
    ui->treeView->setUniformRowHeights(true);


    logging_active = false;
    log_stream = NULL;
    logfile = NULL;

    dialog_send_data = new DialogSendData(this);
    dialog_quadi = new DialogQuadI(this);
    dialog_dataview = new DialogDataView(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::can_received(const decoded_can_frame &frame)
{
    QString signal_name = "";

    foreach(CCANMessage *item, can_messages) {
        if ( item->process_frame(frame) ) {
            signal_name = item->GetName();

            dialog_dataview->UpdateMessage(item);

            if ( !ui->checkBox_ShowAll->isChecked() ) {
                // Update CAN msg list view
                rt_model->insert_data(signal_name, frame);
            }
        }
    }

    if ( ui->checkBox_ShowAll->isChecked() ) {
        // Update CAN msg list view
        rt_model->insert_data(signal_name, frame);
    }

    // Update QuadI view
    dialog_quadi->can_received(frame);

    // Log data
    if ( logging_active && log_stream)
    {
        *log_stream << frame.time_as_str << ';'
                    << frame.can_id << ';'
                    << frame.EFF << ';'
                    << frame.ERR << ';'
                    << frame.RTR << ';'
                    << frame.can_dlc << ';'
                    << frame.data[0] << ';'
                    << frame.data[1] << ';'
                    << frame.data[2] << ';'
                    << frame.data[3] << ';'
                    << frame.data[4] << ';'
                    << frame.data[5] << ';'
                    << frame.data[6] << ';'
                    << frame.data[7] << endl;
    }
}

bool MainWindow::online(bool o, QString port)
{
    if ( o )
    {
        if ( can == NULL )
            return false;

        connect(can, SIGNAL(received(const decoded_can_frame&)),
                this, SLOT(can_received(const decoded_can_frame&)));

        if ( can->open_can(port) )
        {
            ui->label_online->setText("Connection: Online on "+port);
            ui->action_Load_signal_definition->setEnabled(false);
            ui->action_RAW_csv_to_readable_csv->setEnabled(false);
            ui->action_Edit_signal_definition->setEnabled(false);

            dialog_send_data->setCanComm(can);
            dialog_quadi->setCanComm(can);
            return true;
        } else {
            return false;

        }
    } else {
        if ( can == NULL )
            return false;

        dialog_send_data->setCanComm(NULL);
        dialog_quadi->setCanComm(NULL);

        disconnect(can, SIGNAL(received(const decoded_can_frame&)),
                    this, SLOT(can_received(const decoded_can_frame&)));

        can->close_can();
        //delete can;
        can = NULL;

        ui->label_online->setText("Connection: Offline");

        ui->action_Load_signal_definition->setEnabled(true);
        ui->action_RAW_csv_to_readable_csv->setEnabled(true);
        ui->action_Edit_signal_definition->setEnabled(true);

        return true;
    }
}

void MainWindow::on_action_Online_triggered(bool checked)
{
    if ( checked == false )
    {
        online(false, QString(""));

    } else {
        ConnectDialog d(this);
        d.exec();
        can = d.can;

        if ( d.result() == QDialog::Accepted )
        {
            if ( !online(true, d.port) )
            {
                QMessageBox msgBox;
                msgBox.setText("Could not open communication device.");
                msgBox.exec();
                ui->action_Online->setChecked(false);
            }

        } else {
            ui->action_Online->setChecked(false);
        }
    }
}

void MainWindow::on_action_Load_signal_definition_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open DBC file"), "", tr("CAN dbc (*.dbc)"));

    if ( fileName == "" )
        return;

    //TODO: Replace with clean C++ code
    char* s = new char[fileName.length()+1];
    strcpy(s, fileName.toStdString().c_str());
    dbc_t *dbc = dbc_read_file(s);
    delete[] s;

    if ( dbc == NULL ) {
        QMessageBox msgBox;
        msgBox.setText("Could not load signal definition file.");
        msgBox.exec();
        ui->label_signal_def->setText("Signal definition: None");
        return;
    }

    dialog_dataview->ResetView();

    message_list_t *msg_list = dbc->message_list;
    while ( msg_list ) {

        message_t *msg = msg_list->message;

        CCANMessage *can_msg = new CCANMessage();
        can_msg->SetName(msg->name);
        can_msg->SetID(msg->id);
        can_msg->SetSize(msg->len);
        can_messages.append(can_msg);

        signal_list_t *sig = msg->signal_list;
        while ( sig ) {
            //signedness 1 = signed
            //endianess 0 = little

            CCANSignal2 *new_sig = new CCANSignal2();
            // 0 = integer
            // 1 = 32bit float
            // 2 = 64bit float
            if ( sig->signal->signal_val_type == 0 ) {
                if ( sig->signal->signedness == 1 ) {
                    // signed
                    new_sig->SetType(SIGNED_INT);
                } else {
                    new_sig->SetType(UNSIGNED_INT);
                }
            } else if ( sig->signal->signal_val_type == 1 ) {
                new_sig->SetType(FLOAT32);
            } else {
                new_sig->SetType(FLOAT64);
            }

            if ( sig->signal->endianess == 1) {
                new_sig->SetEndianess(ENDIAN_BIG);
            } else {
                new_sig->SetEndianess(ENDIAN_LITTLE);
            }

            new_sig->SetFactor(sig->signal->scale);
            new_sig->SetLength(sig->signal->bit_len);
            new_sig->SetMax(sig->signal->max);
            new_sig->SetMin(sig->signal->min);
            new_sig->SetOffset(sig->signal->offset);
            new_sig->SetStartbit(sig->signal->bit_start);
            new_sig->SetUnit(sig->signal->unit);
            new_sig->SetName(sig->signal->name);

            can_msg->AddSignal(new_sig);

            sig = sig->next;
        }

        dialog_dataview->AddCANMessage(can_msg);

        msg_list = msg_list->next;
    }

    ui->label_signal_def->setText("Signal definition: "+fileName);
}

void MainWindow::on_action_About_triggered()
{
    AboutDialog d(this);
    d.exec();
}


bool MainWindow::log(bool l, QString fileName)
{
    if ( l )
    {
        *log_stream << "Time" << ';'
                    << "ID" << ';'
                    << "EXT" << ';'
                    << "ERR" << ';'
                    << "RTR" << ';'
                    << "DLC" << ';'
                    << "DAT_0" << ';'
                    << "DAT_1" << ';'
                    << "DAT_2" << ';'
                    << "DAT_3" << ';'
                    << "DAT_4" << ';'
                    << "DAT_5" << ';'
                    << "DAT_6" << ';'
                    << "DAT_7" << endl;

        logging_active = true;
        ui->label_logging->setText("Logging: "+fileName);



    } else {
        ui->label_logging->setText("Logging: Off");

        if ( !logging_active )
            return false;

        logging_active = false;

        logfile->close();

        if ( log_stream )
            delete log_stream;

        if ( logfile )
            delete logfile;
    }

    return true;
}

void MainWindow::on_action_Logging_Active_triggered(bool checked)
{
    if ( checked )
    {
        if ( logging_active )
        {
            // You should never get here...
            log(false);
        }

        QString fileName = QFileDialog::getSaveFileName(this, tr("Select logfile"), "", tr("CSV document (*.csv)"));

        if ( fileName == "" )
        {
            ui->action_Logging_Active->setChecked(false);
            return;
        }


        logfile = new QFile(fileName);

        if ( logfile == NULL )
        {
            ui->action_Logging_Active->setChecked(false);
            return;
        }

        logfile->open(QIODevice::WriteOnly | QIODevice::Text);

        log_stream = new QTextStream(logfile);

        if ( log_stream == NULL )
        {
            ui->action_Logging_Active->setChecked(false);
            return;
        }


        log(true, fileName);

    } else {
        log(false);

    }
}

void MainWindow::closeEvent(QCloseEvent *bar)
{
    Q_UNUSED(bar);

    log(false);                 // Stop logging
    online(false, QString("")); // Close connection

    dialog_dataview->close();
    dialog_quadi->close();
    dialog_send_data->close();   // Close send data dialog
}

void MainWindow::on_action_Quit_triggered()
{
    close();
}


void MainWindow::on_action_Data_Graph_triggered(bool checked)
{
    Q_UNUSED(checked);
    dialog_dataview->setVisible(!dialog_dataview->isVisible());
}

void MainWindow::on_actionShow_Hide_send_signal_triggered()
{
    dialog_send_data->setVisible(!dialog_send_data->isVisible());
}

void MainWindow::on_action_Help_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.hans-dampf.org"));
}

void MainWindow::on_actionQuadI_configurator_triggered()
{
    dialog_quadi->setVisible(!dialog_quadi->isVisible());
}
