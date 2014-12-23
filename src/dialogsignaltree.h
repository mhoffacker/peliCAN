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

#ifndef DIALOGSIGNALTREE_H
#define DIALOGSIGNALTREE_H

#include <QDialog>
#include <QAbstractItemModel>
#include "multi_plot.h"

namespace Ui {
class DialogSignalTree;
}

class DialogSignalTree : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSignalTree(QWidget *parent = 0);
    ~DialogSignalTree();

    void SetTreeModel(QAbstractItemModel *model);
    void ExpandTree();
    double getUpdateRate();

    CGraph *getGraph();

private slots:


    void on_checkBox_sliding_toggled(bool checked);

    void on_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::DialogSignalTree *ui;
    CGraph *g;
};

#endif // DIALOGSIGNALTREE_H
