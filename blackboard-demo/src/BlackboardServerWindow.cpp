#include "BlackboardServerWindow.h"
#include "ui_BlackboardServerWindow.h"

BlackboardServerWindow::BlackboardServerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlackboardServerWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->btnToggle->setText("start");
    _server = new BlackboardServer(this);
}

BlackboardServerWindow::~BlackboardServerWindow()
{
    delete ui;
}

void BlackboardServerWindow::on_btnToggle_clicked()
{
    if(_server->isRunning()){
        ui->btnToggle->setText("start");
        _server->stop();
    }
    else{
        ui->btnToggle->setText("stop");
        _server->start(ui->port->text().toInt());
    }
}
