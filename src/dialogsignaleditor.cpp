#include "dialogsignaleditor.h"
#include "ui_dialogsignaleditor.h"

#include <QList>
#include <QMetaProperty>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

CTextTreeData::CTextTreeData()
    : CEditTreeData()
{

}

CTextTreeData::~CTextTreeData()
{

}

CScaleTreeData::CScaleTreeData()
    : CEditTreeData()
{

}

CScaleTreeData::~CScaleTreeData()
{

}

CSignalTreeData::CSignalTreeData()
    : CEditTreeData()
{

}

CSignalTreeData::~CSignalTreeData()
{

}

CSignalGroupTreeData::CSignalGroupTreeData()
    : CEditTreeData()
{

}

CSignalGroupTreeData::~CSignalGroupTreeData()
{

}

CSenderTreeData::CSenderTreeData()
    : CEditTreeData()
{

}

CSenderTreeData::~CSenderTreeData()
{

}

CRawTreeData::CRawTreeData()
    : CEditTreeData()
{

}

CRawTreeData::~CRawTreeData()
{

}
/************************************************************************
 * Model for data displaying
 ************************************************************************/

CEditTreeModel::CEditTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_root = NULL;
}


QModelIndex CEditTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CEditTreeData *parentItem;

    if ( !parent.isValid() )
        parentItem = m_root;
    else
        parentItem = static_cast<CEditTreeData*>(parent.internalPointer());

    CEditTreeData *childItem = parentItem->child(row);
    if ( childItem )
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex CEditTreeModel::parent(const QModelIndex &index) const
{
    if ( !index.isValid() )
        return QModelIndex();

    CEditTreeData *childItem = static_cast<CEditTreeData*>(index.internalPointer());
    CEditTreeData *parentItem = childItem->parent();

    if ( parentItem == m_root )
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

void CEditTreeModel::DataHasChanged(CEditTreeData *sig)
{
    int row = sig->row();

    QModelIndex left = createIndex(row , 1, sig->parent());
    QModelIndex right = createIndex(row, 2, sig->parent());

    emit dataChanged(left, right);
}

int CEditTreeModel::rowCount(const QModelIndex &parent) const
{
    CEditTreeData *parentItem;

    if ( parent.column() > 0 )
        return 0;

    if ( !parent.isValid() )
        parentItem = m_root;
    else
        parentItem = static_cast<CEditTreeData*>(parent.internalPointer());

    return parentItem->childCount();
}

int CEditTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<CEditTreeData*>(parent.internalPointer())->columnCount();
    else
        return m_root->columnCount();
}



QVariant CEditTreeModel::data(const QModelIndex &index, int role) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( role == Qt::DisplayRole )
    {
        CEditTreeData *item = static_cast<CEditTreeData*>(index.internalPointer());
        return item->data(index.column());
    } else if ( role == Qt::TextColorRole )
    {
    }

    return QVariant();
}

