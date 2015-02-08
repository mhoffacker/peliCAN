#ifndef DIALOGSIGNALEDITOR_H
#define DIALOGSIGNALEDITOR_H

#include <QDialog>
#include <QAbstractButton>
#include <QVariant>
#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QItemDelegate>
#include <QItemSelection>

class CEditTreeData;

typedef QList<CEditTreeData*> DataList;

class CEditTreeData
{
public:
    CEditTreeData()
    {
        m_parent = NULL;
        m_insert_item = false;

        // Build empty list
        for ( int i=0; i<12; i++ )
            m_data.append(QVariant());
    }

    virtual ~CEditTreeData() { clear(); }

    virtual QString classname() { return QString("CEditTreeData"); }

    // Properties
    CEditTreeData* parent() { return m_parent; }
    void setParent(CEditTreeData *parent) { m_parent = parent; }

    bool insertItem() { return m_insert_item; }
    void setInsertItem(bool insert_item) { m_insert_item = insert_item; }

    DataList* children() { return &m_childItems; }
    void addChild(CEditTreeData *new_child) { m_childItems.append(new_child); new_child->setParent(this); }
    CEditTreeData *child(int row) { return m_childItems.value(row); }
    int childCount() { return m_childItems.count(); }

    int row() { return ((m_parent != NULL) ? m_parent->children()->indexOf(const_cast<CEditTreeData*>(this)) : 0); }

    int columnCount() { return m_data.count(); }
    QVariant data(int column) { return m_data.at(column); }
    void setData(int column, QVariant data) { m_data[column] = data; }

    virtual bool isEditable() const = 0;
    virtual bool canDragDrop() const = 0;

    void clear()
    {
        while ( !m_childItems.empty() )
        {
            delete m_childItems.first();
            m_childItems.removeFirst();
        }
    }

private:
    CEditTreeData* m_parent;
    bool m_insert_item;

    DataList m_childItems;

    QList<QVariant> m_data;

};

class CTextTreeData : public CEditTreeData
{
public:
    CTextTreeData();
    ~CTextTreeData();

    QString classname() { return QString("CTextTreeData"); }

    bool isEditable() const { return false; }
    bool canDragDrop() const { return false; }

private:
};

class CRawTreeData : public CEditTreeData
{
public:
    CRawTreeData();
    ~CRawTreeData();

    QString classname() { return QString("CRawTreeData"); }

    bool isEditable() const { return true; }
    bool canDragDrop() const { return false; }

private:
};

class CScaleTreeData : public CEditTreeData
{
public:
    CScaleTreeData();
    ~CScaleTreeData();

    QString classname() { return QString("CScaleTreeData"); }

    bool isEditable() const { return true; }
    bool canDragDrop() const { return false; }

private:

};

class CSignalGroupTreeData : public CEditTreeData
{
public:
    CSignalGroupTreeData();
    ~CSignalGroupTreeData();

    QString classname() { return QString("CSignalGroupTreeData"); }

    bool isEditable() const { return true; }
    bool canDragDrop() const { return false; }

private:

};

class CSignalTreeData : public CEditTreeData
{
public:
    CSignalTreeData();
    ~CSignalTreeData();

    QString classname() { return QString("CSignalTreeData"); }

    bool isEditable() const { return true; }
    bool canDragDrop() const { return false; }

private:
};

class CSenderTreeData : public CEditTreeData
{
public:
    CSenderTreeData();
    ~CSenderTreeData();

    QString classname() { return QString("CSenderTreeData"); }

    bool isEditable() const { return true; }
    bool canDragDrop() const { return false; }

private:
};

class CEditTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CEditTreeModel(QObject *parent = 0);


    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                      int role = Qt::EditRole);


    void setRoot(CEditTreeData *root) { m_root = root; }
    CEditTreeData* root() { return m_root; }

    void DataHasChanged(CEditTreeData *sig);


private:
    CEditTreeData *m_root;

signals:

public slots:

};

class Delegate : public QItemDelegate
{
    Q_OBJECT
public:
    Delegate(QObject *parent = 0);
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
};

namespace Ui {
class DialogSignalEditor;
}

class DialogSignalEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSignalEditor(QWidget *parent = 0);
    ~DialogSignalEditor();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_treeView_clicked(const QModelIndex &index);

    void on_treeView_activated(const QModelIndex &index);

    void on_disp_Type_currentIndexChanged(int index);

private:
    Ui::DialogSignalEditor *ui;

    void showEvent(QShowEvent *event);

    void empty_tree();

    void Save();
    void Open();
    void Close();

    CTextTreeData *root;
    CEditTreeModel *model;
    QDataWidgetMapper *mapper;
};

#endif // DIALOGSIGNALEDITOR_H
