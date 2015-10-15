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


#include "raw_tree_model.h"
#include <QColor>

raw_tree_model::raw_tree_model(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int raw_tree_model::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 7;
}

int raw_tree_model::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return my_data.count();
}

QVariant raw_tree_model::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    decoded_can_frame_string *d;

    switch ( role )
    {
    case Qt::DisplayRole:
        if ( row >= my_data.size() )
            return QVariant();

        d = my_data.at(row);

        switch ( col )
        {
        case 0:
            return d->time_as_str;
        case 1:
            return QString("  ");   // Empty, only color
        case 2:
            return d->can_id;
        case 3:
            return d->can_dlc;
        case 4:
            return d->data;
        case 5:
            return d->ascii;
        case 6:
            return d->name;
        }

        break;

    case Qt::BackgroundColorRole:
        if ( row >= my_data.size() )
            return QVariant();

        d = my_data[row];

        if ( col == 1 )
        {
            if ( d->ERR )
                return QColor(Qt::red);
            else if ( d->RTR )
                return QColor(Qt::yellow);
            else
                return QColor(Qt::green);
        }

        if ( row % 2 == 1 )
            return QColor(Qt::gray);
        else
            return QColor(Qt::white);

    }

    return QVariant();
}

void raw_tree_model::insert_data(QString name, const decoded_can_frame &frame)
{
    beginInsertRows( QModelIndex(), 0, 0 );

    decoded_can_frame_string *d = new decoded_can_frame_string;


    d->time_as_str = frame.time_as_str;
    d->name = name;
    d->RTR = frame.RTR;
    d->ERR = frame.ERR;

    if ( frame.EFF )
    {
        d->can_id = QString("(ext) 0x%1").arg(frame.can_id, 8, 16, QLatin1Char( '0' )).toUpper();
    } else {
        d->can_id = QString("0x%1").arg(frame.can_id, 3, 16, QLatin1Char( '0' )).toUpper();
    }

    d->can_dlc = QString("%1").arg(frame.can_dlc, 1, 10, QLatin1Char( '0' ));

    d->data = QString("%1 %2 %3 %4 %5 %6 %7 %8")
                .arg(frame.data[0], 2, 16, QLatin1Char( '0' )).toUpper()
                .arg(frame.data[1], 2, 16, QLatin1Char( '0' )).toUpper()
                .arg(frame.data[2], 2, 16, QLatin1Char( '0' )).toUpper()
                .arg(frame.data[3], 2, 16, QLatin1Char( '0' )).toUpper()
                .arg(frame.data[4], 2, 16, QLatin1Char( '0' )).toUpper()
                .arg(frame.data[5], 2, 16, QLatin1Char( '0' )).toUpper()
                .arg(frame.data[6], 2, 16, QLatin1Char( '0' )).toUpper()
                .arg(frame.data[7], 2, 16, QLatin1Char( '0' )).toUpper();

    d->ascii.fromLocal8Bit((char*)frame.data, frame.can_dlc);


    my_data.append(d);

    endInsertRows();

    if ( my_data.size() == 10002 )
    {
        beginRemoveRows(QModelIndex(), 0, 1);
        delete my_data.first();
        my_data.removeFirst();

        delete my_data.first();
        my_data.removeFirst();
        endRemoveRows();
    }


}

QVariant raw_tree_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
         {
             if (orientation == Qt::Horizontal) {
                 switch (section)
                 {
                 case 0:
                     return QString("Time");
                 case 1:
                     return QString("RTR/ERR");
                 case 2:
                     return QString("Identifier");
                 case 3:
                     return QString("DLC");
                 case 4:
                     return QString("Data");
                 case 5:
                     return QString("ASCII");
                 case 6:
                     return QString("Name");
                 }
             }
         }
         return QVariant();
}
