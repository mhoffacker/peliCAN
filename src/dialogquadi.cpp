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

#include "dialogquadi.h"
#include "ui_dialogquadi.h"

#include <qmessagebox.h>
#include <qtimer.h>


DialogQuadI::DialogQuadI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogQuadI)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(recv_timeout()));
    timer->stop();

    can = NULL;
    state = NONE;
    to_read = READ_NONE;

    CONFIG_crc_init();
}

DialogQuadI::~DialogQuadI()
{
    delete ui;
}

#define CRC_POLY			0x04C11DB7
#define CRC_TOPBIT			(1<<31)
#define CRC_INIT_REMINDER	0xFFFFFFFF

void DialogQuadI::CONFIG_crc_init(void) {
    uint32_t remainder;
    uint32_t dividend = CRC_INIT_REMINDER;
    uint8_t bit;

    for ( dividend = 0; dividend < 256; dividend++ ) {
        remainder = dividend << 24;

        for ( bit=8; bit>0; bit++ ) {
            if ( remainder & CRC_TOPBIT ) {
                remainder = (remainder << 1) ^ CRC_POLY;
            } else {
                remainder = (remainder << 1);
            }
        }

        crcTable[dividend] = remainder;
    }
}

uint32_t DialogQuadI::CONFIG_calc_crc(struct QuadI_config *ptr) {
    uint8_t *p = ((uint8_t*) ptr);
    uint32_t data;
    uint32_t byte;
    uint32_t remainder = CRC_INIT_REMINDER;

    // Skip first 4 bytes
    for ( byte=4; byte<sizeof(struct QuadI_config); byte++ ) {
        data = p[byte] ^ (remainder >> 24);
        remainder = crcTable[data] ^ (remainder << 8);
    }

    return remainder;
}

void DialogQuadI::setCanComm(CCanComm *c)
{
    can = c;
}

void DialogQuadI::recv_timeout()
{
    state=NONE;
    timer->stop();

    QMessageBox msgBox;
    msgBox.setText("Timeout while reception!");
    msgBox.exec();

}

// Write the next of the output buffer
void DialogQuadI::write_next_frame() {
    uint8_t buf[8];
    uint8_t i = 0;

    // Set index of transmission
    buf[0] = (transmit_index >> 8) & 0xFF;
    buf[1] = transmit_index & 0xFF;

    // Copy 6 bytes into buffer
    while ( ( (transmit_index-1)*6 + i < transmit_length) && i <= 5 ) {
        buf[i+2] = trans_buf[(transmit_index-1)*6 + i];
        i++;
    }
    if ( can )
        can->send_can(transmitted_id+0x100, false, false, i+2, (uint8_t*)buf);
}

