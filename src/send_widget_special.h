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

#ifndef SEND_WIDGET_SPECIAL_H
#define SEND_WIDGET_SPECIAL_H

#include <QWidget>
#include <QTimer>

#include "ccancomm.h"

namespace Ui {
class send_widget_special;
}

class send_widget_special : public QWidget
{
    Q_OBJECT

public:
    explicit send_widget_special(QWidget *parent = 0);
    ~send_widget_special();


    enum
    {
        UNDEFINED = 0,
        UINT16,
        INT16,
        UINT32,
        INT32,
        UINT64,
        INT64,
        FLOAT,
        HEX
    };

    void setType(int type);
    int Type();

    void setID(quint32 id);
    quint32 ID();

    void setDataMask(quint64 mask);
    quint64 DataMask();

    void setData(quint64 data);
    quint64 Data();

    void setName(QString name);
    QString Name();

    void setDLC(quint8 dlc);
    quint8 DLC();

    void setEXT(bool ext);
    bool EXT();

    void setCanComm(CCanComm *c);

private slots:
    void on_pushButton_send_clicked();

    void send_data();

private:
    Ui::send_widget_special *ui;

    int m_type;
    quint32 m_id;
    quint64 m_mask;
    quint64 m_data;
    quint8 m_dlc;
    bool m_ext;

    QString m_name;
    QString m_data_str;

    QTimer *timer;

    CCanComm *can;
};

#endif // SEND_WIDGET_SPECIAL_H
