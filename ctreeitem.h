#ifndef CTREEITEM_H
#define CTREEITEM_H

#include <QtCore>

class CTreeItem: public QObject
{
    Q_OBJECT

public:
    CTreeItem(const QString &path, const QString &key, CTreeItem *parent = 0);
    ~CTreeItem();

    QString path() const;

    void appendChild(CTreeItem *child);

    CTreeItem *child(int row);
    int childCount() const;

    int row() const;
    CTreeItem *parent();

    bool isFile() const;
    void setIsFile(bool state);

    QString fullPath() const;
    QString volume() const;

    bool hasChildren() const;
    void setHasChildren(bool state);

    QString hash() const;
    void setHash(const QString &str);

    QString key() const;

    void clear();

public slots:
    void refresh();

protected slots:
    virtual void receiveListing();

signals:
    void refreshed();

protected:
    QList<CTreeItem*> m_children;
    CTreeItem *m_parent;
    QString m_path;
    QString m_hash;
    QString m_key;
    bool m_isFile;
    bool m_hasChildren;
};

#endif // CTREEITEM_H
