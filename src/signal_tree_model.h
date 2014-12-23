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

#ifndef SIGNAL_TREE_MODEL_H
#define SIGNAL_TREE_MODEL_H

#include <QAbstractItemModel>

#include "ccansignal.h"

class signal_tree_model : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit signal_tree_model(QObject *parent = 0);


    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;



    void SetSignalRoot(CCANSignal *sig_groups) { root = sig_groups; }
    CCANSignal* GetSignalRoot() { return root; }

    void DataHasChanged(CCANSignal *sig);


private:
    CCANSignal *root;

signals:

public slots:

};

#endif // SIGNAL_TREE_MODEL_H
