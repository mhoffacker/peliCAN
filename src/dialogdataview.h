#ifndef DIALOGDATAVIEW_H
#define DIALOGDATAVIEW_H

#include <QDialog>
#include <QTreeWidget>
#include <QMessageBox>

#include "qcustomplot.h"
#include "ccanmessage.h"



namespace Ui {
class DialogDataView;
}

class QTreeWidgetItemIntData;

struct InternalData {
    QTreeWidgetItemIntData *item;
    int8_t Progress;
    CCANMessage *can_msg;
    CCANSignal2 *can_sig;
    QCPGraph *graph;
};

class QTreeWidgetItemIntData : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    explicit QTreeWidgetItemIntData(QTreeWidgetItem *parent);
    ~QTreeWidgetItemIntData();

    void SetInternalData(InternalData *data) { m_InternalData = data; }
    InternalData *GetInternalData() { return m_InternalData; }

    void setData(int column, int role, const QVariant& value);

signals:
    void itemCheckStateChanged(QTreeWidgetItemIntData *item, bool checked);

private:
    InternalData *m_InternalData;
};

class DialogDataView : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDataView(QWidget *parent = 0);
    ~DialogDataView();

    void AddCANMessage(CCANMessage *msg);
    void UpdateMessage(CCANMessage *msg);
    void ResetView();

public slots:
    void itemCheckStateChanged(QTreeWidgetItemIntData *item, bool checked);

private:
    Ui::DialogDataView *ui;

    QMap<void*, void*> SignalMap;
};

#endif // DIALOGDATAVIEW_H
