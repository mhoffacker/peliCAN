/*
 *peliCAN
 *Copyright (C) 2014 Michael Hoffacker, mhoffacker3@googlemail.com
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
#include "dialograw2readable.h"
#include "csignaldisplay.h"

#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    can = NULL;

    rt_model = new raw_tree_model(this);
    st_model = new signal_tree_model(this);

    can_signals_root = new CCANSignal();
    can_signals_root->setName("Loaded CAN Signals");
    st_model->SetSignalRoot(can_signals_root);

    ui->treeView->setModel(rt_model);
    ui->treeView->setUniformRowHeights(true);


    logging_active = false;
    log_stream = NULL;
    logfile = NULL;

    dialog_signal_tree = new DialogSignalTree(this);
    dialog_send_data = new DialogSendData(this);
}

MainWindow::~MainWindow()
{
    can_signals_root->clear();   // Clear items
    clear_can_raw_view_filter();

    delete can_signals_root;

    delete ui;
}

void MainWindow::clear_can_raw_view_filter()
{
    while ( !can_raw_view_filter.isEmpty() )
    {
        CCANFilter* filter = can_raw_view_filter.first();
        can_raw_view_filter.removeFirst();

        delete filter;
    }
}

void MainWindow::can_received(const decoded_can_frame &frame)
{
    QString signal_name = "";
    bool matched = false;

    // Search every signal

    foreach (CCANSignal* item_group, *can_signals_root->getChildren() )
    {
        foreach (CCANSignal* item_signal, *item_group->getChildren())
        {
            if ( item_signal->process_frame(frame, dialog_signal_tree->getUpdateRate()) )
            {
                // If the frame is accepted, save the name for the raw view window.
                // Only the first one is saved.
                if ( signal_name == "" )
                    signal_name = item_signal->getName();

                // Update view
                st_model->DataHasChanged(item_signal);
            }
        }
    }
    dialog_signal_tree->getGraph()->update();


    foreach (CCANFilter* item, can_raw_view_filter) {
        matched = item->match(frame);
        if ( matched )
        {
            rt_model->insert_data(signal_name, frame);

            if ( logging_active )
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

            break;
        }
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

            dialog_send_data->setCanComm(can);
            return true;
        } else {
            return false;

        }
    } else {
        if ( can == NULL )
            return false;

        dialog_send_data->setCanComm(NULL);

        disconnect(can, SIGNAL(received(const decoded_can_frame&)),
                    this, SLOT(can_received(const decoded_can_frame&)));

        can->close_can();
        //delete can;
        can = NULL;

        ui->label_online->setText("Connection: Offline");

        ui->action_Load_signal_definition->setEnabled(true);
        ui->action_RAW_csv_to_readable_csv->setEnabled(true);

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

void MainWindow::XML_Attr_To_Filter(CCANFilter *filter, QXmlStreamAttributes &attributes)
{

    bool ok;
    unsigned long l_tmp;
    unsigned long long ll_tmp;
    unsigned char data[8];

    if ( attributes.hasAttribute("eid") )
        filter->setEIDMask( attributes.value("eid").toString() == "true" );

    if ( attributes.hasAttribute("sid") )
        filter->setSIDMask( attributes.value("sid").toString() == "true" );

    if ( attributes.hasAttribute("id_mask") )
    {
        l_tmp = attributes.value("id_mask").toString().toULong(&ok, 16);
        if ( ok )
            filter->setIDMask(l_tmp);
    }

    if ( attributes.hasAttribute("dlc_mask") )
    {
        l_tmp = attributes.value("dlc_mask").toString().toULong(&ok, 16);
        if ( ok && l_tmp <= 0xFF )
            filter->setDLCMask(l_tmp & 0xFF);
    }

    if ( attributes.hasAttribute("rtr_mask") )
    {
        l_tmp = attributes.value("rtr_mask").toString().toULong(&ok, 16);
        if ( ok && l_tmp <= 0xFF )
            filter->setRTRMask(l_tmp & 0xFF);
    }

    if ( attributes.hasAttribute("data_mask") )
    {
        ll_tmp = attributes.value("data_mask").toString().toULongLong(&ok, 16);

        if ( ok )
        {
            for ( int i=7; i>=0; i-- )
            {
                data[i] = ll_tmp & 0xFF;
                ll_tmp = ll_tmp >> 8;
            }
            filter->setDataMask((unsigned char*)&data);
        }
    }
}

void MainWindow::XML_Read_Raw_View(QXmlStreamReader &xmlStream)
{
    while ( !xmlStream.atEnd() && !xmlStream.hasError() )
    {
        QXmlStreamReader::TokenType token = xmlStream.readNext();
        if ( token == QXmlStreamReader::StartElement )
        {
            if ( xmlStream.name() == "filter")
            {
                CCANFilter *new_filter = new CCANFilter();

                QXmlStreamAttributes attributes = xmlStream.attributes();
                XML_Attr_To_Filter(new_filter, attributes);

                can_raw_view_filter.append(new_filter);

            }
        }

        if ( token == QXmlStreamReader::EndElement )
        {
            if ( xmlStream.name() == "raw_view" )
                return;
        }
    }
}

void MainWindow::XML_Read_Signal(CCANSignal *sig_group, QXmlStreamReader &xmlStream)
{
    CCANSignal *new_sig = NULL;
    CSignalDisplay *sd = NULL;

    while ( !xmlStream.atEnd() && !xmlStream.hasError() )
    {
        QXmlStreamReader::TokenType token = xmlStream.readNext();

        if ( token == QXmlStreamReader::StartElement )
        {
            QXmlStreamAttributes attributes = xmlStream.attributes();

            if ( xmlStream.name() == "signal" )
            {
                new_sig = new CCANSignal();

                if ( attributes.hasAttribute("name") )
                    new_sig->setName(attributes.value("name").toString());
            }

            if ( xmlStream.name() == "filter" )
            {
                if ( new_sig != NULL )
                    XML_Attr_To_Filter(new_sig, attributes);
            }

            if ( xmlStream.name() == "graph" )
            {
                if ( new_sig != NULL )
                {
                    CPlot *new_plot = NULL;
                    if ( attributes.hasAttribute("scale") )
                    {
                        new_plot = new_sig->addPlot(dialog_signal_tree->getGraph());
                        new_plot->setAxis(AxisHash[attributes.value("scale").toString()]);
                    }
                    if ( attributes.hasAttribute("color") )
                    {
                        bool ok;
                        unsigned long c = attributes.value("color").toString().toULong(&ok, 16);

                        if ( ok )
                        {
                            QColor color((c>>16) & 0xFF, (c>>8) & 0xFF, c & 0xFF, 255);
                            new_sig->setColor(new QPen(color));
                        }
                    }
                }
            }

            if ( xmlStream.name() == "display")
            {
                if ( new_sig != NULL )
                {
                    if ( attributes.hasAttribute("type") )
                    {
                        QString t = attributes.value("type").toString();
                        QString dm("0xFFFFFFFFFFFFFFFFF");


                        if ( attributes.hasAttribute("display_mask") )
                                dm = attributes.value("display_mask").toString();

                        if ( t.toLower() == "hex")
                        {
                            sd = new CSignalDisplayHex();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "int16" )
                        {
                            sd = new CSignalDisplayInt16();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "uint16" )
                        {
                            sd = new CSignalDisplayUInt16();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "int32" )
                        {
                            sd = new CSignalDisplayInt32();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "uint32" )
                        {
                            sd = new CSignalDisplayUInt32();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "int64" )
                        {
                            sd = new CSignalDisplayInt64();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "uint64" )
                        {
                            sd = new CSignalDisplayUInt64();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "float" )
                        {
                            sd = new CSignalDisplayFloat();
                            sd->setParam(dm);
                        } else if ( t.toLower() == "script" )
                        {
                            sd = new CSignalDisplayScript();
                        }

                        if ( attributes.hasAttribute("unit") && sd != NULL )
                            sd->setUnit(attributes.value("unit").toString());

                    }
                }
            }

        }

        if ( token == QXmlStreamReader::Characters )
        {
            if ( sd != NULL )
            {
                sd->setParam(xmlStream.text().toString());
            }
        }

        if ( token == QXmlStreamReader::EndElement )
        {
            if ( xmlStream.name() == "signal" )
            {
                new_sig->setParent(sig_group);
                sig_group->addChild(new_sig);
                new_sig = NULL;
            }

            if ( xmlStream.name() == "display" )
            {
                if ( sd != NULL )
                    new_sig->setSignalDisplay(sd);
                sd = NULL;
            }

            if ( xmlStream.name() == "signal_group" )
                return;
        }
    }
}

void MainWindow::XML_Read_Scales(QXmlStreamReader &xmlStream)
{
    while ( !xmlStream.atEnd() && !xmlStream.hasError() )
    {
        QXmlStreamReader::TokenType token = xmlStream.readNext();

        if ( token == QXmlStreamReader::StartElement )
        {
            QXmlStreamAttributes attributes = xmlStream.attributes();
            if ( xmlStream.name() == "scale" )
            {
                CAxis *a = NULL;

                if ( attributes.hasAttribute("position") )
                {
                    if ( attributes.value("position").toString() == "left" )
                        a = dialog_signal_tree->getGraph()->addAxisLeft();
                    else
                        a = dialog_signal_tree->getGraph()->addAxisRight();

                    if ( attributes.hasAttribute("text") && a != NULL )
                        a->getScale()->setTitle(attributes.value("text").toString());

                    if ( attributes.hasAttribute("id") )
                        AxisHash[attributes.value("id").toString()] = a;
                }
            }
        }

        if ( token == QXmlStreamReader::EndElement )
        {
            if ( xmlStream.name() == "scales" )
                return;
        }
    }
}

void MainWindow::XML_Read_Sender(QXmlStreamReader &xmlStream)
{
    while ( !xmlStream.atEnd() && !xmlStream.hasError() )
    {
        QXmlStreamReader::TokenType token = xmlStream.readNext();

        if ( token == QXmlStreamReader::StartElement )
        {
            QXmlStreamAttributes attributes = xmlStream.attributes();
            if ( xmlStream.name() == "signal" )
            {
                send_widget_special *a = NULL;

                if ( attributes.hasAttribute("name") )
                {
                    a = dialog_send_data->AddSpecialWidget();
                    a->setName(attributes.value("name").toString());

                    if ( attributes.hasAttribute("id") )
                        a->setID(attributes.value("id").toString().toULongLong(0, 16));
                    else
                        a->setID(0);

                    if ( attributes.hasAttribute("eid") )
                        a->setEXT(attributes.value("eid").toString()=="true");
                    else
                        a->setEXT(false);

                    if ( attributes.hasAttribute("dlc") )
                        a->setDLC(attributes.value("dlc").toString().toInt(0, 16));
                    else
                        a->setDLC(0);

                    if ( attributes.hasAttribute("send_mask") )
                        a->setDataMask(attributes.value("send_mask").toString().toULongLong(0, 16));
                    else
                        a->setDataMask(0);

                    if ( attributes.hasAttribute("data") )
                        a->setData(attributes.value("data").toString().toULongLong(0, 16));
                    else
                        a->setData(0);

                    if ( attributes.hasAttribute("type") )
                    {
                        if ( attributes.value("type").toString() == "float" )
                            a->setType(send_widget_special::FLOAT);
                        else if ( attributes.value("type").toString() == "int16" )
                            a->setType(send_widget_special::INT16);
                        else if ( attributes.value("type").toString() == "uint16" )
                            a->setType(send_widget_special::UINT16);
                        else if ( attributes.value("type").toString() == "int32" )
                            a->setType(send_widget_special::INT32);
                        else if ( attributes.value("type").toString() == "uint32" )
                            a->setType(send_widget_special::UINT32);
                        else if ( attributes.value("type").toString() == "int64" )
                            a->setType(send_widget_special::INT64);
                        else if ( attributes.value("type").toString() == "uint64" )
                            a->setType(send_widget_special::UINT64);
                        else if ( attributes.value("type").toString() == "hex" )
                            a->setType(send_widget_special::HEX);
                        else
                            a->setType(send_widget_special::UNDEFINED);
                    } else {
                        a->setType(send_widget_special::UNDEFINED);
                    }
                }
            }
        }

        if ( token == QXmlStreamReader::EndElement )
        {
            if ( xmlStream.name() == "sender" )
                return;
        }
    }
}

bool MainWindow::XML_Read_Settings(QIODevice *device)
{
    QXmlStreamReader xmlStream(device);

    clear_can_raw_view_filter();
    can_signals_root->clear();

    foreach ( CAxis *a, AxisHash )
    {
        dialog_signal_tree->getGraph()->removeAxis(a);
    }

    AxisHash.clear();

    while ( !xmlStream.atEnd() && !xmlStream.hasError() )
    {
        QXmlStreamReader::TokenType token = xmlStream.readNext();

        if ( token == QXmlStreamReader::StartDocument )
            continue;

        if ( token == QXmlStreamReader::StartElement )
        {
            if ( xmlStream.name() == "message_filter_definition")
                continue;

            if ( xmlStream.name() == "scales" )
                XML_Read_Scales(xmlStream);

            if ( xmlStream.name() == "raw_view" )
                XML_Read_Raw_View(xmlStream);

            if ( xmlStream.name() == "sender" )
                XML_Read_Sender(xmlStream);

            if ( xmlStream.name() == "signal_group")
            {
                CCANSignal *sig_group = new CCANSignal();

                QXmlStreamAttributes attributes = xmlStream.attributes();
                if ( attributes.hasAttribute("name") )
                    sig_group->setName(attributes.value("name").toString());

                XML_Read_Signal(sig_group, xmlStream);

                can_signals_root->addChild(sig_group);
                sig_group->setParent(can_signals_root);
            }
        }
    }

    if ( xmlStream.hasError() )
    {
        return false;
    }
    return true;
}


void MainWindow::on_action_Load_signal_definition_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Settings"), "", tr("XML Document (*.xml)"));

    if ( fileName == "" )
        return;


    QFile *file = new QFile(fileName);
    file->open(QIODevice::ReadOnly | QIODevice::Text);

    if ( !XML_Read_Settings(file) )
    {
        QMessageBox msgBox;
        msgBox.setText("Could not load signal definition file.");
        msgBox.exec();
        ui->label_signal_def->setText("Signal definition: None, accepting all in raw-view.");
    } else {
        ui->label_signal_def->setText("Signal definition: "+fileName);
        dialog_signal_tree->SetTreeModel(NULL);
        dialog_signal_tree->SetTreeModel(st_model);
        dialog_signal_tree->ExpandTree();
    }

    file->close();
    delete file;
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

    dialog_signal_tree->close(); // Close signal tree dialog
    dialog_send_data->close();   // Close send data dialog
}

void MainWindow::on_action_Quit_triggered()
{
    close();
}


void MainWindow::on_action_Data_Graph_triggered(bool checked)
{
    Q_UNUSED(checked);

    dialog_signal_tree->setVisible(!dialog_signal_tree->isVisible());
}

void MainWindow::on_action_RAW_csv_to_readable_csv_triggered()
{
    DialogRAW2Readable d;
    bool err = false;
    int line_nr = 1;
    QString signal_time;
    QString signal_name;
    QString signal_value;
    QString signal_unit;

    d.exec();

    if ( d.result() == QDialog::Rejected )
        return;

    QFile in_file(d.getSource());
    if ( !in_file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QMessageBox msgBox;
        msgBox.setText("Could not open file to read: " + d.getSource());
        msgBox.exec();
        return;
    }
    QTextStream in(&in_file);

    QFile out_file(d.getTarget());
    if ( !out_file.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        QMessageBox msgBox;
        msgBox.setText("Could not open file to write: " + d.getTarget());
        msgBox.exec();
        return;
    }
    QTextStream out(&out_file);

    QString line;

    // Read header data
    line = in.readLine();
    line = in.readLine();

    // Write header data
    out << "Time;Name;Value;Unit" << endl;

    while ( !in.atEnd() )
    {
        line = in.readLine();
        line_nr++;

        QStringList list = line.split(";");

        int size;
        if ( (size = list.size()) != 14 )
        {
            err = true;
            break;
        }

        decoded_can_frame frame;

        signal_time = list[0];
        frame.can_id = list[1].toInt();
        frame.EFF = (list[2].toInt() != 0);
        frame.ERR = (list[3].toInt() != 0);
        frame.RTR = (list[4].toInt() != 0);
        frame.can_dlc = list[5].toInt();
        for ( int i=0; i<8; i++ )
            frame.data[i] = list[6+i].toInt();

        foreach (CCANSignal* item_group, *can_signals_root->getChildren() )
        {
            foreach (CCANSignal* item_signal, *item_group->getChildren())
            {
                if ( item_signal->process_frame(frame, -1) )
                {
                    signal_name = item_signal->getName();
                    signal_value = item_signal->getCurrentValue();
                    signal_unit = item_signal->getUnit();
                    out << signal_time << ";"
                        << signal_name << ";"
                        << signal_value << ";"
                        << signal_unit << endl;
                }
            }
        }

    }


    if ( !err )
    {
        QMessageBox msgBox;
        msgBox.setText("Conversion finished.");
        msgBox.exec();
    } else {
        QMessageBox msgBox;
        msgBox.setText(QString("Conversion error in line %1.").arg(line_nr));
        msgBox.exec();
    }

    in_file.close();
    out_file.close();
}

void MainWindow::on_actionShow_Hide_send_signal_triggered()
{
    dialog_send_data->setVisible(!dialog_send_data->isVisible());
}
