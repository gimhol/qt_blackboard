#ifndef BLACKBOARDCLIENT_H
#define BLACKBOARDCLIENT_H

#include <QObject>
#include <QPointer>
#include <QTcpSocket>

#include "BlackboardNetwork.h"

class BlackboardClient: public QObject
{
    Q_OBJECT
public:
    enum ReadStep{
        ReadStepInvaild = 0,
        ReadStepMsgHeader,
        ReadStepMsgTime,
        ReadStepMsgType,
        ReadStepMsgSize,
        ReadStepMsgBody
    };

    explicit BlackboardClient(QObject *parent = nullptr);
    explicit BlackboardClient(QTcpSocket *socket, QObject *parent = nullptr);

    ~BlackboardClient();

    QPointer<QTcpSocket> socket();
    QDataStream &reader();
    QDataStream &writer();
    MsgHeader msgHeader();
    MsgTime msgTime();
    int msgType();
    MsgSize msgSize();
    QByteArray msgBody();
    QDataStream &msgBodyReader();
    bool isConnected();
    void connectToHost(const QString &hostName, quint16 port);
    void disconnectFromHost();
    void send(int msgType, QByteArray data);

signals:
    void wrongMsgHeader();
    void msgRead();
    void connected();
    void disconnected();
protected:
    void init();
    void onSocketReadyRead();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);

    QPointer<QTcpSocket> _socket;
    QDataStream *_reader = nullptr;
    QDataStream *_writer = nullptr;

    ReadStep _readStep = ReadStepMsgHeader;
    MsgHeader _readMsgHeader = MsgHeaderInvaild;
    MsgTime _readMsgTime;
    int _readMsgType = MsgTypeInvaild;
    MsgSize _readMsgSize = 0;
    QByteArray *_readMsgBody = nullptr;
    QDataStream *_msgBodyReader = nullptr;
    bool _connected = false;
};

#endif // BLACKBOARDCLIENT_H
