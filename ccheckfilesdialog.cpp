#include <QtCore>
#include <QtConcurrent/QtConcurrent>

#include "cglobals.h"
#include "cubuntuonetask.h"
#include "ccheckfilesdialog.h"
#include "ui_ccheckfilesdialog.h"

#define FULL_PATH 0
#define REL_PATH 1
#define IS_DIR 2
#define HASH 3

CCheckFilesDialog::CCheckFilesDialog(const QString &localPath, const QString &remotePath, const QString &volume,
                                     CUbuntuOneService *service, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CCheckFilesDialog), m_localPath(localPath), m_remotePath(remotePath),
    m_service(service), m_volume(volume)
{
    ui->setupUi(this);
    m_abort = false;

    ui->localPathLabel->setText(m_localPath);
    ui->remotePathLabel->setText(m_remotePath);

    ui->localTableWidget->setColumnCount(4);
    ui->localTableWidget->setHorizontalHeaderLabels(QStringList() << "path" << "relPath" << "isDir" << "hash");

    ui->remoteTableWidget->setColumnCount(4);
    ui->remoteTableWidget->setHorizontalHeaderLabels(QStringList() << "path" << "relPath" << "isDir" << "hash");

    connect(this, &CCheckFilesDialog::addRow, this, &CCheckFilesDialog::doAddRow);
    //connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CUploadDialog::doUpload);
    connect(this, &CCheckFilesDialog::importFinished, this, &CCheckFilesDialog::onImportFinished);
}

CCheckFilesDialog::~CCheckFilesDialog()
{
    delete ui;
}

void CCheckFilesDialog::on_scanLocalButton_clicked()
{
    ui->localTableWidget->clear();

    ui->progressBar->setMaximum(0);

    m_abort = false;
    QFuture<void> func = QtConcurrent::run(this, &CCheckFilesDialog::fillLocalStorage, m_localPath);
}

void CCheckFilesDialog::on_scanRemoteButton_clicked()
{   
    ui->progressBar->setMaximum(0);
    ui->remoteTableWidget->clear();
    fillRemoteStorage(m_remotePath, &m_remoteStorage, ui->remoteTableWidget);

    /*DEBUG << "adding remote dirs: " << m_remoteStorage.dirs.size();
    for(int i=0, count=m_remoteStorage.dirs.count(); i<count; i++) {
        emit addRow(ui->remoteTableWidget, m_remoteStorage.dirs.at(i), m_remotePath, true, "");
        QApplication::processEvents();
    }

    DEBUG << "adding remote files: " << m_remoteStorage.files.size();

    ui->progressBar->setMaximum(m_remoteStorage.files.size());
    ui->progressBar->setValue(0);

    QHashIterator<QString, QString> i(m_remoteStorage.files);
    while (i.hasNext()) {
        i.next();

        QString file = i.key();
        QString hash = i.value();

        emit addRow(ui->remoteTableWidget, file, m_remotePath, false, hash);
        QApplication::processEvents();

        ui->progressBar->setValue(ui->progressBar->value()+1);
    }*/

    QApplication::processEvents();

    DEBUG << "creating relFiles";

    m_remoteStorage.relFiles = createRelative(ui->remoteTableWidget);

    DEBUG << "adding remote files finished";
}

void CCheckFilesDialog::fillLocalStorage(const QString &str)
{
    QStringList dirs = findLocalDirectories(str);
    DEBUG << "doImport(): found directories:" << dirs;

    m_localStorage.dirs = dirs;

    foreach(QString str, dirs) {
        emit addRow(ui->localTableWidget, str, m_localPath, true, "");
    }

    foreach(QString str, dirs) {
        if(m_abort)
            break;

        QDir dir(str);
        QStringList filesList = dir.entryList(QDir::Files);
        foreach(QString filename, filesList) {
            if(m_abort)
                break;
            emit addRow(ui->localTableWidget, dir.absoluteFilePath(filename), m_localPath, false, "");
            m_localStorage.files.insert(dir.absoluteFilePath(filename), "");
        }
    }

    DEBUG << "import finished";

    emit importFinished();
}