bool CEditTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    if ( !index.isValid() )
        return false;

    CEditTreeData *item = static_cast<CEditTreeData*>(index.internalPointer());

    if ( index.column() == 0 )
    {
        // If a item with the property insertItem is changed, add a new one
        if ( item->classname() == "CRawTreeData" )
        {
            CRawTreeData *raw = static_cast<CRawTreeData*>(index.internalPointer());
            raw->setData(0, value);

            if ( raw->insertItem() )
            {
                raw->setInsertItem(false);
                beginInsertRows(parent(index), raw->row()+1, raw->row()+2);
                    CRawTreeData *new_raw = new CRawTreeData();
                    new_raw->setData(0, QVariant(QString(tr("<add new>"))));
                    new_raw->setInsertItem(true);
                    raw->parent()->addChild(new_raw);
                endInsertRows();
            }
        } else if ( item->classname() == "CScaleTreeData" )
        {
            CScaleTreeData *scale = static_cast<CScaleTreeData*>(index.internalPointer());
            scale->setData(0, value);

            if ( scale->insertItem() )
            {
                scale->setInsertItem(false);
                beginInsertRows(parent(index), scale->row()+1, scale->row()+2);
                    CScaleTreeData *new_scale = new CScaleTreeData();
                    new_scale->setData(0, QVariant(QString(tr("<add new>"))));
                    new_scale->setInsertItem(true);
                    scale->parent()->addChild(new_scale);
                endInsertRows();
            }
        } else if ( item->classname() == "CSignalGroupTreeData" )
        {
            CSignalGroupTreeData *signal_group = static_cast<CSignalGroupTreeData*>(index.internalPointer());
            signal_group->setData(0, value);

            if ( signal_group->insertItem() )
            {
                signal_group->setInsertItem(false);
                beginInsertRows(parent(index), signal_group->row()+1, signal_group->row()+2);
                    CSignalGroupTreeData *new_signal_group = new CSignalGroupTreeData();
                    new_signal_group->setData(0, QVariant(QString(tr("<add new group>"))));
                    new_signal_group->setInsertItem(true);
                    signal_group->parent()->addChild(new_signal_group);
                endInsertRows();

                beginInsertRows(createIndex(new_signal_group->row(), 0, new_signal_group), 0, 1);
                    CSignalTreeData *new_signal = new CSignalTreeData();
                    new_signal->setData(0, QVariant(QString(tr("<add new signal>"))));
                    new_signal->setInsertItem(true);
                    new_signal_group->addChild(new_signal);
                endInsertRows();
            }
        } else if ( item->classname() == "CSignalTreeData" )
        {
            CSignalTreeData *signal = static_cast<CSignalTreeData*>(index.internalPointer());
            signal->setData(0, value);

            if ( signal->insertItem() )
            {
                signal->setInsertItem(false);
                beginInsertRows(parent(index), signal->row()+1, signal->row()+2);
                    CSignalTreeData *new_signal = new CSignalTreeData();
                    new_signal->setData(0, QVariant(QString(tr("<add new signal>"))));
                    new_signal->setInsertItem(true);
                    signal->parent()->addChild(new_signal);
                endInsertRows();
            }
        } else if ( item->classname() == "CSenderTreeData" )
        {
            CSenderTreeData *sender = static_cast<CSenderTreeData*>(index.internalPointer());
            sender->setData(0, value);

            if ( sender->insertItem() )
            {
                sender->setInsertItem(false);
                beginInsertRows(parent(index), sender->row()+1, sender->row()+2);
                    CSenderTreeData *new_sender = new CSenderTreeData();
                    new_sender->setData(0, QVariant(QString(tr("<add new>"))));
                    new_sender->setInsertItem(true);
                    sender->parent()->addChild(new_sender);
                endInsertRows();
            }
        }

        emit dataChanged(index, index);

        return true;
    } else
    {
        item->setData(index.column(), value);
        return true;
    }


    return false;
}

Qt::ItemFlags CEditTreeModel::flags(const QModelIndex &index) const
{
    if ( !index.isValid() )
        return 0;

    CEditTreeData *item = static_cast<CEditTreeData*>(index.internalPointer());
    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if ( item->isEditable() )
        flag |= Qt::ItemIsEditable;


    return flag;
}

QVariant CEditTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return m_root->data(section);

    return QVariant();
}

/************************************************************************
 * End Model for data displaying
 ************************************************************************/



/************************************************************************
 * Delegate begin
 ************************************************************************/
Delegate::Delegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void Delegate::setEditorData(QWidget *editor,
                       const QModelIndex &index) const
{
    QString className = editor->metaObject()->className();

    if ( className == "QLineEdit" )
    {
        if (editor->property("text").isValid())
        {
            editor->setProperty("text", index.data().toString());
            return;
        }
    } else if ( className == "QComboBox" )
    {
        if (editor->property("currentIndex").isValid())
        {
            // Force currentIndexChanged signal to be emitted
            editor->setProperty("currentIndex", -1);
            editor->setProperty("currentIndex", index.data().toInt());
            return;
        }
    }
    QItemDelegate::setEditorData(editor, index);
}

void Delegate::setModelData(QWidget *editor,
                     QAbstractItemModel *model,
                     const QModelIndex &index) const
{
    QString className = editor->metaObject()->className();

    if ( className == "QComboBox" )
    {
        QVariant i = editor->property("currentIndex");
        model->setData(index, i);

        return;
    }

    QItemDelegate::setModelData(editor, model, index);
}

/************************************************************************
 * Delegate end
 ************************************************************************/

