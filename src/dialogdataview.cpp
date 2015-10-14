#include "dialogdataview.h"
#include "ui_dialogdataview.h"



const QString ProgressString = "|/-\\";

QTreeWidgetItemIntData::QTreeWidgetItemIntData(QTreeWidgetItem *parent) :
    QTreeWidgetItem(parent, 0)
{
    m_InternalData = NULL;
}

QTreeWidgetItemIntData::~QTreeWidgetItemIntData()
{

}

DialogDataView::DialogDataView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDataView)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);


    // Set the number of columns in the tree
    ui->treeWidget->setColumnCount(4);
    ui->treeWidget->headerItem()->setText(0, "Name");
    ui->treeWidget->headerItem()->setText(1, "Value");
    ui->treeWidget->headerItem()->setText(2, "Unit");
    ui->treeWidget->headerItem()->setText(3, "");
    ui->treeWidget->setHeaderHidden(false);
}

DialogDataView::~DialogDataView()
{
    delete ui;
}

void DialogDataView::UpdateMessage(CCANMessage *msg) {
    foreach (CCANSignal2 *sig, *(msg->GetSignals()) ) {
        InternalData *internal = (InternalData*)SignalMap[(void*)sig];

        if ( internal != NULL ) {
            internal->item->setText(1, QString("%1").arg(sig->GetValue()));
            internal->Progress++;
            if ( internal->Progress > 3 ) {
                internal->Progress = 0;
            }

            internal->item->setText(3, ProgressString[internal->Progress]);
        }
    }
}

void DialogDataView::AddCANMessage(CCANMessage *msg) {
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

    treeItem->setText(0, msg->GetName());

    foreach (CCANSignal2 *sig, *(msg->GetSignals()) ) {
        InternalData *internal = new InternalData;

        internal->item = new QTreeWidgetItemIntData(treeItem);

        internal->item->setText(0, sig->GetName());
        internal->item->setText(1, "");
        internal->item->setText(2, sig->GetUnit());
        internal->item->setText(3, "|");

        internal->Progress = 0;
        internal->can_msg = msg;
        internal->can_sig = sig;

        internal->item->SetInternalData(internal);


        SignalMap.insert((void*)sig, (void*)internal);
    }

    ui->treeWidget->expandAll();
}

void DialogDataView::ResetView() {
    ui->treeWidget->clear();
    foreach (void *internal, SignalMap) {
        delete (InternalData*)internal;
    }

    SignalMap.clear();

}
