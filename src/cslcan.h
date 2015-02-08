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

#ifndef CSLCAN_H
#define CSLCAN_H

#include "ccancomm.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#if !defined(__WINDOWS__)
#include <termios.h>
#else
#include <windows.h>
#endif

#include <sys/time.h>

#include <QtSerialPort/QSerialPort>
#include <QMutex>
#include <QQueue>

class CSLCAN : public CCanComm
{
    Q_OBJECT

public:
    CSLCAN();
    ~CSLCAN();
    bool open_can(QString can_device);
    void close_can();
    bool send_can(int64_t id, bool ext, bool rtr, uint8_t dlc, uint8_t *data);

protected:
    void run();

private:
    bool parse_open(QString open);

    bool running;

    QSerialPort serial;

    int WriteString(QString write, int timeout_ms = 0);
    bool ReadChar(char *c, int timeout_ms = 0);

    QString _port;
    unsigned long _baudrate;
    unsigned int _databytes;
    QString _parity;
    unsigned int _stopbits;
    unsigned long _canspeed;
    bool _loopback;

    QQueue<QString> queue;
};

#endif // CSLCAN_H
