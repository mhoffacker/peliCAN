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

#include "send_widget_special.h"
#include "ui_send_widget_special.h"

send_widget_special::send_widget_special(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::send_widget_special)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(send_data()));
    timer->stop();

    can = NULL;

    m_data_str = tr("Undefined");
    m_type = send_widget_special::UNDEFINED;
}

send_widget_special::~send_widget_special()
{
    delete timer;
    delete ui;
}

void send_widget_special::setCanComm(CCanComm *c)
{
    can = c;
}

void send_widget_special::setType(int type)
{
    m_type = type;

    switch ( m_type )
    {
    case send_widget_special::INT16:
        m_data_str = tr("INT16");
        break;
    case send_widget_special::UINT16:
        m_data_str = tr("UINT16");
        break;
    case send_widget_special::INT32:
        m_data_str = tr("INT32");
        break;
    case send_widget_special::UINT32:
        m_data_str = tr("UINT32");
        break;
    case send_widget_special::INT64:
        m_data_str = tr("INT64");
        break;
    case send_widget_special::UINT64:
        m_data_str = tr("UINT64");
        break;
    case send_widget_special::HEX:
        m_data_str = tr("HEX");
        break;
    case send_widget_special::FLOAT:
        m_data_str = tr("FLOAT");
        break;
    default:
    case send_widget_special::UNDEFINED:
        m_data_str = tr("UNDEFINED");
        break;
    }

    ui->label->setText(QString("%1 as %2 (ID: 0x%3)").arg(m_name).arg(m_data_str).arg(m_id,3, 16));
}

int send_widget_special::Type()
{
    return m_type;
}

void send_widget_special::setID(quint32 id)
{
    m_id = id;
}

quint32 send_widget_special::ID()
{
    return m_id;
}

void send_widget_special::setDataMask(quint64 mask)
{
    m_mask = mask;
}

quint64 send_widget_special::DataMask()
{
    return m_mask;
}

void send_widget_special::setData(quint64 data)
{
    m_data = data;
}

quint64 send_widget_special::Data()
{
    return m_data;
}

void send_widget_special::setName(QString name)
{
    m_name = name;

    ui->label->setText(QString("%1 as %2 (ID: 0x%3)").arg(m_name).arg(m_data_str).arg(m_id,3, 16));
}

QString send_widget_special::Name()
{
    return m_name;
}

void send_widget_special::setDLC(quint8 dlc)
{
    m_dlc = dlc;
}

quint8 send_widget_special::DLC()
{
    return m_dlc;
}

void send_widget_special::setEXT(bool ext)
{
    m_ext = ext;
}

bool send_widget_special::EXT()
{
    return m_ext;
}


void send_widget_special::on_pushButton_send_clicked()
{
    send_data();
}

void send_widget_special::send_data()
{
    float f_value;
    //qint16 num_value_s16;
    //quint16 num_value_u16;
    //qint32 num_value_s32;
    //quint32 num_value_u32;
    qint64 num_value_s64;
    quint64 num_value_u64;

    bool ok;


    if ( m_type == send_widget_special::HEX )
    {
        num_value_u64 = ui->lineEdit->text().toLongLong(&ok, 16);

        if ( ! ok )
        {
            ui->checkBox->setChecked(false);
            return;
        }
    } else  if ( m_type == send_widget_special::FLOAT )
    {
        f_value = ui->lineEdit->text().toFloat(&ok);
        if ( !ok )
        {
            ui->checkBox->setChecked(false);
            return;
        }
        memcpy(&num_value_u64, &f_value, sizeof(float));
    } else {
        num_value_s64 = ui->lineEdit->text().toLongLong(&ok, 10);
        if ( num_value_s64 < 0 )
        {
            if ( ! ok )
            {
                ui->checkBox->setChecked(false);
                return;
            } else {
                memcpy(&num_value_u64, &num_value_s64, sizeof(int64_t));
            }
        } else {
            num_value_u64 = ui->lineEdit->text().toLongLong(&ok, 10);

            if ( ! ok )
            {
                ui->checkBox->setChecked(false);
                return;
            }
        }
        if ( m_type == send_widget_special::UINT16 )
        {

            if ( num_value_u64 > Q_UINT64_C(0xFFFF) || num_value_s64 < 0 )
            {
                ui->checkBox->setChecked(false);
                return;
            }
        } else if ( m_type == send_widget_special::INT16 )
        {
            if ( (num_value_u64 & Q_UINT64_C(0xFFFFFFFFFFFF0000)) !=  Q_UINT64_C(0xFFFFFFFFFFFF0000) && // Negative value
                 (num_value_u64 & Q_UINT64_C(0xFFFFFFFFFFFF0000)) !=  Q_UINT64_C(0) )                   // Positive value
            {
                ui->checkBox->setChecked(false);
                return;
            }
        } else if ( m_type == send_widget_special::UINT32 )
        {
            if ( num_value_u64 > Q_UINT64_C(0xFFFFFFFF) || num_value_s64 < 0 )
            {
                ui->checkBox->setChecked(false);
                return;
            }
        } else if ( m_type == send_widget_special::INT32 )
        {
            if ( (num_value_u64 & Q_UINT64_C(0xFFFFFFFF00000000)) !=  Q_UINT64_C(0xFFFFFFFF00000000) && // Negative value
                 (num_value_u64 & Q_UINT64_C(0xFFFFFFFF00000000)) !=  Q_UINT64_C(0) )                   // Positive value
            {
                ui->checkBox->setChecked(false);
                return;
            }
        } else {
            ui->checkBox->setChecked(false);
            return;
        }
    }


    uint8_t data[8];

    uint64_t mask = m_mask;
    uint64_t data_stat = m_data;

    for ( int i=7; i>=0; i-- )
    {
        data[i] = 0;
        if ( (mask & 0xFF) == 0xFF )
        {
            data[i] = num_value_u64 & 0xFF;
            num_value_u64 = num_value_u64 >> 8;
        }
        data[i] = data[i] | ( data_stat & 0xFF );
        mask = mask >> 8;
        data_stat = data_stat >> 8;
    }

    if ( can != NULL )
    {
        if ( can->send_can(m_id, m_ext, false, m_dlc, data) )
            ui->checkBox->setChecked(true);
        else
            ui->checkBox->setChecked(false);
    } else
        ui->checkBox->setChecked(false);

}
