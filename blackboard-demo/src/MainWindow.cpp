#include "BlackboardServer.h"
#include "BlackboardServerWindow.h"
#include "BlackboardClientWindow.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startBlackboardServer_clicked()
{
    auto win0 = new BlackboardServerWindow();
    win0->setAttribute(Qt::WA_DeleteOnClose);
    win0->setWindowTitle(QStringLiteral(u"黑板服务端窗口_%1").arg(QDateTime::currentDateTime().toString()));
    win0->show();
}

void MainWindow::on_startBlackboardClient_clicked()
{
    auto win0 = new BlackboardClientWindow();
    win0->setAttribute(Qt::WA_DeleteOnClose);
    win0->setWindowTitle(QStringLiteral("黑板客户端窗口_%1").arg(QDateTime::currentDateTime().toString()));
    win0->show();
}
