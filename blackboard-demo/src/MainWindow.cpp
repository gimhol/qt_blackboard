#include "BlackboardServer.h"
#include "BlackboardServerWindow.h"
#include "BlackboardTestWindow.h"
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
    win0->setWindowTitle(QStringLiteral(u"黑板服务端窗口").arg(QDateTime::currentDateTime().toString()));
    win0->show();
}

void MainWindow::on_startBlackboardClient_clicked()
{
    auto win0 = new BlackboardTestWindow();
    win0->setAttribute(Qt::WA_DeleteOnClose);
    win0->setWindowTitle(QStringLiteral("黑板客户端窗口%1").arg(QDateTime::currentDateTime().toString()));
    win0->show();
}
