#ifndef BLACKBOARDSERVER_H
#define BLACKBOARDSERVER_H

#include <QList>
#include <QObject>
#include <QPointer>
#include <QTcpServer>
#include "BlackboardClient.h"

class BlackboardServer: public QObject
{
    Q_OBJECT
public:
    explicit BlackboardServer(QObject *parent = nullptr);
protected:
    void onNewConnection();
    void onClientMsgRead();
    void onClientDisconnected();
    QPointer<QTcpServer> _server;
    QList<QPointer<BlackboardClient>> _clients;
};

#endif // BLACKBOARDSERVER_H
