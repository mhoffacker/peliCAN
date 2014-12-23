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

#include "ccanfilter.h"

/*!
 * \brief CCANFilter::CCANFilter Constructor
 * Sets all values to accept everything.
 */
CCANFilter::CCANFilter()
{
    // Initialize filter to accept everything
    eid = true;
    sid = true;
    id_mask = 0xFFFFFFFF;
    dlc_mask = 0xFF;
    rtr_mask = 0;
    data_mask[0] = 0xFF;
    data_mask[1] = 0xFF;
    data_mask[2] = 0xFF;
    data_mask[3] = 0xFF;
    data_mask[4] = 0xFF;
    data_mask[5] = 0xFF;
    data_mask[6] = 0xFF;
    data_mask[7] = 0xFF;
}

/*!
 * \brief Sets the data mask
 * \param accept_data Pointer to the data mask. Array of 8 byte.
 */
void CCANFilter::setDataMask(unsigned char *accept_data)
{
    for ( int i=0; i<8; i++ )
        data_mask[i] = accept_data[i];
}

/*!
 * \brief Gets the data mask
 * \param accept_data Get the data mask. Array of 8 byte.
 */
void CCANFilter::getDataMask(unsigned char *accept_data)
{
    for ( int i=0; i<8; i++ )
        accept_data[i] = data_mask[i];
}

/*!
 * \brief Test if a frame matches the filter criteria.
 * \param frame Reference to a struct decoded_can_frame
 * \return true if matched, false if not
 */
bool CCANFilter::match(const decoded_can_frame &frame)
{
    // Check for extended ID
    if ( frame.EFF & !eid )
        return false;

    // Check fo standard ID
    if ( !frame.EFF & !sid )
        return false;

    // Check for ID
    if ( (frame.can_id & id_mask) != frame.can_id )
        return false;

    // Check for RTR
    if ( rtr_mask != 0xFF && frame.RTR != rtr_mask )
        return false;

    // Check for DLC
    if ( dlc_mask != 0xFF && frame.can_dlc != dlc_mask )
        return false;

    for (int i=0; i<8; i++)
    {
        if ( data_mask[i] != 0xFF && frame.data[i] != data_mask[i] )
            return false;
    }

    return true;
}
