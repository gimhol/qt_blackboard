#include "BlackboardServer.h"
#include "BlackboardServerWindow.h"
#include "BlackboardClientWindow.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QLabel>
#include <QOpenGLWidget>

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

    auto bb = win0->blackboard();
    bb->setBackgroundBrush(QColor::fromRgb(0x555555));
    win0->setAttribute(Qt::WA_DeleteOnClose);
    win0->setWindowTitle(QStringLiteral("黑板客户端窗口_%1").arg(QDateTime::currentDateTime().toString()));
    win0->show();
}

void MainWindow::on_startBlackboardClientOpenGL_clicked()
{
    auto win0 = new BlackboardClientWindow();

    auto vp = new QOpenGLWidget;
    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setSamples(4); //多重采样
    format.setProfile(QSurfaceFormat::CoreProfile);
    vp->setFormat(format);
    vp->setMouseTracking(true);
    vp->setTabletTracking(true);

    auto bb = win0->blackboard();
    // Note: 为viewPort为QOpenGLWidget的QGraphicsView设置styleSheet将导致渲染问题。 -Gim
    bb->setBackgroundBrush(QColor::fromRgb(0x555555));
    bb->setViewport(vp);

    win0->setAttribute(Qt::WA_DeleteOnClose);
    win0->setWindowTitle(QStringLiteral("黑板客户端窗口_%1").arg(QDateTime::currentDateTime().toString()));
    win0->show();
}
