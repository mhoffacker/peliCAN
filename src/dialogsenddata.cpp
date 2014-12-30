#include "dialogsenddata.h"
#include "ui_dialogsenddata.h"



DialogSendData::DialogSendData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSendData)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    can = NULL;
}

DialogSendData::~DialogSendData()
{
    delete ui;
}


void DialogSendData::on_spinBox_editingFinished()
{
    int arg1 = ui->spinBox->value();

    while ( arg1 > sendwidgets.size() )
    {
        send_widget *w = new send_widget(ui->scrollAreaWidgetContents);
        w->setCanComm(can);

        ui->verticalLayout_Widget->addWidget(w);
        sendwidgets.append(w);
    }

    while ( arg1 < sendwidgets.size() )
    {
        send_widget *w = sendwidgets.last();
        ui->verticalLayout_Widget->removeWidget(w);
        delete w;

        sendwidgets.removeLast();
    }
}

void DialogSendData::setCanComm(CCanComm *c)
{
    can = c;
    foreach (send_widget *w, sendwidgets)
    {
        w->setCanComm(can);
    }

    foreach (send_widget_special *w, sendwidgetsspecial)
    {
        w->setCanComm(can);
    }
}

send_widget_special *DialogSendData::AddSpecialWidget()
{
    send_widget_special *s = new send_widget_special(ui->scrollAreaWidgetContents);

    ui->verticalLayout_Widget->addWidget(s);
    sendwidgetsspecial.append(s);
    s->setCanComm(can);

    return s;
}
