#ifndef CSTORAGE_H
#define CSTORAGE_H

#include <QtCore>
#include "cglobals.h"

class CStorage : public QObject
{
    Q_OBJECT

    static CStorage *m_instance;
    QSettings *m_settings;
    QReadWriteLock m_settingsMutex;

public:
    static CStorage *instance();
    ~CStorage();

    static QVariant setting(const QString & key, const QVariant & defaultValue = QVariant());
    static void setSetting(const QString & key, const QVariant & value);

    static QString tokenSecret();
    static void setTokenSecret(const QString&);

    static QString token();
    static void setToken(const QString&);

    static QString consumerKey();
    static void setConsumerKey(const QString&);

    static QString consumerSecret();
    static void setConsumerSecret(const QString&);

    static QSettings *settingsInstance();

private:
    explicit CStorage(QObject *parent = 0);

    QVariant setting_p(const QString & key, const QVariant & defaultValue = QVariant());
    void setSetting_p(const QString & key, const QVariant & value);

public slots:


};

#endif // CSTORAGE_H
