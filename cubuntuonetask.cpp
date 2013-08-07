#include <QtCore>

#include "cubuntuonetask.h"

CUbuntuOneTask::CUbuntuOneTask(CUbuntuOneService *service, QObject *parent) :
    QObject(parent), m_service(service), m_state(false)
{
    connect(m_service, &CUbuntuOneService::error, this, &CUbuntuOneTask::error);
    connect(m_service, &CUbuntuOneService::message, this, &CUbuntuOneTask::message);
}

QString CUbuntuOneTask::errorString() const
{
    return m_errorString;
}


bool CUbuntuOneTask::createDirectory(const QString &path)
{
    QEventLoop loop;
    connect(this, &CUbuntuOneTask::finished, &loop, &QEventLoop::quit);

    m_service->createFolder(path);
    loop.exec();

    return m_state;
}

bool CUbuntuOneTask::uploadFile(const QString &localPath, const QString &remotePath)
{
    QEventLoop loop;
    connect(this, &CUbuntuOneTask::finished, &loop, &QEventLoop::quit);

    m_service->uploadFile(localPath, remotePath);
    loop.exec();

    return m_state;
}

void CUbuntuOneTask::error(const QString &msg)
{
    m_state = false;
    m_errorString = msg;
    emit finished();
}

void CUbuntuOneTask::message(const QString &msg)
{
    m_state = true;
    m_errorString = msg;
    emit finished();
}
