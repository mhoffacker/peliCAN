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

#include "dialograw2readable.h"
#include "ui_dialograw2readable.h"

#include <QFileDialog>

DialogRAW2Readable::DialogRAW2Readable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRAW2Readable)
{
    ui->setupUi(this);
}

DialogRAW2Readable::~DialogRAW2Readable()
{
    delete ui;
}

void DialogRAW2Readable::on_btn_browse_input_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select logfile"), "", tr("CSV document (*.csv)"));
    ui->edit_input->setText(fileName);
}

void DialogRAW2Readable::on_btn_browse_output_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select logfile"), "", tr("CSV document (*.csv)"));
    ui->edit_output->setText(fileName);
}

void DialogRAW2Readable::on_buttonBox_accepted()
{
    source = ui->edit_input->text();
    target = ui->edit_output->text();
}
