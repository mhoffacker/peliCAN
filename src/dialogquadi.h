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

#ifndef DIALOGQUADI_H
#define DIALOGQUADI_H

#include <QDialog>

#include "ccancomm.h"

enum trans_state { NONE, READ, WRITE };
enum read_state { READ_NONE, READ_CONFIG, READ_ACK_CONFIG_STORE, READ_ACK_WRITE, READ_FINAL_ACK_WRITE, READ_ACK_SET_SPEED };

#define MAX_BUFFER  4096

// 1 sec. timeout
#define TIMEOUT_INTERVAL 1000

#if !defined(__ORDER_BIG_ENDIAN__)
#error Wrong endianess
#endif

#define CONFIG_MAJOR    0
#define CONFIG_MINOR    0x01

#define CAN_ID_ACCEL_RAW	0x301
#define CAN_ID_GYRO_RAW		0x302
#define CAN_ID_STATUS_MSG	0x308

typedef struct QuadI_config {
    uint32_t checksum;	// Must be the first 32bit in structure!

    struct {
        uint8_t major;
        uint8_t minor;
        uint64_t size;
        uint8_t reserved_1;
        uint8_t reserved_2;
        uint8_t reserved_3;
        uint8_t reserved_4;
    } header;

    struct {
        uint32_t bitrate;
    } can;

    struct {
        uint32_t bitrate;
    } i2c;

    struct {
        uint32_t config_0;
        uint32_t config_1;
        uint32_t config_2;
        uint32_t config_3;
        uint32_t config_4;
        uint32_t config_5;
        uint32_t run;
        uint32_t pwm_frequency;
    } motor;
} QuadI_config_t, *QuadI_config_p;

namespace Ui {
class DialogQuadI;
}

class DialogQuadI : public QDialog
{
    Q_OBJECT

public:
    explicit DialogQuadI(QWidget *parent = 0);
    ~DialogQuadI();

    void setCanComm(CCanComm *c);
    void can_received(const decoded_can_frame &frame);

private slots:
    void on_pushButton_Read_clicked();

    void recv_timeout();

    void on_pushButton_store_clicked();

    void on_pushButton_write_clicked();

    void on_checkBox_HWT_clicked();

    void on_doubleSpinBox_Motor1_valueChanged(double arg1);

    void on_doubleSpinBox_Motor2_valueChanged(double arg1);

    void on_doubleSpinBox_Motor3_valueChanged(double arg1);

    void on_doubleSpinBox_Motor4_valueChanged(double arg1);

private:
    Ui::DialogQuadI *ui;

    CCanComm *can;
    trans_state state;
    read_state to_read;
    QTimer *timer;

    // 4k data recv buffer for transmissions
    uint8_t recv_buf[MAX_BUFFER];
    // 4k to flag if data was read
    bool recv_full[MAX_BUFFER];


    // 4k data transmit buffer
    void write_next_frame();
    uint8_t trans_buf[MAX_BUFFER];
    uint16_t transmit_index;
    uint16_t transmit_length;


    // ID of last issued transmission
    uint16_t transmitted_id;

    // length of data
    int32_t recv_dat_length;

    struct QuadI_config config;

    uint32_t crcTable[256];

    void CONFIG_crc_init(void);
    uint32_t CONFIG_calc_crc(struct QuadI_config *ptr);

    void SetMotorSpeed(double speed, uint16_t motor);
};

#endif // DIALOGQUADI_H
