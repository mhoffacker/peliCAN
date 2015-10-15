#include "dialogdataview.h"
#include "ui_dialogdataview.h"

#include <QVector>

const QString ProgressString = "|/-\\";

QTreeWidgetItemIntData::QTreeWidgetItemIntData(QTreeWidgetItem *parent) :
    QTreeWidgetItem(parent, 0)
{
    m_InternalData = NULL;
}

// See http://stackoverflow.com/questions/9686648/is-it-possible-to-create-a-signal-for-when-a-qtreewidgetitem-checkbox-is-toggled
void QTreeWidgetItemIntData::setData(int column, int role, const QVariant& value) {
    const bool isCheckChange = column == 0
        && role == Qt::CheckStateRole
        && data(column, role).isValid() // Don't "change" during initialization
        && checkState(0) != value;
    QTreeWidgetItem::setData(column, role, value);
    if (isCheckChange) {
        emit itemCheckStateChanged(this, checkState(0) == Qt::Checked);
    }
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

            if ( internal->graph ) {
                QCPGraph *graph = internal->graph;
                graph->removeDataBefore(msg->GetMsTimeStamp()-10000.0);

                graph->addData(msg->GetMsTimeStamp(), sig->GetValue());
                //graph->rescaleValueAxis();

                ui->customPlot->xAxis->setRange(msg->GetMsTimeStamp()-10000.0, msg->GetMsTimeStamp(), Qt::AlignRight);


            }

        }
    }
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

}

void DialogDataView::AddCANMessage(CCANMessage *msg) {
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

    treeItem->setText(0, msg->GetName());

    foreach (CCANSignal2 *sig, *(msg->GetSignals()) ) {
        InternalData *internal = new InternalData;

        internal->item = new QTreeWidgetItemIntData(treeItem);

        connect(internal->item, SIGNAL(itemCheckStateChanged(QTreeWidgetItemIntData*, bool)),
                this, SLOT(itemCheckStateChanged(QTreeWidgetItemIntData*, bool)));

        internal->item->setText(0, sig->GetName());
        internal->item->setText(1, "");
        internal->item->setText(2, sig->GetUnit());
        internal->item->setText(3, "|");

        internal->graph = NULL;
        internal->item->setFlags(internal->item->flags() | Qt::ItemIsUserCheckable);
        internal->item->setCheckState(0, Qt::Unchecked);


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

void DialogDataView::itemCheckStateChanged(QTreeWidgetItemIntData *item, bool checked) {

    InternalData *internal = item->GetInternalData();

    if ( internal == NULL ) {
        return;
    }

    if ( checked ) {
        internal->graph = ui->customPlot->addGraph();
        if ( internal->graph == NULL ) {
            return;
        }

        QPen pen;
        pen.setColor(QColor(0, 0, 255, 200));

        internal->graph->setLineStyle(QCPGraph::lsLine);
        internal->graph->setPen(pen);

        internal->graph->setAntialiased(false);
    } else {
        if ( internal->graph ) {
            ui->customPlot->removeGraph(internal->graph);
            internal->graph = NULL;
        }
    }
}
