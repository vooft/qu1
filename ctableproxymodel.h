#ifndef CTABLEPROXYMODEL_H
#define CTABLEPROXYMODEL_H

#include <QAbstractProxyModel>

class CTableProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit CTableProxyModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;


    QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
signals:
    
public slots:
    void expanded(const QModelIndex &index);
    
};

#endif // CTABLEPROXYMODEL_H
