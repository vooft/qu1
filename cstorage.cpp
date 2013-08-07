#include <QtGui>

#include "cstorage.h"

CStorage *CStorage::m_instance = 0;

CStorage::CStorage(QObject *parent) :
    QObject(parent)
{
    m_settings = new QSettings(QString("qu1.ini"), QSettings::IniFormat);
    m_settings->setIniCodec("UTF-8");
}

CStorage::~CStorage()
{
    m_settings->sync();
    m_settings->deleteLater();
}

CStorage* CStorage::instance()
{
    if(m_instance == 0)
        m_instance = new CStorage;

    return m_instance;
}

QString CStorage::token()
{
    return setting("token").toString();
}

void CStorage::setToken(const QString &str)
{
    setSetting("token", str);
}

QString CStorage::consumerKey()
{
    //return setting("consumer").toString();
    return "ubuntuone";
}

void CStorage::setConsumerKey(const QString &str)
{
    setSetting("consumer", str);
}

QString CStorage::consumerSecret()
{
    //return setting("consumer_secret").toString();
    return "hammertime";
}

void CStorage::setConsumerSecret(const QString &str)
{
    setSetting("consumer_secret", str);
}

QSettings *CStorage::settingsInstance()
{
    return instance()->m_settings;
}

QString CStorage::tokenSecret()
{
    return setting("token_secret").toString();
}

void CStorage::setTokenSecret(const QString &str)
{
    setSetting("token_secret", str);
}

QVariant CStorage::setting(const QString & key, const QVariant & defaultValue)
{
    return instance()->setting_p(key, defaultValue);
}

QVariant CStorage::setting_p(const QString & key, const QVariant & defaultValue)
{
    m_settingsMutex.lockForRead();
    if(!m_settings->contains(key))
    {
        m_settingsMutex.unlock();
        setSetting(key, defaultValue);
        return defaultValue;
    }

    QVariant result = m_settings->value(key);

    m_settingsMutex.unlock();

    return result;
}

void CStorage::setSetting(const QString &key, const QVariant &value)
{
    instance()->setSetting_p(key, value);
}

void CStorage::setSetting_p(const QString & key, const QVariant & value)
{
    m_settingsMutex.lockForWrite();
    m_settings->setValue(key, value);
    m_settingsMutex.unlock();
}

#undef DEBUG