DialogSignalEditor::DialogSignalEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSignalEditor)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    model = new CEditTreeModel(this);

    root = new CTextTreeData();
    root->setData(0, QVariant(QString(tr("Element"))));

    model->setRoot(root);

    ui->treeView->setModel(model);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new Delegate(this));

    // Hide all colums except first, other data is shown by QDataWidgetMapper
    for ( int i=1; i<100; i++ )
        ui->treeView->setColumnHidden(i, true);

}

DialogSignalEditor::~DialogSignalEditor()
{
    delete ui;

    delete root;
}



void DialogSignalEditor::empty_tree()
{
    ui->treeView->setModel(NULL);

    delete mapper;

    root->clear();

    CTextTreeData *e;

    e = new CTextTreeData();
    e->setData(0, QVariant(QString(tr("Raw view"))));
    root->addChild(e);

    CRawTreeData *raw = new CRawTreeData();
    raw->setData(0, QVariant(QString(tr("<add new>"))));
    raw->setInsertItem(true);
    e->addChild(raw);

    e = new CTextTreeData();
    e->setData(0, QVariant(QString(tr("Scales"))));
    root->addChild(e);

    CScaleTreeData *scale = new CScaleTreeData();
    scale->setData(0, QVariant(QString(tr("<add new>"))));
    scale->setInsertItem(true);
    e->addChild(scale);

    e = new CTextTreeData();
    e->setData(0, QVariant(QString(tr("Signals"))));
    root->addChild(e);

    CSignalGroupTreeData *signal_group = new CSignalGroupTreeData();
    signal_group->setData(0, QVariant(QString(tr("<add new group>"))));
    signal_group->setInsertItem(true);
    e->addChild(signal_group);

    CSignalTreeData *signal = new CSignalTreeData();
    signal->setData(0, QVariant(QString(tr("<add new signal>"))));
    signal->setInsertItem(true);
    signal_group->addChild(signal);

    e = new CTextTreeData();
    e->setData(0, QVariant(QString(tr("Sender"))));
    root->addChild(e);

    CSenderTreeData *sender = new CSenderTreeData();
    sender->setData(0, QVariant(QString(tr("<add new>"))));
    sender->setInsertItem(true);
    e->addChild(sender);

    mapper = new QDataWidgetMapper();

    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new Delegate(this));

    ui->treeView->setModel(model);
    // Hide all colums except first, other data is shown by QDataWidgetMapper
    for ( int i=1; i<100; i++ )
        ui->treeView->setColumnHidden(i, true);


}

void DialogSignalEditor::showEvent(QShowEvent *)
{
    empty_tree();

    ui->treeView->expandAll();
}

void DialogSignalEditor::Save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select signal definition"), "", tr("XML document (*.xml)"));

    if ( fileName == "" )
        return;

    QFile out_file(fileName);
    if ( !out_file.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        QMessageBox msgBox;
        msgBox.setText("Could not open file to write: " + fileName);
        msgBox.exec();
        return;
    }
    QTextStream out(&out_file);


    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;

    out << "<raw_view>" << endl;

    foreach ( CEditTreeData *raw, *(root->children()->at(0)->children()) )
    {
        if ( raw->insertItem() )
            continue;

        out << QString("<filter eid=\"%1\" sid=\"%2\" id_mask=\"%3\" dlc_mask=\"%4\" rtr_mask=\"%5\" data_mask=\"%6\" />")
               .arg(raw->data(1).toInt() != 1 ? "false" : "true")
               .arg(raw->data(1).toInt() != 0 ? "false" : "true")
               .arg(raw->data(4).toString())
               .arg(raw->data(2).toString())
               .arg(raw->data(3).toString())
               .arg(raw->data(5).toString()) << endl;
    }

    out << "</raw_view>" << endl << endl << "<scales>";

    out << "</scales>" << endl << endl;

    foreach ( CEditTreeData *group, *(root->children()->at(2)->children()) )
    {
        if ( group->insertItem() )
            continue;

        out << QString("<signal_group name=\"%1\">").arg(group->data(0).toString()) << endl;

        foreach (CEditTreeData *signal, *(group->children()) )
        {
            if ( signal->insertItem() )
                continue;

            out << QString("<signal name=\"%1\">").arg(group->data(0).toString());

            out << QString("<filter eid=\"%1\" sid=\"%2\" id_mask=\"%3\" dlc_mask=\"%4\" rtr_mask=\"%5\" data_mask=\"%6\" />")
                   .arg(signal->data(1).toInt() != 1 ? "false" : "true")
                   .arg(signal->data(1).toInt() != 0 ? "false" : "true")
                   .arg(signal->data(4).toString())
                   .arg(signal->data(2).toString())
                   .arg(signal->data(3).toString())
                   .arg(signal->data(5).toString()) << endl;

            out << QString("<display type=\"");
            //               float
            out << QString("\" unit=\"%1\" display_mask=\"%2\" />")
                   .arg(signal->data(8).toString())
                   .arg(signal->data(6).toString()) << endl;

            out << "</signal>" << endl;
        }

        out << "<signal_group>" << endl;
    }

    out.flush();
    out_file.close();
}

