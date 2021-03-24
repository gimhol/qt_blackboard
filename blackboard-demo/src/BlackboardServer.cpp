#include "BlackboardServer.h"
#include <QDebug>
BlackboardServer::BlackboardServer(QObject *parent):
    QObject(parent),
    _server(new QTcpServer(this))
{
    connect(_server,&QTcpServer::newConnection,this,&BlackboardServer::onNewConnection);
    connect(_server,&QTcpServer::destroyed,[&](){
        qDebug() << "QTcpServer destroyed";
    });
    connect(_server,&QTcpServer::newConnection,[&](){
        qDebug() << "QTcpServer onNewConnection";
    });
    connect(_server,&QTcpServer::acceptError,[&](QAbstractSocket::SocketError socketError){
        qDebug() << "QTcpServer acceptError" << socketError;
    });
}

BlackboardServer::~BlackboardServer()
{
    stop();
}

bool BlackboardServer::isRunning()
{
    return _server->isListening();
}

void BlackboardServer::start(int port)
{
    _server->listen(QHostAddress::Any,port);
}

void BlackboardServer::stop()
{
    if(_server->isListening())
        _server->close();

    for(auto client: _clients){
        client->disconnectFromHost();
        client->deleteLater();
    }
    _clients.clear();
}

void BlackboardServer::onNewConnection()
{
    auto client = new BlackboardClient(_server->nextPendingConnection(),this);
    connect(client,&BlackboardClient::msgRead,this,&BlackboardServer::onClientMsgRead);
    connect(client,&BlackboardClient::disconnected,this,&BlackboardServer::onClientDisconnected);
    _clients << client;
    emit clientConnected(client);
}

void BlackboardServer::onClientMsgRead()
{
    auto client = qobject_cast<BlackboardClient*>(sender());
    auto type = client->msgType();
    auto body = client->msgBody();
    emit msgRead(type, body);
    for(auto other: _clients) {
        if(client != other) {
            other->send(type, body);
        }
    }

}

void BlackboardServer::onClientDisconnected()
{
    auto client = qobject_cast<BlackboardClient*>(sender());
    emit clientDisconnected(client);
    _clients.removeOne(client);
    client->deleteLater();
}