void CCheckFilesDialog::fillRemoteStorage(const QString &root, SU1StorageInfo *info, QTableWidget *table)
{
    CUbuntuOneTask task(m_service);
    QByteArray arr = task.queryNodeInfo(root);

    QHash<QString, QString> files = parseJsonForFiles(arr);

    QHashIterator<QString, QString> i(files);
    while (i.hasNext()) {
        i.next();

        info->files.insert(i.key(), i.value());
        emit addRow(table, i.key(), m_remotePath, false, i.value());
        QApplication::processEvents();
    }

    QStringList currentDirs = parseJsonForDirectories(arr);
    foreach(QString dir, currentDirs) {
        info->dirs.append(dir);
        emit addRow(table, dir, m_remotePath, true, "");
        fillRemoteStorage(dir, info, table);
    }
}

QStringList CCheckFilesDialog::findLocalDirectories(const QString &str)
{
    QStringList result;
    result.append(str);

    QDir dir(str);
    QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QString subdir, subdirs) {
        if(m_abort)
            return result;

        result.append(findLocalDirectories(dir.absoluteFilePath(subdir)));
    }

    return result;
}

QStringList CCheckFilesDialog::parseJsonForDirectories(const QByteArray &data) const
{
    QStringList result;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument:: fromJson(data, &error);

    QJsonObject obj = doc.object();

    QJsonArray arr = obj.value("children").toArray();

    for(int i=0, count=arr.size(); i<count; i++) {
        QJsonObject child = arr.at(i).toObject();

        if(child.value("kind").toString()!="directory") {
            continue;
        }

        result.append(m_volume + child.value("path").toString());
    }

    return result;
}

QHash<QString, QString> CCheckFilesDialog::parseJsonForFiles(const QByteArray &data) const
{
    QHash<QString, QString> result;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument:: fromJson(data, &error);

    QJsonObject obj = doc.object();

    QJsonArray arr = obj.value("children").toArray();

    for(int i=0, count=arr.size(); i<count; i++) {
        QJsonObject child = arr.at(i).toObject();

        if(child.value("kind").toString()=="directory") {
            continue;
        }

        result.insert(m_volume + child.value("path").toString(), child.value("hash").toString());
    }

    return result;
}

QHash<QString, QString> CCheckFilesDialog::createRelative(QTableWidget *table)
{
    QHash<QString, QString> result;

    ui->progressBar->setMaximum(table->rowCount());
    ui->progressBar->setValue(0);

    for(int i=0, count=table->rowCount(); i<count; i++) {
        QTableWidgetItem *isDirItem = table->item(i, IS_DIR);
        if(isDirItem->text()!="no")
            continue;

        QTableWidgetItem *pathItem = table->item(i, REL_PATH);
        QTableWidgetItem *hashItem = table->item(i, HASH);

        result.insert(pathItem->text(), hashItem->text());

        ui->progressBar->setValue(i+1);

        QApplication::processEvents();
    }

    return result;
}

void CCheckFilesDialog::doAddRow(QTableWidget *table, const QString &path, const QString &root, bool isDir, const QString &hash)
{
    //DEBUG << "doAddRow():" << path << isDir;

    QString relPath = path.right(path.length() - root.length());
    if(relPath.length()==0)
        return;

    int row = table->rowCount();
    table->insertRow(row);

    QTableWidgetItem* pathItem = new QTableWidgetItem(path);
    table->setItem(row, FULL_PATH, pathItem);

    QTableWidgetItem* relPathItem = new QTableWidgetItem(relPath);
    table->setItem(row, REL_PATH, relPathItem);

    QTableWidgetItem* isDirItem = new QTableWidgetItem(isDir ? "yes" : "no");
    table->setItem(row, IS_DIR, isDirItem);

    QTableWidgetItem* resultItem = new QTableWidgetItem(hash);
    table->setItem(row, HASH, resultItem);
}

