#ifndef CUBUNTUONETASK_H
#define CUBUNTUONETASK_H

#include <QObject>

#include "cubuntuoneservice.h"

class CUbuntuOneTask : public QObject
{
    Q_OBJECT
public:
    explicit CUbuntuOneTask(CUbuntuOneService *service, QObject *parent = 0);

    QString errorString() const;

    bool createDirectory(const QString &path);
    bool uploadFile(const QString &localPath, const QString &remotePath);
    QByteArray queryNodeInfo(const QString &path);
    
signals:
    void finished();

public slots:

private slots:
    void error(const QString &msg);
    void message(const QString &msg);

private:
    CUbuntuOneService *m_service;
    QString m_errorString;
    bool m_state;
};

#endif // CUBUNTUONETASK_H
