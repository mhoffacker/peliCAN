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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include "ccancomm.h"
#include "csocketcan.h"
#include "raw_tree_model.h"
#include "dialogsenddata.h"
#include "send_widget_special.h"
#include "dialogquadi.h"
#include "ccanmessage.h"
#include "dialogdataview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *bar);

private slots:
    void can_received(const decoded_can_frame &frame);

    void on_action_Online_triggered(bool checked);

    void on_action_Load_signal_definition_triggered();

    void on_action_About_triggered();

    void on_action_Logging_Active_triggered(bool checked);

    void on_action_Quit_triggered();

    void on_action_Data_Graph_triggered(bool checked);

    void on_actionShow_Hide_send_signal_triggered();

    void on_action_Help_triggered();

    void on_actionQuadI_configurator_triggered();

private:
    Ui::MainWindow *ui;

    CCanComm *can;

    QList<CCANMessage*> can_messages;

    raw_tree_model *rt_model;

    DialogSendData *dialog_send_data;
    DialogQuadI *dialog_quadi;
    DialogDataView *dialog_dataview;

    bool online(bool o, QString port);
    bool log(bool l, QString fileName = "");

    QFile* logfile;
    QTextStream* log_stream;
    bool logging_active;
};

#endif // MAINWINDOW_H
