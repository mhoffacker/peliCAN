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

#ifdef USE_SOCKET_CAN

#include "csocketcan.h"

#include <unistd.h>
#include <stdio.h>


CSocketCAN::CSocketCAN()
{
    memset(&ifr, 0x00, sizeof(ifr));
    memset(&addr, 0x00, sizeof(addr));
    memset(&frame, 0x00, sizeof(frame));
    s=-1;
    running = false;
}

CSocketCAN::~CSocketCAN()
{
    close_can();
}

bool CSocketCAN::open_can(QString can_device)
{
    memset(&ifr, 0x00, sizeof(ifr));
    memset(&addr, 0x00, sizeof(addr));
    memset(&frame, 0x00, sizeof(frame));

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if ( s < 0 )
        return false;

    addr.can_ifindex = if_nametoindex(can_device.toUtf8().constData());
    addr.can_family = PF_CAN;

    if ( bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0 )
        return false;

    running = true;
    start();

    return true;
}

void CSocketCAN::close_can()
{
    if ( s < 0 )
        return;

    running = false;
    shutdown(s, SHUT_RDWR);
    close(s);
    s = -1;
    terminate();

    while ( isRunning() );
}

void CSocketCAN::run()
{
    struct can_frame frame;
    struct decoded_can_frame d_frame;
    int nbytes;

    time_t nowtime;
    char tmbuf[64], buf[64];
    struct tm *nowtm;

    while ( running )
    {
        nbytes = read(s, &frame, sizeof(struct can_frame));
        if ( nbytes == sizeof(struct can_frame) ) //Valid frame read
        {
            //Get time stamp
            if ( ioctl(s, SIOCGSTAMP, &d_frame.tv_ioctl) == -1 )
                memset(&d_frame.tv_ioctl, 0x00, sizeof(struct timeval));

            if ( (frame.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG )
            {
                d_frame.EFF = true;
                d_frame.can_id = frame.can_id & CAN_EFF_MASK;
            } else {
                d_frame.EFF = false;
                d_frame.can_id = frame.can_id & CAN_SFF_MASK;
            }

            if ( (frame.can_id & CAN_RTR_FLAG) == CAN_RTR_FLAG )
            {
                d_frame.RTR = true;
            } else {
                d_frame.RTR = false;
            }

            if ( (frame.can_id & CAN_ERR_FLAG) == CAN_ERR_FLAG )
            {
                d_frame.ERR = true;
            } else {
                d_frame.ERR = false;
            }

            d_frame.can_dlc = frame.can_dlc;
            memcpy(d_frame.data, frame.data, sizeof(frame.data));

            nowtime = d_frame.tv_ioctl.tv_sec;
            nowtm = localtime(&nowtime);
            strftime(tmbuf, sizeof(tmbuf), "%H:%M:%S", nowtm);
            snprintf(buf, sizeof(buf), "%s.%06ld", tmbuf, d_frame.tv_ioctl.tv_usec);

            d_frame.time_as_str = QString::fromLocal8Bit(buf);

            emit received(d_frame);
        }
    }
}

bool CSocketCAN::send_can(int64_t id, bool ext, bool rtr, uint8_t dlc, uint8_t *data)
{
    struct can_frame frame;
    frame.can_id = id;

    if ( ext )
        frame.can_id |= CAN_EFF_FLAG;

    if ( rtr )
        frame.can_id |= CAN_RTR_FLAG;

    frame.can_dlc = dlc;

    memcpy(frame.data, data, dlc);

    if ( send(s, (void *)&frame, sizeof(struct can_frame), 0) < 0 )
        return false;

    return true;
}

#endif
