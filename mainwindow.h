#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ctreemodel.h"
#include "ctableproxymodel.h"
#include "cubuntuoneservice.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void onLoginFailed();
    void onLoginSucceed();

    void on_disconnectButton_clicked();

    void on_connectButton_clicked();

    void on_pushButton_clicked();

    void on_tableView_entered(const QModelIndex &index);

    void on_tableView_activated(const QModelIndex &index);

    void on_pushButton_2_clicked();

    void on_createVolumeButton_clicked();

    void showMessage(const QString &str);
    void showError(const QString &str);

    void on_uploadFileButton_clicked();

    void on_uploadDirButton_clicked();

    void on_checkButton_clicked();

private:
    Ui::MainWindow *ui;
    CUbuntuOneService *m_service;
    CTreeModel *m_model;
};

#endif // MAINWINDOW_H
