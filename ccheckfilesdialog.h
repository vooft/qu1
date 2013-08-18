#ifndef CCHECKFILESDIALOG_H
#define CCHECKFILESDIALOG_H

#include <QtWidgets>

#include "cubuntuoneservice.h"

namespace Ui {
class CCheckFilesDialog;
}

struct SU1StorageInfo {
    QStringList dirs;
    QHash<QString, QString> files; // files and it's hashes
    QHash<QString, QString> relFiles;
};

class CCheckFilesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CCheckFilesDialog(const QString &localPath, const QString &remotePath, const QString &volume,
                               CUbuntuOneService *service, QWidget *parent = 0);
    ~CCheckFilesDialog();
    
private slots:
    void on_scanLocalButton_clicked();
    void on_scanRemoteButton_clicked();

    void doAddRow(QTableWidget *table, const QString &path, const QString &root, bool isDir, const QString &hash);
    //void doUpload();
    void onImportFinished();

    void on_hashButton_clicked();

    void on_createLocalRelButton_clicked();

    void on_createRemoteRelButton_clicked();

    void on_compareButton_clicked();

    void on_pushButton_clicked();

private:
    void fillLocalStorage(const QString &str);
    void fillRemoteStorage(const QString &root, SU1StorageInfo *info, QTableWidget *table);
    QStringList findLocalDirectories(const QString &str);
    QStringList parseJsonForDirectories(const QByteArray &json) const;
    QHash<QString, QString> parseJsonForFiles(const QByteArray &json) const;
    QHash<QString, QString> createRelative(QTableWidget *table);

signals:
    void addRow(QTableWidget *table, const QString &path, const QString &root, bool isDir, const QString &hash);
    void importFinished();

private:
    Ui::CCheckFilesDialog *ui;
    QString m_localPath;
    QString m_remotePath;
    CUbuntuOneService *m_service;
    bool m_abort;
    QString m_volume;
    SU1StorageInfo m_localStorage;
    SU1StorageInfo m_remoteStorage;
};

#endif // CCHECKFILESDIALOG_H
