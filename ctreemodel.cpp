#include "ctreemodel.h"
#include "ctreeitem.h"
#include "croottreeitem.h"
#include "cglobals.h"

#include <QDataStream>

#include <QtGui>

CTreeModel::CTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = createRoot();
    connect(rootItem, &CRootTreeItem::refreshed, this, &CTreeModel::rootRefreshed);
    rootItem->refresh();

    //this->setSupportedDragActions(Qt::MoveAction);
}

CTreeModel::~CTreeModel()
{
    delete rootItem;
}

CTreeItem* CTreeModel::createRoot()
{
    return new CRootTreeItem("");
}

int CTreeModel::columnCount(const QModelIndex &/*parent*/) const
{
/*    if (parent.isValid())
        return static_cast<CTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();*/
    return 2;
}

bool CTreeModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        // это тогда root
        DEBUG << "hasChildren: invalid index";
        return true;
    }

    CTreeItem *item = static_cast<CTreeItem*>(parent.internalPointer());

    return item->hasChildren();
}

QString CTreeModel::fullPath(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());
    return item->fullPath();
}

QString CTreeModel::volume(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());
    return item->volume();
}

void CTreeModel::expanded(const QModelIndex &index)
{
    DEBUG << "expanded";

    if (!index.isValid())
        return;

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());
    connect(item, &CTreeItem::refreshed, this, &CTreeModel::itemRefreshed);
    beginRemoveRows(index, 0, item->childCount());
    item->refresh();
}

void CTreeModel::collapsed(const QModelIndex &index)
{
    DEBUG << "collapsed";

    if (!index.isValid())
        return;

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());

    beginRemoveRows(index, 0, item->childCount());
    item->clear();
    endRemoveRows();
}

void CTreeModel::itemRefreshed()
{
    DEBUG << "itemRefreshed()";

    endRemoveRows();

    CTreeItem *item = qobject_cast<CTreeItem*>(sender());
    disconnect(item, &CTreeItem::refreshed, this, &CTreeModel::itemRefreshed);

    QModelIndexList list = match(this->index(0, 0), ITEM_KEY_ROLE, item->key(), 1, Qt::MatchRecursive);

    if(list.size()==0) {
        DEBUG << "index not found for " << item->path();
        return;
    }

    QModelIndex index = list.at(0);

    beginInsertRows(index, 0, item->childCount()-1);
    endInsertRows();

    DEBUG << "inserted rows:" << item->childCount();
}

void CTreeModel::rootRefreshed()
{
    beginResetModel();
    endResetModel();
}

QVariant CTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());

    switch(role)
    {
        case Qt::DisplayRole:
            if(index.column()==0)
                return item->path();
            else
                return item->hash();
        case ITEM_KEY_ROLE: return item->key();
        default: return QVariant();
    }

    DEBUG << "data(): wtf?";

    return QVariant();
}

/*Qt::ItemFlags CTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}*/

QVariant CTreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if(role!=Qt::DisplayRole)
        return QVariant();

    return QString::number(section);
}

QModelIndex CTreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<CTreeItem*>(parent.internalPointer());

    CTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

    return QModelIndex();
}

QModelIndex CTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    CTreeItem *childItem = static_cast<CTreeItem*>(index.internalPointer());
    CTreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CTreeModel::rowCount(const QModelIndex &parent) const
{
    CTreeItem *parentItem = 0;

    if (!parent.isValid())
    {
        parentItem = rootItem;
    }
    else
    {
        parentItem = static_cast<CTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

/*Qt::DropActions CTreeModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList CTreeModel::mimeTypes() const
{
    QStringList result;
    //result << MIME_SECTION;
    //result << MIME_ENTRY;
    return result;
}

QMimeData* CTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream in(&encodedData, QIODevice::WriteOnly);

    QModelIndex idx = indexes.at(0);
    CTreeItem *item = static_cast<CTreeItem*>(idx.internalPointer());
    CTreeItem *parent = item->parent();

    in << item->id();
    in << parent->id();

    mimeData->setData(MIME_SECTION, encodedData);

    return mimeData;
}

bool CTreeModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if(column>0)
        return false;

    CTreeItem *newParent;
    if(parent.isValid())
        newParent = static_cast<CTreeItem*>(parent.internalPointer());
    else
        newParent = rootItem;

    if(data->hasFormat(MIME_SECTION))
    {
        QString id;
        QString oldParentId;

        QDataStream out(data->data(MIME_SECTION));
        out >> id;
        out >> oldParentId;

        CTreeItem *oldParent = rootItem->findById(oldParentId);
        if(oldParent==0)
        {
            refill();
            return true;
        }

        moveSection(id, newParent->id());

        refill();

        return true;
    }
    else if(data->hasFormat(MIME_ENTRY))
    {
        if(newParent==rootItem)
            return false;

        QString id;
        QDataStream out(data->data(MIME_ENTRY));
        out >> id;

        debug("Moving entry: " + id);

        moveEntry(id, newParent->id());
        refill(false);
        emit update(true);
        return true;
    }

    return false;
}*/
