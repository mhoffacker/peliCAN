#ifndef SEND_WIDGET_SPECIAL_H
#define SEND_WIDGET_SPECIAL_H

#include <QWidget>
#include <QTimer>

#include "ccancomm.h"

namespace Ui {
class send_widget_special;
}

class send_widget_special : public QWidget
{
    Q_OBJECT

public:
    explicit send_widget_special(QWidget *parent = 0);
    ~send_widget_special();


    enum
    {
        UNDEFINED = 0,
        UINT16,
        INT16,
        UINT32,
        INT32,
        UINT64,
        INT64,
        FLOAT,
        HEX
    };

    void setType(int type);
    int Type();

    void setID(quint32 id);
    quint32 ID();

    void setDataMask(quint64 mask);
    quint64 DataMask();

    void setData(quint64 data);
    quint64 Data();

    void setName(QString name);
    QString Name();

    void setDLC(quint8 dlc);
    quint8 DLC();

    void setEXT(bool ext);
    bool EXT();

    void setCanComm(CCanComm *c);

private slots:
    void on_pushButton_send_clicked();

    void send_data();

private:
    Ui::send_widget_special *ui;

    int m_type;
    quint32 m_id;
    quint64 m_mask;
    quint64 m_data;
    quint8 m_dlc;
    bool m_ext;

    QString m_name;
    QString m_data_str;

    QTimer *timer;

    CCanComm *can;
};

#endif // SEND_WIDGET_SPECIAL_H
