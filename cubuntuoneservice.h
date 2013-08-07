#ifndef CUBUNTUONESERVICE_H
#define CUBUNTUONESERVICE_H

#include <QObject>

#include "o1.h"
#include "o1requestor.h"

class CUbuntuOneService : public QObject
{
    Q_OBJECT

    static CUbuntuOneService *m_instance;

public:
    explicit CUbuntuOneService(QObject *parent = 0);

    static CUbuntuOneService *instance();

    QNetworkReply *get(const QString &path = "");
    QNetworkReply *put(const QString &path, const QByteArray &data);

    void createVolume(const QString &name);
    void createFolder(const QString &name);
    void uploadFile(const QString &localPath, const QString &remotePath);

signals:
    void tokenReceived(const QString &token, const QString &token_secret);
    void loginFailed();
    void loginSucceed();
    void message(const QString &str);
    void error(const QString &str);

public slots:
    void link();
    void unlink();
    
private slots:
    void onLinkedChanged();
    void onLinkingFailed();
    void onLinkingSucceeded();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();
    void onTokenRequestFinished();
    void onCreateVolumeFinished();
    void onCreateFolderFinished();
    void onUploadFileFinished();

private:
    O1 *m_oauth;
    QNetworkAccessManager *m_manager;
    O1Requestor* m_requestor;
    
};

#endif // CUBUNTUONESERVICE_H
