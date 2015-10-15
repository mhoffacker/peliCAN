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
#ifndef CCANCOMM_H
#define CCANCOMM_H

#include <QThread>
#include <QMetaType>
#include <sys/time.h>
#include <inttypes.h>

#define CAN_MAX_DLEN 8


/*!
 * \brief The decoded_can_frame struct
 * Holds the received data from a communication. Similar to the SocketCAN
 * can_frame structure. EFF, ERR, RTR are here as bool, not as bitmasks in
 * can_id
 */
struct decoded_can_frame {
    //! 32 bit CAN ID
    unsigned long can_id;
    //! frame payload length in byte (0 .. CAN_MAX_DLEN)
    unsigned char    can_dlc;
    //! frame payload
    unsigned char    data[CAN_MAX_DLEN] __attribute__((aligned(8)));
    //! flag for extended ID
    bool EFF;
    //! flag for remote transmission request
    bool RTR;
    //! flag for error
    bool ERR;
    //! time of reception
    struct timeval tv_ioctl;
    //! time of reception as QString
    QString time_as_str;
};

/*!
 * \brief The CCanComm class
 * Prototype class for communication. Inherit a communication interface from
 * this class.
 */
class CCanComm : public QThread
{
    Q_OBJECT
public:
    CCanComm();
    virtual ~CCanComm();

    virtual bool open_can(QString can_device) = 0;
    virtual void close_can() = 0;

    virtual bool send_can(int64_t id, bool ext, bool rtr, uint8_t dlc, uint8_t *data) = 0;

signals:
    /*!
     * \brief received Signal emitted after a frame is received
     * Connect in the application to this signal. The signal is emitted,
     * after a new valid frame has been received.
     * \param frame The decoded CAN frame
     */
    void received(const decoded_can_frame& frame);
};

#endif // CCANCOMM_H
