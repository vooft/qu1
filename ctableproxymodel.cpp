#include "ctreemodel.h"
#include "ctableproxymodel.h"

CTableProxyModel::CTableProxyModel(QObject *parent) :
    QAbstractProxyModel(parent)
{
}

QVariant CTableProxyModel::data(const QModelIndex &index, int role) const
{
    if(index.row()==0) {
        if(role==Qt::DisplayRole)
            return "..";

        return QVariant();
    }

    return sourceModel()->data(mapToSource(index), role);
}

QModelIndex CTableProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return index(sourceIndex.row()+1, sourceIndex.column());
}

QModelIndex CTableProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(proxyIndex.row()==0)
        return QModelIndex();

    return sourceModel()->index(proxyIndex.row()-1, proxyIndex.column());
}

QModelIndex CTableProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!parent.isValid())
        return QModelIndex();

    return createIndex(row, column);
}

int CTableProxyModel::rowCount(const QModelIndex &parent) const
{
    return sourceModel()->rowCount(mapToSource(parent))+1;
}

int CTableProxyModel::columnCount(const QModelIndex &parent) const
{
    return sourceModel()->columnCount(mapToSource(parent));
}

void CTableProxyModel::expanded(const QModelIndex &index)
{
    CTreeModel *srcModel = qobject_cast<CTreeModel*>(sourceModel());
    srcModel->expanded(mapToSource(index));
}


QModelIndex CTableProxyModel::parent(const QModelIndex &index) const
{
    return mapFromSource(sourceModel()->parent(mapToSource(index)));
}
