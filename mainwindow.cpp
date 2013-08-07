#include <QtCore>
#include <QtWidgets>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "cubuntuonetask.h"
#include "cuploaddialog.h"
#include "cglobals.h"
#include "ctreemodel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_service = new CUbuntuOneService(this);
    connect(m_service, &CUbuntuOneService::loginFailed, this, &MainWindow::onLoginFailed);
    connect(m_service, &CUbuntuOneService::loginSucceed, this, &MainWindow::onLoginSucceed);
    //connect(m_service, &CUbuntuOneService::error, this, &MainWindow::showError);
    //connect(m_service, &CUbuntuOneService::message, this, &MainWindow::showMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoginFailed()
{
    QMessageBox::critical(this, QString::fromUtf8("Ошибка"),
                          QString::fromUtf8("Невозможно войти в аккаунт, попробуйте снова"));
}

void MainWindow::onLoginSucceed()
{
    ui->connectButton->setEnabled(false);
    ui->disconnectButton->setEnabled(true);

    ui->statusBar->showMessage("Подключено");

    //ui->treeView->setModel(new CTreeModel(this));
    m_model = new CTreeModel(this);

    //connect(ui->treeView, &QTreeView::expanded, this, &MainWindow::on_tableView_activated);
    //connect(ui->treeView, &QTreeView::collapsed, m_model, &CTreeModel::collapsed);

    //ui->treeView->setModel(m_model);
    ui->tableView->setModel(m_model);
}

void MainWindow::on_disconnectButton_clicked()
{
    m_service->unlink();
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);

    ui->statusBar->showMessage("Отключено");
}

void MainWindow::on_connectButton_clicked()
{
    m_service->link();
}

void MainWindow::on_pushButton_clicked()
{
    ui->treeView->reset();

    ui->tableView->setRootIndex(m_model->index(0, 0));
    ui->tableView->reset();
}

void MainWindow::on_tableView_entered(const QModelIndex &index)
{

}

void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    DEBUG << "on_tableView_activated";

    m_model->expanded(index);
    ui->tableView->reset();
    ui->tableView->setRootIndex(index);
}

void MainWindow::on_pushButton_2_clicked()
{
    //m_service->createVolume("testvol");
    //m_service->createFolder("testfolder1/saas/fgasg/asg/a/sg/asg/asg/as/g");
    //m_service->uploadFile("/home/vooft/cals.sh", "/content/~/testvol/cals.sh");
    DEBUG << m_model->fullPath(ui->tableView->rootIndex());
}

void MainWindow::on_createVolumeButton_clicked()
{
    QString volumeName = QInputDialog::getText(this, QString::fromUtf8("Введите название"), QString::fromUtf8("Том:"));
    if(volumeName.isEmpty())
        return;

    m_service->createVolume(volumeName);
}

void MainWindow::showMessage(const QString &str)
{
    QMessageBox::information(this, QString::fromUtf8("Ошибка"), str);
}

void MainWindow::showError(const QString &str)
{
    QMessageBox::critical(this, QString::fromUtf8("Ошибка"), str);
}

void MainWindow::on_uploadFileButton_clicked()
{
    /*m_service->uploadFile("/home/vooft/cals.sh",
                          QString("/content").append(m_model->fullPath(ui->tableView->rootIndex())).append("/cals.sh"));*/
    CUbuntuOneTask task(m_service, this);
    bool result = task.createDirectory("/~/testvol/testdir");
    DEBUG << "created:" << result;
    DEBUG << task.errorString();
}

void MainWindow::on_uploadDirButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromUtf8("Выберите директорию для загрузки"));
    if(dir.isEmpty())
        return;

    CUploadDialog dialog(dir, m_model->fullPath(ui->tableView->rootIndex()), m_service, this);
    dialog.exec();
}