void DialogSignalEditor::Open()
{

}

void DialogSignalEditor::Close()
{
    close();
}

void DialogSignalEditor::on_buttonBox_clicked(QAbstractButton *button)
{
    if ( button == ui->buttonBox->button(QDialogButtonBox::Close) )
    {
        Close();
    } else if ( button == ui->buttonBox->button(QDialogButtonBox::Save) )
    {
        Save();
    } else if ( button == ui->buttonBox->button(QDialogButtonBox::Open) )
    {
        Open();
    } else if ( button == ui->buttonBox->button(QDialogButtonBox::Reset) )
    {
        // Restores the values
        mapper->revert();
    }
}

void DialogSignalEditor::on_treeView_clicked(const QModelIndex &index)
{
    // Get the item clicked on
    CEditTreeData *item = static_cast<CEditTreeData*>(index.internalPointer());

    qDebug() << item->classname() << "  " << item->data(0);

    // Save old data
    mapper->submit();

    // Add filter mapping
    mapper->addMapping(ui->filter_ID_select, 1);
    mapper->addMapping(ui->filter_DLC, 2);
    mapper->addMapping(ui->filter_RTR, 3);
    mapper->addMapping(ui->filter_ID, 4);
    mapper->addMapping(ui->filter_Data, 5);

    // Add display mapping
    mapper->addMapping(ui->disp_Mask, 6);
    mapper->addMapping(ui->disp_Type, 7);
    mapper->addMapping(ui->disp_Unit, 8);

    // Insert script here

    // Add Graph mapping
    mapper->addMapping(ui->graph_Color, 10);
    mapper->addMapping(ui->graph_Scale, 11);

    QString cn = item->classname();

    ui->filter_ID_select->setEnabled( !item->insertItem() && (cn == "CRawTreeData" || cn == "CSignalTreeData" || cn == "CSenderTreeData") );
    ui->filter_DLC->setEnabled( !item->insertItem() && (cn == "CRawTreeData" || cn == "CSignalTreeData" || cn == "CSenderTreeData") );
    ui->filter_RTR->setEnabled( !item->insertItem() && (cn == "CRawTreeData" || cn == "CSignalTreeData") );
    ui->filter_ID->setEnabled( !item->insertItem() && (cn == "CRawTreeData" || cn == "CSignalTreeData" || cn == "CSenderTreeData") );
    ui->filter_Data->setEnabled( !item->insertItem() && (cn == "CRawTreeData" || cn == "CSignalTreeData" || cn == "CSenderTreeData") );

    ui->disp_Mask->setEnabled( !item->insertItem() && cn == "CSignalTreeData" );
    ui->disp_btn_edit->setEnabled( !item->insertItem() && cn == "CSignalTreeData" );
    ui->disp_Type->setEnabled( !item->insertItem() && cn == "CSignalTreeData" );
    ui->disp_Unit->setEnabled( !item->insertItem() && cn == "CSignalTreeData" );

    ui->graph_Color->setEnabled( !item->insertItem() && cn == "CSignalTreeData" );
    ui->graph_Scale->setEnabled( !item->insertItem() && cn == "CSignalTreeData" );


    mapper->setRootIndex(index.parent());
    mapper->setCurrentModelIndex(index);
}

void DialogSignalEditor::on_treeView_activated(const QModelIndex &index)
{
    on_treeView_clicked(index);
}

void DialogSignalEditor::on_disp_Type_currentIndexChanged(int index)
{
    ui->disp_Mask->setEnabled(index > 0);
    ui->disp_btn_edit->setEnabled(index == 1 );
}

