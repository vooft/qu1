#ifndef CTREEMODEL_H
#define CTREEMODEL_H

#include "ctreeitem.h"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QModelIndex>
#include <QVariant>

class CTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CTreeModel(QObject *parent = 0);
    ~CTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    //Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QString fullPath(const QModelIndex &index) const;

    /*Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent);*/

public slots:
    void expanded(const QModelIndex &index);
    void collapsed(const QModelIndex &index);

private slots:
    void itemRefreshed();
    void rootRefreshed();

private:
    CTreeItem *rootItem;

    CTreeItem* createRoot();
};

#endif // CTREEMODEL_H
