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

#ifndef CCANMESSAGE
#define CCANMESSAGE

#include "ccancomm.h"

#include <QString>
#include <QModelIndex>

enum SignalType { SIGNED_INT, UNSIGNED_INT, FLOAT32, FLOAT64 };
enum SignalEndianess { ENDIAN_BIG, ENDIAN_LITTLE };

class CCANSignal2 {
public:
    CCANSignal2() { };
    ~CCANSignal2() { };

    double GetValue() { return m_data; }
    void CalcValue(uint64_t data);

    void SetEndianess(SignalEndianess endianess) { m_endianess = endianess; }
    SignalEndianess GetEndianess() { return m_endianess; }

    void SetStartbit(uint8_t start_bit) { m_start_bit = start_bit; }
    uint8_t GetStartbit() { return m_start_bit; }

    void SetLength(uint8_t length) { m_length = length; }
    uint8_t GetLength() { return m_length; }

    void SetMin(double min) { m_min = min; }
    double GetMin() { return m_min; }

    void SetMax(double max) { m_max = max; }
    double GetMax() { return m_max; }

    void SetUnit(QString unit) { m_unit = unit; }
    QString GetUnit() { return m_unit; }

    void SetOffset(double offset) { m_offset = offset; }
    double GetOffset() { return m_offset; }

    void SetFactor(double factor) { m_factor = factor; }
    double GetFactor() { return m_factor; }

    void SetType(SignalType type) { m_type = type; }
    SignalType GetType() { return m_type; }

    void SetName(QString name) { m_name = name; }
    QString GetName() { return m_name; }

private:
    SignalEndianess m_endianess;
    uint8_t m_start_bit;
    uint8_t m_length;
    double m_min;
    double m_max;
    double m_data;
    QString m_unit;
    SignalType m_type;
    double m_offset;
    double m_factor;
    QString m_name;
};

typedef QList<CCANSignal2*> CANSignal2List;

class CCANMessage {
public:
    CCANMessage();
    ~CCANMessage();

    void SetID(uint16_t id) { m_id = id; }
    uint16_t GetID() { return m_id; }

    void SetName(QString name) { m_name = name; }
    QString GetName() { return m_name; }

    void SetSize(uint8_t size) {m_size = size; }
    uint8_t GetSize() { return m_size; }

    double GetMsTimeStamp() { return m_timestamp; }

    void AddSignal(CCANSignal2 *sig) { m_list.append(sig); }

    bool process_frame(const decoded_can_frame &frame);

    CANSignal2List *GetSignals() { return &m_list; }

private:
    uint16_t m_id;
    uint8_t m_size;
    QString m_name;
    double m_timestamp;
    CANSignal2List m_list;
};


#endif // CCANMESSAGE

