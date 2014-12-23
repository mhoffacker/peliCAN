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

#include "signal_tree_model.h"
#include <QColor>

signal_tree_model::signal_tree_model(QObject *parent) :
    QAbstractItemModel(parent)
{
    root = NULL;
}


QModelIndex signal_tree_model::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CCANSignal *parentItem;

    if ( !parent.isValid() )
        parentItem = root;
    else
        parentItem = static_cast<CCANSignal*>(parent.internalPointer());

    CCANSignal *childItem = parentItem->getChild(row);
    if ( childItem )
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex signal_tree_model::parent(const QModelIndex &index) const
{
    if ( !index.isValid() )
        return QModelIndex();

    CCANSignal *childItem = static_cast<CCANSignal*>(index.internalPointer());
    CCANSignal *parentItem = childItem->getParent();

    if ( parentItem == root )
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

void signal_tree_model::DataHasChanged(CCANSignal *sig)
{
    int row = sig->row();

    QModelIndex left = createIndex(row , 1, sig->getParent());
    QModelIndex right = createIndex(row, 2, sig->getParent());

    emit dataChanged(left, right);
}

int signal_tree_model::rowCount(const QModelIndex &parent) const
{
    CCANSignal *parentItem;

    if ( parent.column() > 0 )
        return 0;

    if ( !parent.isValid() )
        parentItem = root;
    else
        parentItem = static_cast<CCANSignal*>(parent.internalPointer());

    return parentItem->ChildCount();
}

int signal_tree_model::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<CCANSignal*>(parent.internalPointer())->columnCount();
    else
        return root->columnCount();
}

QVariant signal_tree_model::data(const QModelIndex &index, int role) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( role == Qt::DisplayRole )
    {
        CCANSignal *item = static_cast<CCANSignal*>(index.internalPointer());
        return item->data(index.column());
    } else if ( role == Qt::TextColorRole )
    {
        CCANSignal *item = static_cast<CCANSignal*>(index.internalPointer());
        QPen *color = item->getColor();

        if ( color == NULL )
            return QColor(Qt::black);

        return color->color();
    }

    return QVariant();
}

Qt::ItemFlags signal_tree_model::flags(const QModelIndex &index) const
{
    if ( !index.isValid() )
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant signal_tree_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return root->data(section);

    return QVariant();
}
