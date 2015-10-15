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

#include "dialogsenddata.h"
#include "ui_dialogsenddata.h"



DialogSendData::DialogSendData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSendData)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    can = NULL;
}

DialogSendData::~DialogSendData()
{
    delete ui;
}


void DialogSendData::setCanComm(CCanComm *c)
{
    can = c;
    foreach (send_widget *w, sendwidgets)
    {
        w->setCanComm(can);
    }

    foreach (send_widget_special *w, sendwidgetsspecial)
    {
        w->setCanComm(can);
    }
}

send_widget_special *DialogSendData::AddSpecialWidget()
{
    send_widget_special *s = new send_widget_special(ui->scrollAreaWidgetContents);

    ui->verticalLayout_Widget->addWidget(s);
    sendwidgetsspecial.append(s);
    s->setCanComm(can);

    return s;
}

void DialogSendData::RemoveAllSpecialWidget()
{
    while ( !sendwidgetsspecial.empty() )
    {
        send_widget_special *w = sendwidgetsspecial.first();
        ui->verticalLayout_Widget->removeWidget(w);
        delete w;

        sendwidgetsspecial.removeFirst();
    }
}

void DialogSendData::on_spinBox_valueChanged(int arg1)
{
    while ( arg1 > sendwidgets.size() )
    {
        send_widget *w = new send_widget(ui->scrollAreaWidgetContents);
        w->setCanComm(can);

        ui->verticalLayout_Widget->addWidget(w);
        sendwidgets.append(w);
    }

    while ( arg1 < sendwidgets.size() )
    {
        send_widget *w = sendwidgets.last();
        ui->verticalLayout_Widget->removeWidget(w);
        delete w;

        sendwidgets.removeLast();
    }
}
