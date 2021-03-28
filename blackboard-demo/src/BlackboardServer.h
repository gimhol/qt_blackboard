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
    ~BlackboardServer();
    bool isRunning();
    void start(int port);
    void stop();
signals:
    void clientConnected(BlackboardClient *client);
    void msgRead(int type, QByteArray data);
    void clientDisconnected(BlackboardClient *client);
protected:
    void onNewConnection();
    void onClientMsgRead();
    void onClientDisconnected();
    QPointer<QTcpServer> _server;
    QList<QPointer<BlackboardClient>> _clients;
};

#endif // BLACKBOARDSERVER_H
