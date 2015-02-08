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

#ifdef USE_SOCKET_CAN

#ifndef CSOCKETCAN_H
#define CSOCKETCAN_H

#include "ccancomm.h"

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>



class CSocketCAN : public CCanComm
{
    Q_OBJECT

public:
    CSocketCAN();
    ~CSocketCAN();
    bool open_can(QString can_device);
    void close_can();
    bool send_can(int64_t id, bool ext, bool rtr, uint8_t dlc, uint8_t *data);

protected:
    void run();

private:
    struct ifreq ifr;
    struct sockaddr_can addr;
    struct can_frame frame;
    int s;
    bool running;

};

#endif // CSOCKETCAN_H

#endif
