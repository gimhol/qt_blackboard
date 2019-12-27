#include "BlackboardServer.h"

BlackboardServer::BlackboardServer(QObject *parent):
    QObject(parent),
    _server(new QTcpServer(this))
{
    _server->connect(_server,&QTcpServer::newConnection,this,&BlackboardServer::onNewConnection);
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
    for(auto client: _clients){
        client->deleteLater();
    }
    _clients.clear();
    _server->close();
}

void BlackboardServer::onNewConnection()
{
    auto client = new BlackboardClient(_server->nextPendingConnection(),this);
    connect(client,&BlackboardClient::msgRead,this,&BlackboardServer::onClientMsgRead);
    connect(client,&BlackboardClient::disconnected,this,&BlackboardServer::onClientDisconnected);
    _clients << client;
}

void BlackboardServer::onClientMsgRead()
{
    auto client = qobject_cast<BlackboardClient*>(sender());
    for(auto other: _clients){
        if(client != other){
            other->send(client->msgType(),
                        client->msgBody());
        }
    }
}

void BlackboardServer::onClientDisconnected()
{
    auto client = qobject_cast<BlackboardClient*>(sender());
    _clients.removeOne(client);
    client->deleteLater();
}
