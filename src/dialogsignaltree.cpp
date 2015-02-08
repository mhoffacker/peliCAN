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

#include "dialogsignaltree.h"
#include "ui_dialogsignaltree.h"



DialogSignalTree::DialogSignalTree(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSignalTree)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);


    g = new CGraph(ui->widget_2);
    g->setSliding(true, 10000);
}

DialogSignalTree::~DialogSignalTree()
{
    delete ui;
}

void DialogSignalTree::SetTreeModel(QAbstractItemModel *model)
{
    ui->treeView->setModel(model);
}

void DialogSignalTree::ExpandTree()
{
    if ( ui )
        ui->treeView->expandAll();
}


void DialogSignalTree::on_checkBox_sliding_toggled(bool checked)
{
    g->setSliding(checked, ui->doubleSpinBox->value()*1000);
}

CGraph* DialogSignalTree::getGraph()
{
    return g;
}

void DialogSignalTree::on_doubleSpinBox_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    g->setSliding(ui->checkBox_sliding->checkState() == Qt::Checked, ui->doubleSpinBox->value()*1000);
}

double DialogSignalTree::getUpdateRate()
{
    return ui->doubleSpinBox_UpdateRate->value();
}