void CCheckFilesDialog::onImportFinished()
{
    //on_hashButton_clicked();
    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(1);
}

void CCheckFilesDialog::on_hashButton_clicked()
{
    ui->progressBar->setMaximum(ui->localTableWidget->rowCount());
    for(int i=0, count=ui->localTableWidget->rowCount(); i<count; i++) {
        QTableWidgetItem *isDirItem = ui->localTableWidget->item(i, IS_DIR);
        if(isDirItem->text()!="no")
            continue;

        QTableWidgetItem *pathItem = ui->localTableWidget->item(i, FULL_PATH);

        QFile f(pathItem->text());
        if(f.open(QIODevice::ReadOnly)==false) {
            ui->errorsListWidget->addItem("Unable to open file " + pathItem->text());
            continue;
        }

        QByteArray data = f.readAll();
        QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);
        QString hashStr = QString::fromLatin1(hash.toHex());

        QTableWidgetItem *hashItem = ui->localTableWidget->item(i, HASH);
        hashItem->setText("sha1:" + hashStr);
        ui->progressBar->setValue(i+1);

        QApplication::processEvents();
    }

    QApplication::processEvents();

    m_localStorage.relFiles = createRelative(ui->localTableWidget);
}

void CCheckFilesDialog::on_createLocalRelButton_clicked()
{
    m_localStorage.relFiles = createRelative(ui->localTableWidget);
}

void CCheckFilesDialog::on_createRemoteRelButton_clicked()
{
    m_remoteStorage.relFiles = createRelative(ui->remoteTableWidget);
}

void CCheckFilesDialog::on_compareButton_clicked()
{
    ui->errorsListWidget->clear();

    QStringList localFiles = m_localStorage.relFiles.keys();
    QStringList remoteFiles = m_remoteStorage.relFiles.keys();

    ui->progressBar->setMaximum(localFiles.size());
    ui->progressBar->setValue(1);
    QStringList onlyLocal;
    foreach(QString s, localFiles) {
        if(remoteFiles.contains(s)==false) {
            onlyLocal.append(s);
            ui->errorsListWidget->addItem("only local: " + s);
        }

        QApplication::processEvents();

        ui->progressBar->setValue(ui->progressBar->value()+1);
    }

    ui->progressBar->setMaximum(remoteFiles.size());
    ui->progressBar->setValue(1);
    QStringList onlyRemote;
    foreach(QString s, remoteFiles) {
        if(localFiles.contains(s)==false) {
            onlyRemote.append(s);
            ui->errorsListWidget->addItem("only remote: " + s);
        }

        QApplication::processEvents();

        ui->progressBar->setValue(ui->progressBar->value()+1);
    }

    DEBUG << "only local: " << onlyLocal;
    DEBUG << "only remote: " << onlyRemote;

    QStringList both(localFiles);
    foreach(QString s, onlyLocal)
        both.removeAll(s);

    ui->progressBar->setMaximum(both.size());
    ui->progressBar->setValue(1);
    foreach(QString str, both) {
        if(m_localStorage.relFiles.value(str)!=m_remoteStorage.relFiles.value(str)) {
            ui->errorsListWidget->addItem("For " + str + " hashes are not equal");
        }
        QApplication::processEvents();
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }

    QMessageBox::information(this, QString::fromUtf8("Finish"), QString::fromUtf8("Comparison finished"));
}

void CCheckFilesDialog::on_pushButton_clicked()
{
    QString s = QString::fromUtf8("/2009.12.31 Новый год 2010/DSC04277.JPG");
    DEBUG << m_remoteStorage.relFiles.keys();
    DEBUG << s;
    DEBUG << "m_remoteStorage.relFiles.keys().contains(s)" << m_remoteStorage.relFiles.keys().contains(s);
}
