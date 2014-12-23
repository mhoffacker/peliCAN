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

#include "connectdialog.h"
#include "ui_connectdialog.h"
#include "global_config.h"

#include <QDialogButtonBox>

/*!
 * \brief Constructor
 * \param parent Parent widget
 */
ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
    can = NULL;

#if !(defined(USE_SOCKET_CAN))
    ui->tabWidget->setCurrentIndex(1);  //Go to page 2
    ui->tabWidget->setTabEnabled(0, false);
#endif
}

/*!
 * \brief Destructor
 */
ConnectDialog::~ConnectDialog()
{
    delete ui;
}

/*!
 * \brief Slot called, after Ok was clicked.
 * Creates a new CCanComm and a configuration string and returns
 * it bz ConnectDialog::can and ConnectDialog::port to the MainWindow
 */
void ConnectDialog::on_buttonBox_accepted()
{
    switch ( ui->tabWidget->currentIndex() )
    {
#ifdef USE_SOCKET_CAN
    case 0: // Open a Socket CAN port

        can = new CSocketCAN();
        port = ui->lineEdit_SocketCAN_device->text();
        setConfig("SOCKET_CAN_PORT", ui->lineEdit_SocketCAN_device->text());
        break;
#endif

    case 1: // Open a SLCAN port

        // Port, COM Port, UART Bitrate, Databits, Parity, Stopbits, CAN speed
        // e.g. COM1,9600.8,N,1,500000
        port = QString("%1,%2,%3,%4,%5,%6")
                .arg(ui->comboBox_COM_port->currentText())
                .arg(ui->comboBox_speed->currentText())
                .arg(ui->comboBox_Data_Bits->currentText())
                .arg(ui->comboBox_Parity->currentText()[0]) // First character of None, Odd, Even
                .arg(ui->comboBox_StopBits->currentText())
                .arg(ui->comboBox_CAN_Bitrate->currentText());
        can = new CSLCAN();

        setConfig("CAN_BITRATE_BOX", QString("%1").arg(ui->comboBox_CAN_Bitrate->currentIndex()));
        setConfig("COM_PORT_BOX", ui->comboBox_COM_port->currentText());
        setConfig("DATA_BIT_BOX", QString("%1").arg(ui->comboBox_Data_Bits->currentIndex()));
        setConfig("PARITY_BOX", QString("%1").arg(ui->comboBox_Parity->currentIndex()));
        setConfig("SPEED_BOX", QString("%1").arg(ui->comboBox_speed->currentIndex()));
        setConfig("STOP_BIT_BOX", QString("%1").arg(ui->comboBox_StopBits->currentIndex()));

        break;

    default:
        can = NULL;
        port = QString("");
        reject();
    }


    accept();
}

/*!
 * \brief Slot called, after Cancel was clicked.
 */
void ConnectDialog::on_buttonBox_rejected()
{
    can = NULL;
    port = QString("");
    reject();
}

void ConnectDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    ui->comboBox_CAN_Bitrate->setCurrentIndex(getConfig("CAN_BITRATE_BOX").toInt());
    ui->comboBox_COM_port->lineEdit()->setText(getConfig("COM_PORT_BOX"));
    ui->comboBox_Data_Bits->setCurrentIndex(getConfig("DATA_BIT_BOX").toInt());
    ui->comboBox_Parity->setCurrentIndex(getConfig("PARITY_BOX").toInt());
    ui->comboBox_speed->setCurrentIndex(getConfig("SPEED_BOX").toInt());
    ui->comboBox_StopBits->setCurrentIndex(getConfig("STOP_BIT_BOX").toInt());
    ui->lineEdit_SocketCAN_device->setText(getConfig("SOCKET_CAN_PORT"));
}
