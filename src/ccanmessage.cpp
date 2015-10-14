#include "ccanmessage.h"

#include <assert.h>

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#define ct_assert(e) enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }

CCANMessage::CCANMessage() {

}

CCANMessage::~CCANMessage() {
    foreach (CCANSignal2* item, m_list) {
        delete item;
    }
    m_list.clear();
}

bool CCANMessage::process_frame(const decoded_can_frame &frame, double update_rate_ms) {
    Q_UNUSED(update_rate_ms);

    if ( frame.can_id != m_id ) {
        return false;
    }

    uint64_t data = ((uint64_t)frame.data[0] << 56 ) |
                    ((uint64_t)frame.data[1] << 48 ) |
                    ((uint64_t)frame.data[2] << 40 ) |
                    ((uint64_t)frame.data[3] << 32 ) |
                    ((uint64_t)frame.data[4] << 24 ) |
                    ((uint64_t)frame.data[5] << 16 ) |
                    ((uint64_t)frame.data[6] << 8 ) |
                    ((uint64_t)frame.data[7] << 0 );

    foreach (CCANSignal2* item, m_list) {
        item->CalcValue(data);
    }

    m_timestamp = (frame.tv_ioctl.tv_sec + ((double)frame.tv_ioctl.tv_usec)/1000000.0) / 1000;

    return true;
}

void CCANSignal2::CalcValue(uint64_t data) {
    uint64_t temp, mask;
    int64_t stemp;


    //Mask out bits that we need
    mask = 0;                    // Fill with 1
    mask = ~mask;

    // << 64 does not work??
    if ( m_length != 64 ) {
        mask = mask << m_length;    // Shift left, by length. Zeros = bits we want
    } else {
        mask = 0;
    }

    mask = ~mask;               // Invert, ones = bits we want

    //shift data by startbit and mask
    temp = (data >> m_start_bit) & mask;

    // Swap byte order
#ifdef BIG_ENDIAN
    if ( m_endianess == ENDIAN_LITTLE ) {
#else
    if ( m_endianess == ENDIAN_BIG ) {
#endif
        //TODO add conversion big/little endian
#warning Add conversion between big and little endian
    }

    if ( m_type == UNSIGNED_INT ) {
        m_data = (double)temp;
    } else if ( m_type == SIGNED_INT ) {
        // Test for highest byte
        if ( temp & (1<<(m_length-1)) ) {
            // We cant simply invert it, since we dont know the bit-with
            // invert the mask again for filling up the
            // bits for the 2s complement
            mask = ~mask;
            temp = (temp | mask);
            memcpy(&stemp, &temp, sizeof(int64_t));
        } else {
            stemp = (int64_t)temp;
        }

        m_data = (double)stemp;
    } else if ( m_type == FLOAT32 || m_type == FLOAT64 ) {
        if ( m_type == FLOAT32 ) {
            ct_assert(sizeof(float) == 4);
            uint32_t t2 = (uint32_t)temp;
            float tf;
            memcpy(&tf, &t2, sizeof(float));
            m_data=tf;
        } else if ( m_type == FLOAT64 ) {
            ct_assert(sizeof(double) == 8);
            memcpy(&m_data, &temp, sizeof(double));
        }
    }

    if ( m_data > m_max ) {
        m_data = m_max;
    }

    if ( m_data < m_min ) {
        m_data = m_min;
    }

    m_data = m_data * m_factor + m_offset;

}
