#include <QtNetwork>

#include "cglobals.h"
#include "ctreeitem.h"
#include "croottreeitem.h"

CRootTreeItem::CRootTreeItem(const QString &path, CTreeItem *parent):
    CTreeItem(path, "ROOT", parent)
{
    setHasChildren(true);
}

void CRootTreeItem::receiveListing()
{
    DEBUG << "receiveListing(): path:" << m_path;

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    QByteArray data = reply->readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument:: fromJson(data, &error);

    QJsonObject obj = doc.object();

    QJsonArray arr = obj.value("user_node_paths").toArray();

    for(int i=0, count=arr.size(); i<count; i++) {
        QJsonValue v = arr.at(i);
        CTreeItem *item = new CTreeItem(v.toString(), v.toString(), this);
        item->setIsFile(false);
        item->setHasChildren(true);
        //item->refresh();
        appendChild(item);
    }

    emit refreshed();
}
