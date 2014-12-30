#ifndef DIALOGSENDDATA_H
#define DIALOGSENDDATA_H

#include <QDialog>

#include "send_widget.h"
#include "send_widget_special.h"

#include "ccancomm.h"

namespace Ui {
class DialogSendData;
}

class DialogSendData : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSendData(QWidget *parent = 0);
    ~DialogSendData();

    void setCanComm(CCanComm *c);

    send_widget_special *AddSpecialWidget();

private slots:

    void on_spinBox_editingFinished();

private:
    Ui::DialogSendData *ui;
    QList<send_widget*> sendwidgets;
    QList<send_widget_special*> sendwidgetsspecial;

    CCanComm *can;
};

#endif // DIALOGSENDDATA_H
