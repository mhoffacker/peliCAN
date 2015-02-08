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

#include "cslcan.h"
#include <QStringList>

#include <QDebug>



// Silence compiler warning if a function return value is not used
#define NO_WARNING_UNUSED_RESULT(x) if((x)){};


CSLCAN::CSLCAN()
{
    running = false;

}

CSLCAN::~CSLCAN()
{
    close_can();
}

int CSLCAN::WriteString(QString write, int timeout_ms)
{

    int i;
    i = serial.write(write.toStdString().c_str(), write.length());

    if ( i == -1 )
    {
        return -1;
    }

    if ( i == write.length() )
    {
        return i;
    }

    serial.flush();

    if ( !serial.waitForBytesWritten(timeout_ms) )
    {
        return -1;
    }


    return i;

}

bool CSLCAN::ReadChar(char *c, int timeout_ms)
{

    serial.waitForReadyRead(timeout_ms);

    if ( serial.read(c, 1) == 1 )
    {
        return true;
    }

    return false;
}

bool CSLCAN::parse_open(QString open)
{
    QStringList list = open.split(",");

    if ( list.size() != 7 )
        return false;

    _port = list[0];
    _baudrate = list[1].toLong();
    _databytes = list[2].toInt();
    _parity = list[3].toUpper();
    _stopbits = list[4].toInt();
    _canspeed = list[5].toLong();
    _loopback = list[6] == "l";

    return true;
}

bool CSLCAN::open_can(QString can_device)
{
    char r;

    QString CR = "\x0D";
    QString VersionString = "V\x0D";
    QString SpeedString = "Sx\x0D";
    QString OpenString = "O\x0D";
    QString LoopBackString = "I\x0D";
    QString CloseString = "C\x0D";

    if ( !parse_open(can_device) )
        return false;

    serial.setPortName(_port);
    serial.setBaudRate(_baudrate);
    switch ( _databytes )
    {
    case 5: serial.setDataBits(QSerialPort::Data5); break;
    case 6: serial.setDataBits(QSerialPort::Data6); break;
    case 7: serial.setDataBits(QSerialPort::Data7); break;
    case 8: serial.setDataBits(QSerialPort::Data8); break;
    default:
        break;
    }

    if ( _parity == "N")
        serial.setParity(QSerialPort::NoParity);
    else if ( _parity == "E" )
        serial.setParity(QSerialPort::EvenParity);
    else if ( _parity == "O" )
        serial.setParity(QSerialPort::OddParity);

    if ( _stopbits == 1)
        serial.setStopBits(QSerialPort::OneStop);
    else if ( _stopbits == 2 )
        serial.setStopBits(QSerialPort::TwoStop);


    if ( !serial.open(QSerialPort::ReadWrite) )
        return false;

    // Close
    NO_WARNING_UNUSED_RESULT( WriteString(CloseString) );


    // Reset by sending 0x0D three times
    for ( int i=0; i<3; i++ )
        if ( WriteString(CR) != 1 )
        {
            close_can();
            return false;
        }

    // Send version string
    if ( WriteString(VersionString) != 2 )
    {
        close_can();
        return false;
    }

    // Wait for reply
    while ( r != 'V' )
    {
        if ( !ReadChar(&r, 100) )
        {
            close_can();
            return false;
        }
    }

    // Empty buffer
    while ( r != 0x0D )
    {
        if ( !ReadChar(&r, 100) )
        {
            close_can();
            return false;
        }
    }

    switch ( _canspeed )
    {
    case 10000: SpeedString[1] = '0'; break;
    case 20000: SpeedString[1] = '1'; break;
    case 50000: SpeedString[1] = '2'; break;
    case 100000: SpeedString[1] = '3'; break;
    case 125000: SpeedString[1] = '4'; break;
    case 250000: SpeedString[1] = '5'; break;
    case 500000: SpeedString[1] = '6'; break;
    case 800000: SpeedString[1] = '7'; break;
    case 1000000: SpeedString[1] = '8'; break;
    default: close_can(); return false;
    }

    if (  WriteString(SpeedString) != 3 )
    {
        close_can();
        return false;
    }


    if ( !ReadChar(&r, 100) )
    {
        close_can();
        return false;
    }

    if ( r != 0x0D )
    {
        close_can();
        return false;
    }

    if ( !_loopback )
    {
        if ( WriteString(OpenString) != 2 )
        {
            close_can();
            return false;
        }
    } else {
        if ( WriteString(LoopBackString) != 2 )
        {
            close_can();
            return false;
        }
    }


    running = true;
    start();

    return true;
}

void CSLCAN::close_can()
{
    QString CloseString = "C\x0D";



    running = false;
    terminate();

    while ( isRunning() );
    NO_WARNING_UNUSED_RESULT( WriteString(CloseString) );

    serial.close();

}

enum SLCAN_Thread_States
{
    WAIT_0x0D = 0,
    WAIT_COMMAND,
    DECODE_SID,
    DECODE_EID,
    DECODE_DLC,
    DECODE_DATA
};