void DialogQuadI::can_received(const decoded_can_frame &frame)
{
    struct QuadI_config config_temp;

    uint16_t ticks = (((uint16_t)frame.data[0]) << 8) | frame.data[1];

    if ( frame.can_id == CAN_ID_STATUS_MSG ) {
        ui->lineEdit_Ticks->setText(QString("%1").arg(ticks));
        ui->lineEdit_Status_Byte_1->setText(QString("0x%1").arg(frame.data[2], 2, 16, QChar('0')));
        ui->lineEdit_Status_Byte_2->setText(QString("0x%1").arg(frame.data[3], 2, 16, QChar('0')));
        ui->lineEdit_Status_Byte_3->setText(QString("0x%1").arg(frame.data[4], 2, 16, QChar('0')));
        ui->lineEdit_Status_Byte_4->setText(QString("0x%1").arg(frame.data[5], 2, 16, QChar('0')));
        ui->lineEdit_Status_Byte_5->setText(QString("0x%1").arg(frame.data[6], 2, 16, QChar('0')));
        ui->lineEdit_Status_Byte_6->setText(QString("0x%1").arg(frame.data[7], 2, 16, QChar('0')));

        uint8_t mode_status = frame.data[7];

        ui->checkBox_HWT->setChecked((mode_status & 0x20) == 0x20);
        ui->doubleSpinBox_Motor1->setEnabled((mode_status & 0x20) == 0x20);
        ui->doubleSpinBox_Motor2->setEnabled((mode_status & 0x20) == 0x20);
        ui->doubleSpinBox_Motor3->setEnabled((mode_status & 0x20) == 0x20);
        ui->doubleSpinBox_Motor4->setEnabled((mode_status & 0x20) == 0x20);

        ui->checkBox_ParamMode->setChecked((mode_status & 0x01) == 0x01);
        ui->checkBox_Control->setChecked((mode_status & 0x40) == 0x40);

    } else if ( frame.can_id == CAN_ID_ACCEL_RAW ) {
        ui->lineEdit_Ticks->setText(QString("%1").arg(ticks));
        int16_t raw_x, raw_y, raw_z;

        uint16_t raw_x_tmp = (((uint16_t)frame.data[2]) << 8) | frame.data[3];
        uint16_t raw_y_tmp = (((uint16_t)frame.data[4]) << 8) | frame.data[5];
        uint16_t raw_z_tmp = (((uint16_t)frame.data[6]) << 8) | frame.data[7];

        memcpy(&raw_x, &raw_x_tmp, sizeof(int16_t));
        memcpy(&raw_y, &raw_y_tmp, sizeof(int16_t));
        memcpy(&raw_z, &raw_z_tmp, sizeof(int16_t));

        ui->lcdNumber_AccelRawX->display(raw_x);
        ui->lcdNumber_AccelRawY->display(raw_y);
        ui->lcdNumber_AccelRawZ->display(raw_z);

    } else if ( frame.can_id == CAN_ID_GYRO_RAW ) {
        ui->lineEdit_Ticks->setText(QString("%1").arg(ticks));
        int16_t raw_x, raw_y, raw_z;

        uint16_t raw_x_tmp = (((uint16_t)frame.data[2]) << 8) | frame.data[3];
        uint16_t raw_y_tmp = (((uint16_t)frame.data[4]) << 8) | frame.data[5];
        uint16_t raw_z_tmp = (((uint16_t)frame.data[6]) << 8) | frame.data[7];

        memcpy(&raw_x, &raw_x_tmp, sizeof(int16_t));
        memcpy(&raw_y, &raw_y_tmp, sizeof(int16_t));
        memcpy(&raw_z, &raw_z_tmp, sizeof(int16_t));

        ui->lcdNumber_GyroRawX->display(raw_x);
        ui->lcdNumber_GyroRawY->display(raw_y);
        ui->lcdNumber_GyroRawZ->display(raw_z);

    }

    if ( state == READ ) {

        if ( frame.can_id == transmitted_id && frame.can_dlc >= 2 ) {
            recv_dat_length = ((int32_t)frame.data[0] << 8) | (int32_t)frame.data[1];

            if ( recv_dat_length == 0 ) {
                timer->stop();
                QMessageBox msgBox;
                msgBox.setText("Reply 0x00 sent!");
                msgBox.exec();
                return;
            } else {

                // Config stored successfully
                if ( to_read == READ_ACK_CONFIG_STORE ) {

                    timer->stop();
                    to_read = READ_NONE;
                    state = NONE;

                    QMessageBox msgBox;
                    msgBox.setText("Config successfully stored in EEPROM");
                    msgBox.exec();

                    return;
                } else if ( to_read == READ_ACK_WRITE ) {
                    if ( recv_dat_length != transmit_length ) {
                        timer->stop();
                        to_read = READ_NONE;
                        state = NONE;

                        QMessageBox msgBox;
                        msgBox.setText("No acknowledge of write size");
                        msgBox.exec();

                        return;
                    } else {
                        state = WRITE;
                        write_next_frame();
                        // Restart timer
                        timer->stop();
                        timer->start(TIMEOUT_INTERVAL);
                        return;
                    }
                } else if ( to_read == READ_FINAL_ACK_WRITE ) {
                    timer->stop();
                    to_read = READ_NONE;
                    state = NONE;

                    if ( recv_dat_length == 0 ) {
                        QMessageBox msgBox;
                        msgBox.setText("An error occured while writing");
                        msgBox.exec();
                    } else {
                        QMessageBox msgBox;
                        msgBox.setText("Writing finished");
                        msgBox.exec();
                    }
                } else if ( to_read == READ_ACK_SET_SPEED ) {
                    timer->stop();
                    to_read = READ_NONE;
                    state = NONE;
                    return;
                }
            }

            // Restart timer
            timer->stop();
            timer->start(TIMEOUT_INTERVAL);

        } else if (frame.can_id == transmitted_id + 0x100 ) {
            // Index of 6 byte block, starting with 1
            uint16_t index = ((uint16_t)frame.data[0] << 8) | (uint16_t)frame.data[1];

            // Start at 2. byte of frame
            uint16_t byte = 2;
            // Read all bytes without length word
            for ( int i=(index-1) * 6; i<(index-1) * 6 + frame.can_dlc - 2; i++ )
            {
                recv_buf[i] = frame.data[byte];
                recv_full[i] = true;

                byte++;
            }
        }

        // Data bytes were received
        if ( recv_dat_length > 0 ) {
            // Check if all data is available
            for ( int i=0; i<recv_dat_length; i++ )
            {
                // If one is missing, return
                if ( !recv_full[i] ) {
                    // Restart timer
                    timer->stop();
                    timer->start(TIMEOUT_INTERVAL);
                    return;
                }
            }

            // We only come here, if all data bytes are received.

            // Stop timer
            timer->stop();
            state = NONE;

            if (  to_read == READ_CONFIG ) {

                if ( recv_dat_length != sizeof(struct QuadI_config) ) {
                    QMessageBox msgBox;
                    msgBox.setText("Wrong numbers of byte received. Check config version!");
                    msgBox.exec();

                    return;
                }

                memcpy((void*)&config_temp, (void*)recv_buf, recv_dat_length);

                uint32_t crc = CONFIG_calc_crc(&config_temp);

                if ( crc != config_temp.checksum ) {
                    QMessageBox msgBox;
                    msgBox.setText("CRC error while config read");
                    msgBox.exec();
                } else {
                    memcpy((void*)&config, (void*)&config_temp, recv_dat_length);

                    ui->lineEdit_versionMajor->setText(QString("0x%1").arg(config.header.major, 2, 16, QChar('0')));
                    ui->lineEdit_versionMinor->setText(QString("0x%1").arg(config.header.minor, 2, 16, QChar('0')));

                    ui->lineEdit_CAN_bitrate->setText(QString("%1").arg(config.can.bitrate));

                    ui->lineEdit_I2C_bitrate->setText(QString("%1").arg(config.i2c.bitrate));

                    ui->lineEdit_MotorDriverFreq->setText(QString("%1").arg(config.motor.pwm_frequency));
                    ui->comboBox_CONFIG0_RM->setCurrentIndex((config.motor.config_0 >> 10) & 0x03);
                    ui->lineEdit_CONFIG0_BT->setText(QString("%1").arg((config.motor.config_0 >> 6) & 0x0F));
                    ui->lineEdit_CONFIG0_DT->setText(QString("%1").arg((config.motor.config_0 >> 0) & 0x3F));

                    ui->comboBox_CONFIG1_PFD->setCurrentIndex((config.motor.config_1 >> 11) & 0x01);
                    ui->comboBox_CONFIG1_IPI->setCurrentIndex((config.motor.config_1 >> 10) & 0x01);
                    ui->lineEdit_CONFIG1_VIL->setText(QString("%1").arg((config.motor.config_1 >> 6) & 0x0F));
                    ui->comboBox_CONFIG1_VDQ->setCurrentIndex((config.motor.config_1 >> 5) & 0x01);
                    ui->lineEdit_CONFIG1_VT->setText(QString("%1").arg((config.motor.config_1 >> 0) & 0x1F));

                    ui->lineEdit_CONFIG2_CP->setText(QString("%1").arg((config.motor.config_2 >> 8) & 0x0F));
                    ui->comboBox_CONFIG2_SH->setCurrentIndex((config.motor.config_2 >> 6) & 0x03);
                    ui->comboBox_CONFIG2_DGC ->setCurrentIndex((config.motor.config_2 >> 5) & 0x01);
                    ui->lineEdit_CONFIG2_PW->setText(QString("%1").arg((config.motor.config_2 >> 0) & 0x1F));

                    ui->lineEdit_CONFIG3_CI->setText(QString("%1").arg((config.motor.config_3 >> 8) & 0x0F));
                    ui->lineEdit_CONFIG3_HD->setText(QString("%1").arg((config.motor.config_3 >> 4) & 0x0F));
                    ui->lineEdit_CONFIG3_HT->setText(QString("%1").arg((config.motor.config_3 >> 0) & 0x0F));

                    ui->lineEdit_CONFIG4_SP->setText(QString("%1").arg((config.motor.config_4 >> 8) & 0x0F));
                    ui->lineEdit_CONFIG4_SD->setText(QString("%1").arg((config.motor.config_4 >> 4) & 0x0F));
                    ui->lineEdit_CONFIG4_SS->setText(QString("%1").arg((config.motor.config_4 >> 0) & 0x0F));

                    ui->lineEdit_CONFIG5_SI->setText(QString("%1").arg((config.motor.config_5 >> 8) & 0x0F));
                    ui->comboBox_CONFIG5_SPO->setCurrentIndex((config.motor.config_5 >> 7) & 0x01);
                    ui->lineEdit_CONFIG5_SMX->setText(QString("%1").arg((config.motor.config_5 >> 4) & 0x07));
                    ui->lineEdit_CONFIG5_PA->setText(QString("%1").arg((config.motor.config_5 >> 0) & 0x0F));

                    ui->comboBox_RUN_CM->setCurrentIndex((config.motor.run >> 10) & 0x03);
                    ui->comboBox_RUN_ESF->setCurrentIndex((config.motor.run >> 9) & 0x01);
                    ui->lineEdit_RUN_DI->setText(QString("%1").arg((config.motor.run >> 4) & 0x1F));
                    ui->comboBox_RUN_RSC->setCurrentIndex((config.motor.run >> 3) & 0x01);
                    ui->comboBox_RUN_BRK->setCurrentIndex((config.motor.run >> 2) & 0x01);
                    ui->comboBox_RUN_DIR->setCurrentIndex((config.motor.run >> 1) & 0x01);
                    ui->comboBox_RUN_RUN->setCurrentIndex((config.motor.run >> 0) & 0x01);

                    QMessageBox msgBox;
                    msgBox.setText("Config data successfully read.");
                    msgBox.exec();
                }
            } else {
                // Read anything other here
            }

            to_read = READ_NONE;
        }
    } else if ( state == WRITE ) {
        if ( frame.can_id == transmitted_id + 0x100 ) {
            if ( frame.can_dlc >=2 ) {
                uint16_t tmp = ((uint16_t)frame.data[0]) << 8 | frame.data[1];

                if ( tmp == transmit_index ) {
                    // TODO: Check for data bytes

                    if ( (transmit_index - 1) * 6 + frame.can_dlc - 2 == transmit_length ) {
                        // Data is completely transfered
                        // Wait for final ack.

                        state = READ;
                        to_read = READ_FINAL_ACK_WRITE;

                        // Restart timer
                        timer->stop();
                        timer->start(TIMEOUT_INTERVAL);
                        return;

                    } else {
                        transmit_index++;
                        write_next_frame();
                        // Restart timer
                        timer->stop();
                        timer->start(TIMEOUT_INTERVAL);
                        return;
                    }
                } else {

                }
            }
        }
    } else {

    }
}

