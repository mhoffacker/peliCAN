#ifndef DIALOGDATAVIEW_H
#define DIALOGDATAVIEW_H

#include <QDialog>
#include <QTreeWidget>
#include <ccanmessage.h>



namespace Ui {
class DialogDataView;
}

class QTreeWidgetItemIntData;

struct InternalData {
    QTreeWidgetItemIntData *item;
    int8_t Progress;
    CCANMessage *can_msg;
    CCANSignal2 *can_sig;

    // Todo: Add plot here
};

class QTreeWidgetItemIntData : public QTreeWidgetItem
{

public:
    explicit QTreeWidgetItemIntData(QTreeWidgetItem *parent);
    ~QTreeWidgetItemIntData();

    void SetInternalData(InternalData *data) { m_InternalData = data; }
    InternalData *GetInternalData() { return m_InternalData; }

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

private:
    Ui::DialogDataView *ui;

    QMap<void*, void*> SignalMap;
};

#endif // DIALOGDATAVIEW_H
