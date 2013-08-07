#ifndef CGLOBALS_H
#define CGLOBALS_H

#include <QtCore>
#include <QtDebug>

typedef QHash<QString, QString> QStringMap;

#define DEBUG (qDebug() << qPrintable(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")) \
    << qPrintable(QString("%1:%2").arg(__FILE__).arg(__LINE__)) )

#define WARNING (qWarning() << qPrintable(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")) \
    << qPrintable(QString("%1:%2").arg(__FILE__).arg(__LINE__)) )


#define OAUTH_STORAGE_KEY "F7piT6Cl1JPj"
#define ITEM_KEY_ROLE (Qt::UserRole)

#endif // CGLOBALS_H
