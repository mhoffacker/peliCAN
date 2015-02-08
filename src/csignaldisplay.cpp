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

#include "csignaldisplay.h"

#include <stdint.h>

CSignalDisplay::CSignalDisplay()
{

}

CSignalDisplay::~CSignalDisplay()
{

}

CSignalDisplayScript::CSignalDisplayScript()
    : CSignalDisplay()
{

}

CSignalDisplayScript::~CSignalDisplayScript()
{

}

bool CSignalDisplayScript::setParam(QString param)
{
    QString test_res;

    prog = QScriptProgram(param);

    engine.evaluate(prog);

    disp_fun = engine.globalObject().property("disp");

    script_data = engine.newArray(8);

    return true;
}

QString CSignalDisplayScript::getValue(const decoded_can_frame &frame)
{
    QScriptValueList args;

    for ( int i=0; i<= 7; i++)
        script_data.setProperty(i, QScriptValue(frame.data[i]));

    // ToDo: Add conversion for long!!
    args << QScriptValue(0) << QScriptValue(frame.EFF) << QScriptValue(frame.ERR)
         << QScriptValue(frame.RTR) << QScriptValue(frame.can_dlc) << script_data;

    return disp_fun.call(QScriptValue(), args).toString();
}


CSignalDisplayNum::CSignalDisplayNum()
    : CSignalDisplay()
{

}

QString CSignalDisplayNum::getValue(const decoded_can_frame &frame)
{
    uint64_t tmp = 0;

    for ( int i = 0; i <= 7; i++ )
    {
        if ( filter[i] != 0x00 )
        {
            tmp = tmp << 8;
            tmp = tmp + ( frame.data[i] & filter[i] );
        }
    }

    return format_number(tmp);
}

bool CSignalDisplayNum::setParam(QString param)
{
    bool ok;

    memset(filter, 0xFF, sizeof(filter));

    qint64 tmp = param.toULongLong(&ok, 16);

    if ( ok )
    {
        for ( int i=7; i >= 0; i-- )
        {
            filter[i] = (unsigned char) ( tmp & 0xFF );
            tmp = tmp >> 8;
        }
    }

    return ok;
}

CSignalDisplayHex::CSignalDisplayHex()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayHex::format_number(uint64_t num)
{
    return QString("0x%1").arg(num, 0, 16);
}

CSignalDisplayInt8::CSignalDisplayInt8()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayInt8::format_number(uint64_t num)
{
    if ( num > 0xFF )
        return QString("Out of range");

    int8_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(int8_t));

    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayUInt8::CSignalDisplayUInt8()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayUInt8::format_number(uint64_t num)
{
    if ( num > 0xFF )
        return QString("Out of range");

    uint8_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(uint8_t));

    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayInt16::CSignalDisplayInt16()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayInt16::format_number(uint64_t num)
{
    if ( num > 0xFFFF )
        return QString("Out of range");

    int16_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(int16_t));

    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayUInt16::CSignalDisplayUInt16()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayUInt16::format_number(uint64_t num)
{
    if ( num > 0xFFFF )
        return QString("Out of range");

    uint16_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(uint16_t));

    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayInt32::CSignalDisplayInt32()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayInt32::format_number(uint64_t num)
{
    if ( num > 0xFFFFFFFF )
        return QString("Out of range");

    int32_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(int32_t));

    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayUInt32::CSignalDisplayUInt32()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayUInt32::format_number(uint64_t num)
{
    if ( num > 0xFFFFFFFF )
        return QString("Out of range");

    uint32_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(uint32_t));

    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayInt64::CSignalDisplayInt64()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayInt64::format_number(uint64_t num)
{
    int64_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(int64_t));
    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayUInt64::CSignalDisplayUInt64()
    : CSignalDisplayNum()
{

}

QString CSignalDisplayUInt64::format_number(uint64_t num)
{
    uint64_t tmp;
    memcpy((void*)&tmp, (void*)&num, sizeof(uint64_t));
    return QString("%1").arg(tmp, 0, 10);
}

CSignalDisplayFloat::CSignalDisplayFloat()
    : CSignalDisplayNum()
{

}


#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#define ct_assert(e) enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }

QString CSignalDisplayFloat::format_number(uint64_t num)
{
    float tmp;

    // Check if sizes of float is 4 bytes. Otherwise stop compilation
    ct_assert(sizeof(float)==4);

    memcpy((void*)&tmp, (void*)&num, sizeof(float));


    return QString("%1").arg(tmp);
}
