#include <QtCore>
#include <QtNetwork>
#include <QtGui>

#include "o1.h"
#include "o1requestor.h"
#include "o2settingsstore.h"
#include "o2globals.h"
#include "cstorage.h"
#include "cglobals.h"

#include "cubuntuoneservice.h"

CUbuntuOneService* CUbuntuOneService::m_instance = 0;

CUbuntuOneService::CUbuntuOneService(QObject *parent) :
    QObject(parent), m_oauth(0), m_requestor(0)
{
    m_instance = this;

    m_manager = new QNetworkAccessManager(this);

    m_oauth = new O1(this);
    m_oauth->setStore(new O2SettingsStore(CStorage::settingsInstance(), OAUTH_STORAGE_KEY));
    m_oauth->setRequestTokenUrl(QUrl("https://one.ubuntu.com/oauth/request/"));
    m_oauth->setAccessTokenUrl(QUrl("https://one.ubuntu.com/oauth/access/"));

    QUrl authUrl("https://one.ubuntu.com/oauth/authorize/");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("description", QString("Ubuntu One @ %1 [qu1]").arg(QHostInfo().hostName()));

    authUrl.setQuery(urlQuery);

    m_oauth->setAuthorizeUrl(authUrl);

    m_oauth->setClientId(CStorage::consumerKey());
    m_oauth->setClientSecret(CStorage::consumerSecret());

    connect(m_oauth, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(m_oauth, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
    connect(m_oauth, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(m_oauth, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(m_oauth, SIGNAL(closeBrowser()), this, SLOT(onCloseBrowser()));
}

CUbuntuOneService* CUbuntuOneService::instance()
{
    return m_instance;
}

void CUbuntuOneService::link()
{
    m_oauth->link();
}

void CUbuntuOneService::unlink()
{
    m_oauth->unlink();
}

void CUbuntuOneService::onLinkedChanged()
{
    DEBUG << "onLinkedChanged()";
}

void CUbuntuOneService::onLinkingFailed()
{
    DEBUG << "onLinkingFailed()";
    emit loginFailed();
}

void CUbuntuOneService::onLinkingSucceeded()
{
    DEBUG << "onLinkingSucceeded()";
    emit loginSucceed();
}

void CUbuntuOneService::onOpenBrowser(const QUrl &url)
{
    DEBUG << "onOpenBrowser()" << url;
    QDesktopServices::openUrl(url);
}

void CUbuntuOneService::onCloseBrowser()
{
    DEBUG << "onCloseBrowser()";
}

void CUbuntuOneService::onTokenRequestFinished()
{
    DEBUG << "onTokenRequestFinished";

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    DEBUG << "error:" << reply->errorString();

    QByteArray data = reply->readAll();
    DEBUG << "reply:" << data;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument:: fromJson(data, &error);

    DEBUG << error.errorString();

    QJsonObject obj = doc.object();

    QString consumer_key = obj.value("consumer_key").toString();
    QString consumer_secret = obj.value("consumer_secret").toString();
    QString token = obj.value("token").toString();
    QString token_secret = obj.value("token_secret").toString();

    CStorage::setConsumerKey(consumer_key);
    CStorage::setConsumerSecret(consumer_secret);
    CStorage::setToken(token);
    CStorage::setTokenSecret(token_secret);

    m_oauth->setClientId(consumer_key);
    m_oauth->setClientSecret(consumer_secret);

    DEBUG << "oauth consumer_key received";
}

void CUbuntuOneService::onCreateVolumeFinished()
{
    DEBUG << "onCreateVolumeFinished";

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    if(reply->error()!=QNetworkReply::NoError) {
        DEBUG << "error:" << reply->errorString();
        emit error(reply->errorString());
    } else {
        emit message(QString::fromUtf8("Том создан"));
    }

    QByteArray data = reply->readAll();
    DEBUG << "reply:" << data;
}

void CUbuntuOneService::onCreateFolderFinished()
{
    DEBUG << "onCreateFolderFinished";

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    if(reply->error()!=QNetworkReply::NoError) {
        DEBUG << "error:" << reply->errorString();
        emit error(reply->errorString());
    } else {
        emit message(QString::fromUtf8("Папка создана"));
    }

    QByteArray data = reply->readAll();
    DEBUG << "reply:" << data;
}

void CUbuntuOneService::onUploadFileFinished()
{
    DEBUG << "onUploadFileFinished";

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    if(reply->error()!=QNetworkReply::NoError) {
        DEBUG << "error:" << reply->errorString();
        emit error(reply->errorString());
    } else {
        QByteArray data = reply->readAll();
        DEBUG << "reply:" << data;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        QString size = QString::number(obj.value("size").toDouble());
        QString orig_size = reply->property("size").toString();

        if(size==orig_size) {
            emit message(QString::fromUtf8("Файл загружен"));
            DEBUG << "size" << size << "==" << orig_size;
        } else {
            emit error(QString::fromUtf8("Файл загружен, но размеры не совпали: %1 != %2").arg(size, orig_size));
        }
    }
}

QNetworkReply *CUbuntuOneService::get(const QString &path)
{
    DEBUG << "get():" << path;

    QList<O1RequestParameter> reqParams = QList<O1RequestParameter>();
    //reqParams.append(O1RequestParameter("include_children", "true"));

    if(m_requestor==0)
        m_requestor = new O1Requestor(m_manager, m_oauth, this);

    // Using Twitter's REST API ver 1.1
    QUrl url;
    if(path=="ROOT")
        url = QUrl("https://one.ubuntu.com/api/file_storage/v1");
    else
        url = QUrl("https://one.ubuntu.com/api/file_storage/v1" + path + (path.length()>0 ? "/?include_children=true" : ""));

    DEBUG << "request to" << url;

    QNetworkRequest request(url);
    //request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QNetworkReply *reply = m_requestor->get(request, reqParams);

    return reply;
}

QNetworkReply *CUbuntuOneService::put(const QString &path, const QByteArray &data)
{
    DEBUG << "put():" << path;

    QList<O1RequestParameter> reqParams = QList<O1RequestParameter>();
    //reqParams.append(O1RequestParameter("include_children", "true"));

    if(m_requestor==0)
        m_requestor = new O1Requestor(m_manager, m_oauth, this);

    // Using Twitter's REST API ver 1.1
    QUrl url;
    if(path=="ROOT")
        url = QUrl("https://one.ubuntu.com/api/file_storage/v1");
    else
        url = QUrl("https://one.ubuntu.com/api/file_storage/v1" + path);

    DEBUG << "request to" << url;

    QNetworkRequest request(url);
    //request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QNetworkReply *reply = m_requestor->put(request, reqParams, data);

    return reply;
}

void CUbuntuOneService::createVolume(const QString &name)
{
    QNetworkReply *r = put("/volumes/~/" + name, QByteArray());
    connect(r, &QNetworkReply::finished, this, &CUbuntuOneService::onCreateVolumeFinished);
}

void CUbuntuOneService::createFolder(const QString &name)
{
    QJsonObject obj;
    obj.insert("kind", QJsonValue::fromVariant(QString("directory")));

    QJsonDocument doc;
    doc.setObject(obj);

    QNetworkReply *r = put(name, doc.toJson());
    connect(r, &QNetworkReply::finished, this, &CUbuntuOneService::onCreateFolderFinished);
}

void CUbuntuOneService::uploadFile(const QString &localPath, const QString &remotePath)
{
    DEBUG << "uploadFile():" << localPath << remotePath;

    QList<O1RequestParameter> reqParams = QList<O1RequestParameter>();
    //reqParams.append(O1RequestParameter("include_children", "true"));

    if(m_requestor==0)
        m_requestor = new O1Requestor(m_manager, m_oauth, this);

    // Using Twitter's REST API ver 1.1
    QUrl url;
    url = QUrl("https://files.one.ubuntu.com/content" + remotePath);

    DEBUG << "request to" << url;

    QFile f(localPath);
    if(f.open(QIODevice::ReadOnly)==false) {
        DEBUG << "unable to open file" << f.errorString();
        return;
    }

    QByteArray data = f.readAll();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");

    QNetworkReply *reply = m_requestor->put(request, reqParams, data);
    reply->setProperty("size", QString::number(data.size()));

    DEBUG << "request sent";

    connect(reply, &QNetworkReply::finished, this, &CUbuntuOneService::onUploadFileFinished);
}