void DialogQuadI::on_pushButton_Read_clicked()
{
    if ( state != NONE ) {
        QMessageBox msgBox;
        msgBox.setText("Transmission in progress...");
        msgBox.exec();
        return;
    }

    if ( can == NULL ) {
        QMessageBox msgBox;
        msgBox.setText("Not connected.");
        msgBox.exec();
        return;
    }

    for ( int i=0; i<MAX_BUFFER; i++ ) {
        recv_buf[i] = 0x00;
        recv_full[i] = false;
    }

    recv_dat_length = -1;
    transmitted_id = 0x501;

    uint8_t data = 0x01;

    can->send_can(0x501, false, false, 1, (uint8_t*)&data);
    state = READ;
    to_read = READ_CONFIG;

    timer->start(TIMEOUT_INTERVAL);

}

void DialogQuadI::on_pushButton_store_clicked()
{
    if ( state != NONE ) {
        QMessageBox msgBox;
        msgBox.setText("Transmission in progress...");
        msgBox.exec();
        return;
    }

    if ( can == NULL ) {
        QMessageBox msgBox;
        msgBox.setText("Not connected.");
        msgBox.exec();
        return;
    }

    // Set to -1, no data are received right now
    recv_dat_length=-1;
    transmitted_id = 0x503;
    uint8_t data = 0x01;
    can->send_can(0x503, false, false, 1, (uint8_t*)&data);
    state = READ;
    to_read = READ_ACK_CONFIG_STORE;

    timer->start(TIMEOUT_INTERVAL);
}

