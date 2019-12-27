#include "BlackboardServer.h"

BlackboardServer::BlackboardServer(QObject *parent):
    QObject(parent),
    _server(new QTcpServer(this))
{
    _server->listen(QHostAddress::Any,9527);
    _server->connect(_server,&QTcpServer::newConnection,this,&BlackboardServer::onNewConnection);
}

void BlackboardServer::onNewConnection()
{
    auto client = new BlackboardClient(_server->nextPendingConnection(),this);
    client->setObjectName("client");
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
