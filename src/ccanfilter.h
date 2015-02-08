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

#ifndef CCANFILTER_H
#define CCANFILTER_H

#include "ccancomm.h"

/*!
 * \brief The CCANFilter class
 * Checks with the bool match() function, if a CAN frame is accepted
 * - EID: Set to true to accept extended IDs
 * - SID: Set to true to accept standard IDs
 * - ID mask: Set a bit-mask to check for ID. (ID & ID_Mask == ID?)
 * - DLC: Accept only frames with given DLC. Disable mask with DLC == 0xFF
 * - RTR: Accept only RTR frames. Disable with RTR == 0xFF
 * - Data mask: Set a bit-mask for every data: (Data[n] & Data_Mask[n] == Data[n], n=0..7)
 */
class CCANFilter
{
public:
    CCANFilter();

    /*!
     * \brief Set EID acceptance
     * \param accept_eid Accept EID
     */
    void setEIDMask(bool accept_eid) { eid = accept_eid; }
    /*!
     * \brief Get EID acceptance
     * \return Accept EID
     */
    bool getEIDMask() { return eid; }

    /*!
     * \brief Set SID acceptance
     * \param Accept SID
     */
    void setSIDMask(bool accept_sid) { sid = accept_sid; }
    /*!
     * \brief Get SID acceptance
     * \return Accept SID
     */
    bool getSIDMask() { return sid; }

    /*!
     * \brief Set EID/SID bit-mask
     * \param accept_id EID/SID bit-mask
     */
    void setIDMask(quint32 accept_id) { id_mask = accept_id; }
    /*!
     * \brief Gets EID/SID bit-mask
     * \return EID/SID bit-mask
     */
    quint32 getIDMask() { return id_mask; }

    /*!
     * \brief Sets accepted DLC
     * \param accept_dlc Data length to accept. Disable with 0xFF
     */
    void setDLCMask(unsigned char accept_dlc) { dlc_mask = accept_dlc; }
    /*!
     * \brief Gets accepted DLC
     * \return Accepted data length
     */
    unsigned char getDLCMask() { return dlc_mask; }

    /*!
     * \brief Sets accepted RTR
     * \param accept_rtr Remote transmission request flag mask. Disable with 0xFF
     */
    void setRTRMask(unsigned char accept_rtr) { rtr_mask = accept_rtr; }
    /*!
     * \brief Gets accepted RTR
     * \return Gets the remote transmisstion request flag.
     */
    unsigned char getRTRMask() { return rtr_mask; }


    void setDataMask(unsigned char *accept_data);
    void getDataMask(unsigned char *accept_data);

    bool match(const decoded_can_frame &frame);

private:
    bool eid;                    //! accept eid
    bool sid;                    //! accept sid
    quint32 id_mask;       //! IDs to accept, must match, except id_mask >0x7FF or >0x7FFFFFFFFFFFFFFF
    unsigned char dlc_mask;      //! DLC to accept, frame.dlc must be equal to dlc_mask, except dlc_mask = 0xFF
    unsigned char rtr_mask;      //! RTR to accept, frame.rtr must be equal to rtr_mask, except rtr_mask = 0xFF
    unsigned char data_mask[8];  //! Data mask to accept, data{n} must be qeual to data_mask{n}, except data_mask{n} = 0xFF
};

#endif // CCANFILTER_H