void CSLCAN::run()
{
    struct decoded_can_frame d_frame;
    SLCAN_Thread_States state = WAIT_0x0D;

    char r;
    int id_cnt = 0;
    int dat_cnt = 0;
    int dat_index = 0;

    time_t nowtime;
    char tmbuf[64], buf[64];
    struct tm *nowtm;

    while ( running )
    {
        if ( ! queue.isEmpty() )
            WriteString(queue.dequeue());

        if ( ReadChar(&r, 0) )
        {
            switch ( state )
            {
            case WAIT_0x0D:
                if ( r == 0x0D )
                {
                    memset(d_frame.data, 0, sizeof(d_frame.data));
                    d_frame.can_id = 0;
                    id_cnt = 0;
                    dat_cnt = 0;
                    dat_index = 0;

                    state = WAIT_COMMAND;
                }
                break;

            case WAIT_COMMAND:
                // There might be a 0x0D after sending sth. Do dismiss this 0x0D
                if ( r == 0x0D )
                    continue;

                if ( r == 't' ) // Receive SID
                {
                    d_frame.EFF = false;
                    d_frame.ERR = false;
                    d_frame.RTR = false;
                    gettimeofday(&d_frame.tv_ioctl, 0);
                    state = DECODE_SID;
                } else if ( r == 'T' )
                {
                    d_frame.EFF = true;
                    d_frame.ERR = false;
                    d_frame.RTR = false;
                    gettimeofday(&d_frame.tv_ioctl, 0);
                    state = DECODE_EID;
                } else if ( r == 'r' )
                {
                    d_frame.EFF = false;
                    d_frame.ERR = false;
                    d_frame.RTR = true;
                    gettimeofday(&d_frame.tv_ioctl, 0);
                    state = DECODE_SID;
                } else if ( r == 'R' )
                {
                    d_frame.EFF = true;
                    d_frame.ERR = false;
                    d_frame.RTR = true;
                    gettimeofday(&d_frame.tv_ioctl, 0);
                    state = DECODE_EID;
                } else {
                    state = WAIT_0x0D;
                    break;
                }
                nowtime = d_frame.tv_ioctl.tv_sec;
                nowtm = localtime(&nowtime);
                strftime(tmbuf, sizeof(tmbuf), "%H:%M:%S", nowtm);
                snprintf(buf, sizeof(buf), "%s.%06ld", tmbuf, d_frame.tv_ioctl.tv_usec);

                d_frame.time_as_str = QString::fromLocal8Bit(buf);
                break;

            case DECODE_SID:
            case DECODE_EID:
                d_frame.can_id = d_frame.can_id << 4;
                if ( r >= '0' && r <= '9')
                    d_frame.can_id = d_frame.can_id + r - '0';
                else if ( r >= 'a' && r <= 'f')
                    d_frame.can_id = d_frame.can_id + r - 'a' + 10;
                else if ( r >= 'A' && r <= 'F')
                    d_frame.can_id = d_frame.can_id + r - 'A' + 10;
                else {
                    state = WAIT_0x0D;
                    break;
                }
                id_cnt++;

                if ( (state == DECODE_SID && id_cnt == 3) || (state == DECODE_EID && id_cnt == 8) )
                    state = DECODE_DLC;
                break;

            case DECODE_DLC:
                if ( r >= '0' && r <= '8')
                    d_frame.can_dlc = r - '0';
                else {
                    state = WAIT_0x0D;
                    break;
                }

                if ( d_frame.RTR )
                {
                    emit received(d_frame);
                    state = WAIT_0x0D;
                } else
                    state = DECODE_DATA;
                break;

            case DECODE_DATA:
                d_frame.data[dat_cnt] = d_frame.data[dat_cnt] << 4;
                if ( r >= '0' && r <= '9')
                    d_frame.data[dat_cnt] = d_frame.data[dat_cnt] + r - '0';
                else if ( r >= 'a' && r <= 'f')
                    d_frame.data[dat_cnt] = d_frame.data[dat_cnt] + r - 'a' + 10;
                else if ( r >= 'A' && r <= 'F')
                    d_frame.data[dat_cnt] = d_frame.data[dat_cnt] + r - 'A' + 10;
                else {
                    state = WAIT_0x0D;
                    break;
                }
                dat_index++;

                if ( dat_index == 2 )
                {
                    dat_index = 0;
                    dat_cnt++;
                }

                if ( dat_cnt == d_frame.can_dlc )
                {
                    emit received(d_frame);
                    state = WAIT_0x0D;
                }
                break;

            default:
                state = WAIT_0x0D;
                break;

            }
        }
    }
}

bool CSLCAN::send_can(int64_t id, bool ext, bool rtr, uint8_t dlc, uint8_t *data)
{
    QString s;

    if ( dlc > 8 )
        return false;


    if ( !ext && !rtr )
        s = tr("t");        // Transmit SID
    else if ( ext && !rtr )
        s = tr("T");        // Transmit EID
    else if ( !ext && rtr )
        s = tr("r");        // Remote tramsmission request with SID
    else if ( ext && rtr )
        s = tr("R");        // Remote transmission request with EID


    if ( ext )              // Add ID for EID
        s = s + QString("%1").arg(id, 8, 16, QChar('0')).toUpper();
    else
        s = s + QString("%1").arg(id, 3, 16, QChar('0')).toUpper();

    s = s + QString("%1").arg(dlc, 1, 16, QChar('0')).toUpper();

    if ( !rtr )
        for ( uint8_t i = 0; i<dlc; i++ )
            s = s + QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();

    s = s + tr("\x0D");

    //int i = s.length();

    queue.enqueue(s);
    //if ( WriteString(s) != i )
    //{
    //    return false;
    //}

    return true;
}
