#include <QtConcurrent/QtConcurrent>

#include "cglobals.h"
#include "cubuntuonetask.h"

#include "cuploaddialog.h"
#include "ui_cuploaddialog.h"

#define FULL_PATH 0
#define REL_PATH 1
#define IS_DIR 2
#define RESULT 3

CUploadDialog::CUploadDialog(const QString &localDir, const QString &remoteDir, CUbuntuOneService *service, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CUploadDialog), m_localDir(localDir), m_remoteDir(remoteDir), m_service(service)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "path" << "relPath" << "isDir" << "result");

    connect(this, &CUploadDialog::addRow, this, &CUploadDialog::doAddRow);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CUploadDialog::doUpload);

    m_abort = false;
    QFuture<void> func = QtConcurrent::run(this, &CUploadDialog::doImport, m_localDir);
}

CUploadDialog::~CUploadDialog()
{
    delete ui;
}

void CUploadDialog::closeEvent(QCloseEvent *evnt)
{
    m_abort = true;
    QThreadPool::globalInstance()->waitForDone();

    QDialog::closeEvent(evnt);
}

void CUploadDialog::doAddRow(const QString &path, bool isDir)
{
    DEBUG << "doAddRow():" << path << isDir;

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    QTableWidgetItem* pathItem = new QTableWidgetItem(path);
    ui->tableWidget->setItem(row, FULL_PATH, pathItem);

    QString relPath = path.right(path.length() - m_localDir.length());
    QTableWidgetItem* relPathItem = new QTableWidgetItem(relPath);
    ui->tableWidget->setItem(row, REL_PATH, relPathItem);

    QTableWidgetItem* isDirItem = new QTableWidgetItem(isDir ? "yes" : "no");
    ui->tableWidget->setItem(row, IS_DIR, isDirItem);

    QTableWidgetItem* resultItem = new QTableWidgetItem();
    ui->tableWidget->setItem(row, RESULT, resultItem);
}

void CUploadDialog::doUpload()
{
    QFile logFile("/home/vooft/upload.log");
    logFile.open(QIODevice::WriteOnly);

    QTextStream log(&logFile);

    QEventLoop loop;
    connect(m_service, &CUbuntuOneService::message, &loop, &QEventLoop::quit);
    connect(m_service, &CUbuntuOneService::error, &loop, &QEventLoop::quit);

    bool dirs = ui->dirCheckBox->isChecked();
    bool files = ui->fileCheckBox->isChecked();

    for(int i=0, count = ui->tableWidget->rowCount(); i<count; i++) {
        QString path = ui->tableWidget->item(i, FULL_PATH)->text();
        QString relPath = ui->tableWidget->item(i, REL_PATH)->text();
        QString isDir = ui->tableWidget->item(i, IS_DIR)->text();

        QTableWidgetItem *resultItem = ui->tableWidget->item(i, RESULT);

        CUbuntuOneTask task(m_service);

        bool result = false;

        if(isDir=="yes") {
            if(dirs)
                result = task.createDirectory(m_remoteDir + relPath);
        } else {
            if(files)
                result = task.uploadFile(path, m_remoteDir + relPath);
        }

        if(result) {
            log << "ok " << path;
            resultItem->setText("ok");
        } else {
            log << "fail " << path;
            resultItem->setText(task.errorString());
            resultItem->setBackgroundColor(Qt::red);
        }

        log << "\n";

        log.flush();

        if(m_abort)
            break;
    }

    logFile.close();
}


void CUploadDialog::doImport(const QString &str)
{
    QStringList dirs = findDirectories(str);
    DEBUG << "doImport(): found directories:" << dirs;

    foreach(QString str, dirs) {
        emit addRow(str, true);
    }

    foreach(QString str, dirs) {
        if(m_abort)
            break;

        QDir dir(str);
        QStringList filesList = dir.entryList(QDir::Files);
        foreach(QString filename, filesList) {
            if(m_abort)
                break;
            emit addRow(dir.absoluteFilePath(filename), false);
        }
    }
}

QStringList CUploadDialog::findDirectories(const QString &str)
{
    QStringList result;
    result.append(str);

    QDir dir(str);
    QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QString subdir, subdirs) {
        if(m_abort)
            return result;

        DEBUG << "findDirectories():" << str << subdir;
        result.append(findDirectories(dir.absoluteFilePath(subdir)));
    }

    return result;
}
