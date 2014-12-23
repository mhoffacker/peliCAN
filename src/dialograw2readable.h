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

#ifndef DIALOGRAW2READABLE_H
#define DIALOGRAW2READABLE_H

#include <QDialog>

namespace Ui {
class DialogRAW2Readable;
}

class DialogRAW2Readable : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRAW2Readable(QWidget *parent = 0);
    ~DialogRAW2Readable();

    QString getSource() { return source; }
    QString getTarget() { return target; }

private slots:
    void on_btn_browse_input_clicked();

    void on_btn_browse_output_clicked();

    void on_buttonBox_accepted();

private:
    Ui::DialogRAW2Readable *ui;

    QString source;
    QString target;
};

#endif // DIALOGRAW2READABLE_H
