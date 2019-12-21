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

    QDataStream &reader();
    QDataStream &writer();
    MsgHeader msgHeader();
    MsgTime msgTime();
    int msgType();
    MsgSize msgSize();
    QByteArray msgBody();
    QDataStream &msgBodyReader();
    void connectToHost(const QHostAddress &address, quint16 port);

    void send(int msgType, QByteArray data);
    QDataStream &send(int msgType, MsgSize size);

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
};

#endif // BLACKBOARDCLIENT_H
