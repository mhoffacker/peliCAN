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

#ifndef SEND_WIDGET_H
#define SEND_WIDGET_H

#include <QWidget>
#include <QTimer>

#include "ccancomm.h"

namespace Ui {
class send_widget;
}

class send_widget : public QWidget
{
    Q_OBJECT

public:
    explicit send_widget(QWidget *parent = 0);
    ~send_widget();

    void setCanComm(CCanComm *c);

private slots:
    void on_spinBox_DLC_valueChanged(int arg1);

    void on_pushButton_send_clicked(bool checked);

    void on_checkBox_RTR_toggled(bool checked);


    void send_data();

    void on_pushButton_send_clicked();

    void on_spinBox_cyclic_valueChanged(int arg1);

private:
    Ui::send_widget *ui;

    QTimer *timer;

    CCanComm *can;
};

#endif // SEND_WIDGET_H
