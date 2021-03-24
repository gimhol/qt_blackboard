#include "BlackboardServerWindow.h"
#include "ui_BlackboardServerWindow.h"

#include <QJsonDocument>

BlackboardServerWindow::BlackboardServerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlackboardServerWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->btnToggle->setText("start");
    _server = new BlackboardServer(this);

    connect(_server,&BlackboardServer::clientConnected,this,[&](BlackboardClient *client){
        ui->textBrowser->append("client connected.");
    });
    connect(_server,&BlackboardServer::clientDisconnected,this,[&](BlackboardClient *client){
        ui->textBrowser->append("client disconnected.");
    });
    connect(_server,&BlackboardServer::msgRead,this,[&](int type, QByteArray data){
        ui->textBrowser->append(QString("msg: [%1]%2").arg(type).arg(QString::fromUtf8(QJsonDocument::fromBinaryData(data).toJson())));
    });

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
