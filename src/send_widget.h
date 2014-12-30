#ifndef SEND_WIDGET_H
#define SEND_WIDGET_H

#include <QWidget>
#include <QTimer>

#include "ccancomm.h"

namespace Ui {
class send_widget;
}

class send_widget : public QWidget
{
    Q_OBJECT

public:
    explicit send_widget(QWidget *parent = 0);
    ~send_widget();

    void setCanComm(CCanComm *c);

private slots:
    void on_spinBox_DLC_valueChanged(int arg1);

    void on_pushButton_send_clicked(bool checked);

    void on_checkBox_RTR_toggled(bool checked);


    void send_data();

    void on_pushButton_send_clicked();

    void on_spinBox_cyclic_valueChanged(int arg1);

private:
    Ui::send_widget *ui;

    QTimer *timer;

    CCanComm *can;
};

#endif // SEND_WIDGET_H
