#include "send_widget.h"
#include "ui_send_widget.h"
#include <stdint.h>

#include <QDebug>

send_widget::send_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::send_widget)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(send_data()));
    timer->stop();

    can = NULL;
}

send_widget::~send_widget()
{
    delete timer;
    delete ui;
}

void send_widget::on_spinBox_DLC_valueChanged(int arg1)
{
    ui->lineEdit_data_0->setEnabled(arg1 >= 1 && !ui->checkBox_RTR->isChecked());
    ui->lineEdit_data_1->setEnabled(arg1 >= 2 && !ui->checkBox_RTR->isChecked());
    ui->lineEdit_data_2->setEnabled(arg1 >= 3 && !ui->checkBox_RTR->isChecked());
    ui->lineEdit_data_3->setEnabled(arg1 >= 4 && !ui->checkBox_RTR->isChecked());
    ui->lineEdit_data_4->setEnabled(arg1 >= 5 && !ui->checkBox_RTR->isChecked());
    ui->lineEdit_data_5->setEnabled(arg1 >= 6 && !ui->checkBox_RTR->isChecked());
    ui->lineEdit_data_6->setEnabled(arg1 >= 7 && !ui->checkBox_RTR->isChecked());
    ui->lineEdit_data_7->setEnabled(arg1 >= 8 && !ui->checkBox_RTR->isChecked());
}


void send_widget::on_pushButton_send_clicked(bool checked)
{
    Q_UNUSED(checked);
}

void send_widget::on_checkBox_RTR_toggled(bool checked)
{
    int arg1 = ui->spinBox_DLC->value();
    ui->lineEdit_data_0->setEnabled(arg1 >= 1 && !checked);
    ui->lineEdit_data_1->setEnabled(arg1 >= 2 && !checked);
    ui->lineEdit_data_2->setEnabled(arg1 >= 3 && !checked);
    ui->lineEdit_data_3->setEnabled(arg1 >= 4 && !checked);
    ui->lineEdit_data_4->setEnabled(arg1 >= 5 && !checked);
    ui->lineEdit_data_5->setEnabled(arg1 >= 6 && !checked);
    ui->lineEdit_data_6->setEnabled(arg1 >= 7 && !checked);
    ui->lineEdit_data_7->setEnabled(arg1 >= 8 && !checked);
}

void send_widget::send_data()
{
    bool ok;

    int64_t id = ui->lineEdit_ID->text().toULong(&ok, 16);

    if ( !ok )
        return;

    bool ext = ui->lineEdit_ID->text().length() > 3;
    bool rtr = ui->checkBox_RTR->isChecked();
    uint8_t  dlc = ui->spinBox_DLC->value();

    uint8_t data[8];

    data[0] = ui->lineEdit_data_0->text().toUInt(&ok, 16);
    if ( !ok && dlc > 0)
        return;

    data[1] = ui->lineEdit_data_1->text().toUInt(&ok, 16);
    if ( !ok && dlc > 1)
        return;

    data[2] = ui->lineEdit_data_2->text().toUInt(&ok, 16);
    if ( !ok && dlc > 2)
        return;

    data[3] = ui->lineEdit_data_3->text().toUInt(&ok, 16);
    if ( !ok && dlc > 3)
        return;

    data[4] = ui->lineEdit_data_4->text().toUInt(&ok, 16);
    if ( !ok && dlc > 4)
        return;

    data[5] = ui->lineEdit_data_5->text().toUInt(&ok, 16);
    if ( !ok && dlc > 5)
        return;

    data[6] = ui->lineEdit_data_6->text().toUInt(&ok, 16);
    if ( !ok && dlc > 6)
        return;

    data[7] = ui->lineEdit_data_7->text().toUInt(&ok, 16);
    if ( !ok && dlc > 7)
        return;

    if ( can != NULL )
        can->send(id, ext, rtr, dlc, data);
}

void send_widget::on_pushButton_send_clicked()
{
    send_data();
}

void send_widget::on_spinBox_cyclic_valueChanged(int arg1)
{
    timer->stop();
    if ( arg1 > 0 )
    {
        timer->setInterval(arg1);
        timer->start();
    }
}

void send_widget::setCanComm(CCanComm *c)
{
    can = c;
}
