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


#if !defined(__WINDOWS__)
#define __write(__fd, __buf, __nbytes) write(__fd, __buf, __nbytes)
#define __read(__fd, __buf, __nbytes) read(__fd, __buf, __nbytes)
#define __close(__fd) close(__fd)
#else
int __write(HANDLE __fd, const void *__buf, size_t __nbytes)
{
    long unsigned int written;

    if ( !WriteFile(__fd, __buf, __nbytes, &written, NULL) )
    {
        DWORD e = GetLastError();
        return -1;
    }


    return written;
}

int __read(HANDLE __fd, void *__buf, size_t __nbytes)
{
    long unsigned int r;

    if ( !ReadFile(__fd, __buf, __nbytes, &r, NULL) )
    {
        DWORD e = GetLastError();
        return -1;
    }

    return r;
}

#define __close(__fd) CloseHandle(__fd)
#endif

CSLCAN::CSLCAN()
{
    running = false;
#if !defined(__WINDOWS__)
    tty_fd = -1;
#else
    tty_fd = NULL;
#endif
}

CSLCAN::~CSLCAN()
{
    close_can();
}

bool CSLCAN::parse_open(QString open)
{
    QStringList list = open.split(",");

    if ( list.size() != 6 )
        return false;

    _port = list[0];                // *
    _baudrate = list[1].toLong();   // *
    _databytes = list[2].toInt();   // *
    _parity = list[3];              // *
    _stopbits = list[4].toInt();    // *
    _canspeed = list[5].toLong();

    return true;
}

