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

#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QAbstractButton>

#include "ccancomm.h"
#include "csocketcan.h"
#include "cslcan.h"

namespace Ui {
class ConnectDialog;
}

/*!
 * \brief The ConnectDialog class
 * Shows the connect dialog and returns a pointer to the communication
 * interface to be opened.
 */
class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();

    //! Pointer to the communication interface
    CCanComm *can;
    //! String for opening the comm interface.
    QString port;


private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ConnectDialog *ui;
    void showEvent ( QShowEvent * event ) ;
};

#endif // CONNECTDIALOG_H
