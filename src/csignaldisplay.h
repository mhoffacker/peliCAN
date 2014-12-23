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

#ifndef CSIGNALDISPLAY_H
#define CSIGNALDISPLAY_H

#include <QString>
#include <QScriptEngine>
#include <QScriptProgram>
#include <QScriptValue>

#include <stdint.h>

#include "ccancomm.h"

class CSignalDisplay
{
public:
    CSignalDisplay();
    virtual ~CSignalDisplay();

    virtual QString getUnit() = 0;
    virtual void setUnit(QString u) = 0;

    virtual bool setParam(QString param) = 0;

    virtual QString getValue(const decoded_can_frame &frame) = 0;
};

class CSignalDisplayScript : public CSignalDisplay
{
public:
    CSignalDisplayScript();
    ~CSignalDisplayScript();

    QString getUnit() { return unit; }
    void setUnit(QString u) { unit = u;}

    bool setParam(QString param);

    QString getValue(const decoded_can_frame &frame);

private:
    QString unit;
    QScriptProgram prog;
    QScriptEngine engine;
    QScriptValue disp_fun;
    QScriptValue script_data;
};

class CSignalDisplayNum : public CSignalDisplay
{
public:
    CSignalDisplayNum();

    QString getUnit() { return unit; }
    void setUnit(QString u) { unit = u;}

    bool setParam(QString param);

    QString getValue(const decoded_can_frame &frame);

private:
    QString unit;
    virtual QString format_number(uint64_t num) = 0;
    unsigned char filter[8];
};

class CSignalDisplayHex : public CSignalDisplayNum
{
public:
    CSignalDisplayHex();

private:
    QString format_number(uint64_t num);
};

class CSignalDisplayInt16 : public CSignalDisplayNum
{
public:
    CSignalDisplayInt16();

private:
    QString format_number(uint64_t num);
};

class CSignalDisplayUInt16 : public CSignalDisplayNum
{
public:
    CSignalDisplayUInt16();

private:
    QString format_number(uint64_t num);
};

class CSignalDisplayInt32 : public CSignalDisplayNum
{
public:
    CSignalDisplayInt32();

private:
    QString format_number(uint64_t num);
};

class CSignalDisplayUInt32 : public CSignalDisplayNum
{
public:
    CSignalDisplayUInt32();

private:
    QString format_number(uint64_t num);
};

class CSignalDisplayInt64 : public CSignalDisplayNum
{
public:
    CSignalDisplayInt64();

private:
    QString format_number(uint64_t num);
};

class CSignalDisplayUInt64 : public CSignalDisplayNum
{
public:
    CSignalDisplayUInt64();

private:
    QString format_number(uint64_t num);
};

class CSignalDisplayFloat : public CSignalDisplayNum
{
public:
    CSignalDisplayFloat();

private:
    QString format_number(uint64_t num);
};

#endif // CSIGNALDISPLAY_H
