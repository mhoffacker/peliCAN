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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include "ccancomm.h"
#include "csocketcan.h"
#include "ccanfilter.h"
#include "ccansignal.h"
#include "raw_tree_model.h"
#include "signal_tree_model.h"
#include "dialogsignaltree.h"

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

    void on_action_RAW_csv_to_readable_csv_triggered();

private:
    Ui::MainWindow *ui;

    CCanComm *can;
    CCANSignal can_signals_root;
    QList<CCANFilter*> can_raw_view_filter;

    raw_tree_model rt_model;
    signal_tree_model st_model;
    DialogSignalTree dialog_signal_tree;

    QHash<QString, CAxis*> AxisHash;
    bool XML_Read_Settings(QIODevice* device);
    void XML_Read_Raw_View(QXmlStreamReader &xmlStream);
    void XML_Read_Scales(QXmlStreamReader &xmlStream);
    void XML_Read_Signal(CCANSignal* sig_group, QXmlStreamReader &xmlStream);
    void XML_Attr_To_Filter(CCANFilter *filter, QXmlStreamAttributes &attributes);

    bool online(bool o, QString port);
    bool log(bool l, QString fileName = "");

    void clear_can_raw_view_filter();

    QFile* logfile;
    QTextStream* log_stream;
    bool logging_active;
};

#endif // MAINWINDOW_H
