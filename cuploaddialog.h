#ifndef CUPLOADDIALOG_H
#define CUPLOADDIALOG_H

#include <QtConcurrent/QtConcurrent>

#include <QDialog>

#include "cubuntuoneservice.h"

namespace Ui {
class CUploadDialog;
}

class CUploadDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CUploadDialog(const QString &localDir, const QString &remoteDir, CUbuntuOneService *service, QWidget *parent = 0);
    ~CUploadDialog();

protected:
    void closeEvent(QCloseEvent *evnt);

private slots:
    void doAddRow(const QString &path, bool isDir);
    void doUpload();
    void onImportFinished();

signals:
    void addRow(const QString &path, bool isDir);
    void importFinished();

private:
    void doImport(const QString &str);
    QStringList findDirectories(const QString &str);
    
private:
    Ui::CUploadDialog *ui;
    QString m_localDir;
    QString m_remoteDir;
    bool m_abort;
    CUbuntuOneService *m_service;
};

#endif // CUPLOADDIALOG_H