bool CSLCAN::open_can(QString can_device)
{
    unsigned char r;

    unsigned char CR = 0x0D;
    unsigned char VersionString[] = "V\x0D";
    unsigned char SpeedString[] = "Sx\x0D";
    unsigned char OpenString[] = "O\x0D";
    unsigned char CloseString[] = "C\x0D";

    if ( !parse_open(can_device) )
        return false;

#if !defined(__WINDOWS__)
    memset(&tio, 0x00, sizeof(struct termios));

    tio.c_iflag=0;
    tio.c_oflag=0;

    tio.c_cflag=CREAD|CLOCAL;
    switch ( _databytes )
    {
    case 5: tio.c_cflag|=CS5; break;
    case 6: tio.c_cflag|=CS6; break;
    case 7: tio.c_cflag|=CS7; break;
    case 8: tio.c_cflag|=CS8; break;
    default:
        return false;
    }

    switch ( _stopbits )
    {
    case 1: break;
    case 2: tio.c_cflag|=CSTOPB; break;
    default:
        return false;
    }

    if ( _parity == "N" )
    {

    } else if ( _parity == "O" )
    {
        tio.c_cflag |= PARENB;
        tio.c_cflag |= PARODD;
    } else if ( _parity == "E" )
    {
        tio.c_cflag |= PARENB;
    } else {
        return false;
    }

    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;  // set inter byte timer to 0.5 secs

    tty_fd = open(_port.toStdString().c_str(), O_RDWR ); // Read/write blocking mode

    if ( tty_fd < 0 )
        return false;



    speed_t speed;

    switch ( _baudrate )
    {
    case 0: speed = B0; break;
    case 50: speed = B50; break;
    case 75: speed = B75; break;
    case 110: speed = B110; break;
    case 134: speed = B134; break;
    case 150: speed = B150; break;
    case 200: speed = B200; break;
    case 300: speed = B300; break;
    case 600: speed = B600; break;
    case 1200: speed = B1200; break;
    case 1800: speed = B1800; break;
    case 2400: speed = B2400; break;
    case 4800: speed = B4800; break;
    case 9600: speed = B9600; break;
    case 19200: speed = B19200; break;
    case 38400: speed = B38400; break;
    case 57600: speed = B57600; break;
    case 115200: speed = B115200; break;
    case 230400: speed = B230400; break;
    case 460800: speed = B460800; break;
    case 500000: speed = B500000; break;
    case 576000: speed = B576000; break;
    case 921600: speed = B921600; break;
    case 1000000: speed = B1000000; break;
    case 1152000: speed = B1152000; break;
    case 2000000: speed = B2000000; break;
    case 2500000: speed = B2500000; break;
    case 3000000: speed = B3000000; break;
    case 3500000: speed = B3500000; break;
    case 4000000: speed = B4000000; break;

    default:
        close(tty_fd);
        return false;
    }

    if ( cfsetispeed(&tio, speed) < 0 ||  cfsetospeed(&tio, speed) < 0)
    {
        close(tty_fd);
        return false;
    }


    if ( tcsetattr(tty_fd, TCSANOW, &tio) == -1 )
    {
        close(tty_fd);
        return false;
    }

#else
    tty_fd = CreateFile( (LPCWSTR) _port.toStdWString().c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        0);
    if (tty_fd == INVALID_HANDLE_VALUE)
        return false;

    DCB dcb;
    ZeroMemory(&dcb, sizeof(DCB));

    QString buffer = QString("baud=%1 parity=%2 data=%3 stop=%4").arg(_baudrate).arg(_parity.toLower()).arg(_databytes).arg(_stopbits);

    BuildCommDCB(buffer.toStdWString().c_str(), &dcb);

    if(!SetCommState(tty_fd, &dcb))
        return false;

    COMMTIMEOUTS comTimeOut;
    comTimeOut.ReadIntervalTimeout = 3000;
    comTimeOut.ReadTotalTimeoutMultiplier = 3000;
    comTimeOut.ReadTotalTimeoutConstant = 2000;
    comTimeOut.WriteTotalTimeoutMultiplier = 3000;
    comTimeOut.WriteTotalTimeoutConstant = 2000;
    SetCommTimeouts(tty_fd,&comTimeOut);
#endif

    // Close
    __write(tty_fd, &CloseString, 2);

    // Reset by sending 0x0D three times
    for ( int i=0; i<3; i++ )
        if ( __write(tty_fd, &CR, 1) != 1 )
        {
            close_can();
            return false;
        }

    // Send version string
    if ( __write(tty_fd, VersionString, 2) != 2 )
    {
        close_can();
        return false;
    }

    // Wait for reply
    while ( r != 'V' )
    {
        if ( __read(tty_fd, &r, 1) == 0 )
        {
            close_can();
            return false;
        }
    }

    // Empty buffer
    while ( r != 0x0D )
    {
        if ( __read(tty_fd, &r, 1) == 0 )
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

    if ( __write(tty_fd, SpeedString, 3) != 3 )
    {
        close_can();
        return false;
    }

    if ( __read(tty_fd, &r, 1) != 1 )
    {
        close_can();
        return false;
    }

    if ( r != 0x0D )
    {
        close_can();
        return false;
    }

    if ( __write(tty_fd, OpenString, 2) != 2 )
    {
        close_can();
        return false;
    }

    /*if ( read(tty_fd, &r, 1) != 1 )
    {
        close_can();
        return false;
    }

    if ( r != 0x0D )
    {
        close_can();
        return false;
    }*/

    running = true;
    start();

    return true;
}

void CSLCAN::close_can()
{
    unsigned char CloseString[] = "C\x0D";

#if !defined(__WINDOWS__)
    if ( tty_fd < 0 )
        return;
#else
    if ( tty_fd == NULL )
        return;
#endif

    running = false;
    terminate();

    while ( isRunning() );
    __write(tty_fd, CloseString, 2);
    __close(tty_fd);

#if !defined(__WINDOWS__)
    tty_fd = -1;
#else
    tty_fd = NULL;
#endif

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

    unsigned char r;
    int id_cnt = 0;
    int dat_cnt = 0;
    int dat_index = 0;

    time_t nowtime;
    char tmbuf[64], buf[64];
    struct tm *nowtm;

    while ( running )
    {
        if ( __read(tty_fd, &r, 1) == 1 )
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

                if ( (state == DECODE_SID && id_cnt == 3) || (state == DECODE_EID && id_cnt == 7) )
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

