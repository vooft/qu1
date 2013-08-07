#include "cubuntuoneservice.h"
#include "ctreeitem.h"

#include "cglobals.h"

CTreeItem::CTreeItem(const QString &path, const QString &key, CTreeItem *parent):
    m_parent(parent), m_path(path), m_isFile(true), m_hasChildren(false), m_key(key)
{

}

CTreeItem::~CTreeItem()
{
    qDeleteAll(m_children);
}

QString CTreeItem::path() const
{
    return m_path;
}

void CTreeItem::appendChild(CTreeItem *item)
{
    m_children.append(item);
}

CTreeItem *CTreeItem::child(int row)
{
    return m_children.at(row);
}

int CTreeItem::childCount() const
{
    return m_children.size();
}

int CTreeItem::row() const
{
    if (m_parent)
        return m_parent->m_children.indexOf(const_cast<CTreeItem*>(this));
    return 0;
}

CTreeItem *CTreeItem::parent()
{
    return m_parent;
}

bool CTreeItem::isFile() const
{
    return m_isFile;
}

void CTreeItem::setIsFile(bool state)
{
    m_isFile = state;
}

QString CTreeItem::fullPath()
{
    if(m_parent==0)
        return m_path;

    return m_parent->fullPath() + m_path;
}

bool CTreeItem::hasChildren() const
{
    return m_hasChildren;
}

void CTreeItem::setHasChildren(bool state)
{
    m_hasChildren = state;
}

QString CTreeItem::hash() const
{
    return m_hash;
}

void CTreeItem::setHash(const QString &str)
{
    m_hash = str;
}

QString CTreeItem::key() const
{
    return m_key;
}

void CTreeItem::clear()
{
    qDeleteAll(m_children);
}

void CTreeItem::refresh()
{
    QNetworkReply *r = CUbuntuOneService::instance()->get(fullPath());
    connect(r, &QNetworkReply::finished, this, &CTreeItem::receiveListing);
}

void CTreeItem::receiveListing()
{
    qDeleteAll(m_children);

    DEBUG << "receiveListing(): path:" << m_path;

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    QByteArray data = reply->readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument:: fromJson(data, &error);

    QJsonObject obj = doc.object();

    QJsonArray arr = obj.value("children").toArray();

    for(int i=0, count=arr.size(); i<count; i++) {
        QJsonObject child = arr.at(i).toObject();

        QString kind = child.value("kind").toString();
        QString key = child.value("key").toString();
        QString hash = child.value("hash").toString();

        CTreeItem *item = new CTreeItem(child.value("path").toString(), key, this);
        item->setHash(hash);

        if(kind=="directory") {
            item->setIsFile(false);
            item->setHasChildren(child.value("has_children").toBool());
        }

        appendChild(item);
    }

    emit refreshed();
}