#define CONV_NOT_OK(x) \
        if ( !ok ) { \
            QMessageBox msgBox; \
            msgBox.setText(x); \
            msgBox.exec(); \
            return; \
        }

void DialogQuadI::on_pushButton_write_clicked()
{

    if ( state != NONE ) {
        QMessageBox msgBox;
        msgBox.setText("Transmission in progress...");
        msgBox.exec();
        return;
    }

    if ( can == NULL ) {
        QMessageBox msgBox;
        msgBox.setText("Not connected.");
        msgBox.exec();
        return;
    }

    struct QuadI_config config_tmp;
    memset((void*)&config_tmp, 0, sizeof(struct QuadI_config));

    config_tmp.header.major = CONFIG_MAJOR;
    config_tmp.header.minor = CONFIG_MINOR;
    config_tmp.header.reserved_1 = 0x00;
    config_tmp.header.reserved_2 = 0x00;
    config_tmp.header.reserved_3 = 0x00;
    config_tmp.header.reserved_4 = 0x00;
    config_tmp.header.size = sizeof(struct QuadI_config);

    bool ok;

#define GET_NUM_LINEEDIT(l) l->text().toLong(&ok)
#define GET_NUM_DROPBOX(l) l->currentIndex()

    config_tmp.can.bitrate = GET_NUM_LINEEDIT(ui->lineEdit_CAN_bitrate); CONV_NOT_OK("Not a number in CAN bitrate");
    config_tmp.i2c.bitrate = GET_NUM_LINEEDIT(ui->lineEdit_I2C_bitrate); CONV_NOT_OK("Not a number in I2C bitrate");

    config_tmp.motor.pwm_frequency = GET_NUM_LINEEDIT(ui->lineEdit_MotorDriverFreq); CONV_NOT_OK("Not a number in Motor driver frequency");

    config_tmp.motor.config_0 = 0;
    config_tmp.motor.config_0 |= GET_NUM_DROPBOX(ui->comboBox_CONFIG0_RM) << 10;
    config_tmp.motor.config_0 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG0_BT) & 0x0F) << 6 ; CONV_NOT_OK("Not a number in Config 0 BT");
    config_tmp.motor.config_0 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG0_DT) & 0x3F) << 0; CONV_NOT_OK("Not a number in Config 0 DT");

    config_tmp.motor.config_1 = 0;
    config_tmp.motor.config_1 |= GET_NUM_DROPBOX(ui->comboBox_CONFIG1_PFD) << 11;
    config_tmp.motor.config_1 |= GET_NUM_DROPBOX(ui->comboBox_CONFIG1_IPI) << 10;
    config_tmp.motor.config_1 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG1_VIL) & 0x0F) << 6 ; CONV_NOT_OK("Not a number in Config 1 VIL");
    config_tmp.motor.config_1 |= GET_NUM_DROPBOX(ui->comboBox_CONFIG1_VDQ) << 5;
    config_tmp.motor.config_1 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG1_VT) & 0x1F) << 0 ; CONV_NOT_OK("Not a number in Config 1 VT");

    config_tmp.motor.config_2 = 0;
    config_tmp.motor.config_2 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG2_CP) & 0x0F) << 8 ; CONV_NOT_OK("Not a number in Config 2 CP");
    config_tmp.motor.config_2 |= GET_NUM_DROPBOX(ui->comboBox_CONFIG2_SH) << 6;
    config_tmp.motor.config_2 |= GET_NUM_DROPBOX(ui->comboBox_CONFIG2_DGC) << 5;
    config_tmp.motor.config_2 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG2_PW) & 0x1F) << 0 ; CONV_NOT_OK("Not a number in Config 2 PW");

    config_tmp.motor.config_3 = 0;
    config_tmp.motor.config_3 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG3_CI) & 0x0F) << 8 ; CONV_NOT_OK("Not a number in Config 3 CI");
    config_tmp.motor.config_3 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG3_HD) & 0x0F) << 4 ; CONV_NOT_OK("Not a number in Config 3 HD");
    config_tmp.motor.config_3 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG3_HT) & 0x0F) << 0 ; CONV_NOT_OK("Not a number in Config 3 HT");

    config_tmp.motor.config_4 = 0;
    config_tmp.motor.config_4 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG4_SP) & 0x0F) << 8 ; CONV_NOT_OK("Not a number in Config 4 SP");
    config_tmp.motor.config_4 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG4_SD) & 0x0F) << 4 ; CONV_NOT_OK("Not a number in Config 4 SD");
    config_tmp.motor.config_4 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG4_SS) & 0x0F) << 0 ; CONV_NOT_OK("Not a number in Config 4 SS");

    config_tmp.motor.config_5 = 0;
    config_tmp.motor.config_5 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG5_SI) & 0x0F) << 8 ; CONV_NOT_OK("Not a number in Config 5 SI");
    config_tmp.motor.config_5 |= GET_NUM_DROPBOX(ui->comboBox_CONFIG5_SPO) << 7;
    config_tmp.motor.config_5 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG5_SMX) & 0x07) << 4 ; CONV_NOT_OK("Not a number in Config 5 SMX");
    config_tmp.motor.config_5 |= (GET_NUM_LINEEDIT(ui->lineEdit_CONFIG5_PA) & 0x0F) << 0 ; CONV_NOT_OK("Not a number in Config 5 PA");

    config_tmp.motor.run = 0;
    config_tmp.motor.run |= GET_NUM_DROPBOX(ui->comboBox_RUN_CM) << 10;
    config_tmp.motor.run |= GET_NUM_DROPBOX(ui->comboBox_RUN_ESF) << 9;
    config_tmp.motor.run |= (GET_NUM_LINEEDIT(ui->lineEdit_RUN_DI) & 0x1F) << 4 ; CONV_NOT_OK("Not a number in RUN DI");
    config_tmp.motor.run |= GET_NUM_DROPBOX(ui->comboBox_RUN_RSC) << 3;
    config_tmp.motor.run |= GET_NUM_DROPBOX(ui->comboBox_RUN_BRK) << 2;
    config_tmp.motor.run |= GET_NUM_DROPBOX(ui->comboBox_RUN_DIR) << 1;
    config_tmp.motor.run |= GET_NUM_DROPBOX(ui->comboBox_RUN_RUN) << 0;

    config_tmp.checksum = CONFIG_calc_crc(&config_tmp);

    memcpy((void*)&trans_buf, (void*)&config_tmp, sizeof(struct QuadI_config));
    transmit_index = 1;
    transmit_length = sizeof(struct QuadI_config);

    recv_dat_length=-1;
    transmitted_id = 0x502;
    uint8_t data[3];

    data[0] = 0x01;
    data[1] = (sizeof(QuadI_config) >> 8 ) & 0xFF;
    data[2] = sizeof(QuadI_config) & 0xFF;

    can->send_can(0x502, false, false, 3, (uint8_t*)&data);
    state = READ;
    to_read = READ_ACK_WRITE;

    timer->start(TIMEOUT_INTERVAL);

}

