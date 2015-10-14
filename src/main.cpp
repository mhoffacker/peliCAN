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

#include "mainwindow.h"
#include <QApplication>
#include <QMetaType>

#include "csocketcan.h"
#include "global_config.h"

#include <iostream>

#include "ccanmessage.h"

Q_DECLARE_METATYPE(decoded_can_frame);

int main(int argc, char *argv[])
{
    qRegisterMetaType<decoded_can_frame>("CANFrame");

    loadConfig();

    double k = -10;
    uint64_t b;
    memcpy(&b, &k, sizeof(double));

    CCANSignal2 test;
    test.SetEndianess(ENDIAN_BIG);
    test.SetStartbit(0);
    test.SetLength(64);
    test.SetType(FLOAT64);
    test.CalcValue(b);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int i = a.exec();

    saveConfig();

    return i;
}
