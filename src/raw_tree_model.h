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

#ifndef RAW_TREE_MODEL_H
#define RAW_TREE_MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QList>

#include "ccancomm.h"

struct decoded_can_frame_string {
    QString can_id;
    QString can_dlc;
    QString data;
    QString ascii;
    bool EFF;
    bool RTR;
    bool ERR;
    QString time_as_str;
    QString name;
};

class raw_tree_model : public QAbstractTableModel
{
    Q_OBJECT
public:
    raw_tree_model(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void insert_data(QString name, const decoded_can_frame &frame);

private:
    QList<decoded_can_frame_string*> my_data;
};

#endif // RAW_TREE_MODEL_H