void DialogQuadI::on_checkBox_HWT_clicked() {
    if ( state != NONE ) {
        QMessageBox msgBox;
        msgBox.setText("Transmission in progress...");
        msgBox.exec();
        return;
    }

    if ( can == NULL ) {
        QMessageBox msgBox;
        msgBox.setText("Not connected.");
        msgBox.exec();
        return;
    }

    uint8_t data;
    if ( ui->checkBox_HWT->isChecked() ) {
        data = 0x01;
    } else {
        data = 0x00;
    }
    can->send_can(0x401, false, false, 1, (uint8_t*)&data);
}

void DialogQuadI::SetMotorSpeed(double speed, uint16_t motor) {
    if ( state != NONE ) {
        QMessageBox msgBox;
        msgBox.setText("Transmission in progress...");
        msgBox.exec();
        return;
    }

    if ( can == NULL ) {
        QMessageBox msgBox;
        msgBox.setText("Not connected.");
        msgBox.exec();
        return;
    }


    uint8_t data[4];

    data[0] = 0x01;
    data[1] = motor;

    uint16_t temp = (uint16_t)(speed * 100.0);
    data[2] = (temp >> 8) & 0xFF;
    data[3] = temp & 0xFF;

    // Set to -1, no data are received right now
    recv_dat_length=-1;
    transmitted_id = 0x504;
    can->send_can(0x504, false, false, 4, (uint8_t*)&data);
    state = READ;
    to_read = READ_ACK_SET_SPEED;

    timer->start(TIMEOUT_INTERVAL);
}

void DialogQuadI::on_doubleSpinBox_Motor1_valueChanged(double arg1)
{
    SetMotorSpeed(arg1, 1);
}

void DialogQuadI::on_doubleSpinBox_Motor2_valueChanged(double arg1)
{
    SetMotorSpeed(arg1, 2);
}

void DialogQuadI::on_doubleSpinBox_Motor3_valueChanged(double arg1)
{
    SetMotorSpeed(arg1, 3);
}

void DialogQuadI::on_doubleSpinBox_Motor4_valueChanged(double arg1)
{
    SetMotorSpeed(arg1, 4);
}
