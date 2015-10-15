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

#ifndef DIALOGSENDDATA_H
#define DIALOGSENDDATA_H

#include <QDialog>

#include "send_widget.h"
#include "send_widget_special.h"

#include "ccancomm.h"

namespace Ui {
class DialogSendData;
}

class DialogSendData : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSendData(QWidget *parent = 0);
    ~DialogSendData();

    void setCanComm(CCanComm *c);

    send_widget_special *AddSpecialWidget();
    void RemoveAllSpecialWidget();

private slots:

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::DialogSendData *ui;
    QList<send_widget*> sendwidgets;
    QList<send_widget_special*> sendwidgetsspecial;

    CCanComm *can;
};

#endif // DIALOGSENDDATA_H
