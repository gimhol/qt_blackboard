#include "BlackboardClient.h"
#include <QDataStream>

BlackboardClient::BlackboardClient(QObject *parent):
    QObject(parent),
    _socket(new QTcpSocket(this)),
    _reader(new QDataStream(_socket)),
    _writer(new QDataStream(_socket)),
    _readMsgBody(new QByteArray),
    _msgBodyReader(new QDataStream(_readMsgBody,QIODevice::ReadOnly))
{
    init();
}

BlackboardClient::BlackboardClient(QTcpSocket *socket, QObject *parent):
    QObject(parent),
    _socket(socket),
    _reader(new QDataStream(_socket)),
    _writer(new QDataStream(_socket)),
    _readMsgBody(new QByteArray),
    _msgBodyReader(new QDataStream(_readMsgBody,QIODevice::ReadOnly))
{
    init();
}

BlackboardClient::~BlackboardClient(){
    if(_reader)
        delete _reader;
    if(_writer)
        delete _writer;
    if(_readMsgBody)
        delete _readMsgBody;
}

QDataStream &BlackboardClient::reader()
{
    return *_reader;
}

QDataStream &BlackboardClient::writer()
{
    return *_writer;
}

MsgHeader BlackboardClient::msgHeader(){ return _readMsgHeader;}

MsgTime BlackboardClient::msgTime(){ return _readMsgTime;}

int BlackboardClient::msgType(){ return _readMsgType;}

MsgSize BlackboardClient::msgSize(){ return _readMsgSize;}

QByteArray BlackboardClient::msgBody(){ return *_readMsgBody;}

QDataStream &BlackboardClient::msgBodyReader(){ return *_msgBodyReader;}

void BlackboardClient::connectToHost(const QHostAddress &address, quint16 port)
{
    _socket->connectToHost(address,port);
}

void BlackboardClient::send(int msgType, QByteArray data){
    writer() << int(MsgHeaderDefault)
             << QDateTime::currentMSecsSinceEpoch()
             << msgType
             << data.size();
    writer().writeRawData(data.data(),data.size());
}

QDataStream &BlackboardClient::send(int msgType, MsgSize size)
{
    return writer() << int(MsgHeaderDefault)
             << QDateTime::currentMSecsSinceEpoch()
             << msgType
             << int(size);
}

void BlackboardClient::init()
{
    connect(_socket,&QAbstractSocket::readyRead,this,&BlackboardClient::onSocketReadyRead);
    connect(_socket,&QAbstractSocket::connected,this,&BlackboardClient::onSocketConnected);
    connect(_socket,&QAbstractSocket::disconnected,this,&BlackboardClient::onSocketDisconnected);
}
#define fallThrough [[clang::fallthrough]]
void BlackboardClient::onSocketReadyRead()
{
    while(1){
        switch(_readStep){
        case ReadStepMsgHeader:{
            if(_socket->bytesAvailable() < sizeof(MsgHeader))
                return;
            reader() >> reinterpret_cast<int&>(_readMsgHeader);
            if(_readMsgHeader != MsgHeaderDefault){
                emit wrongMsgHeader();
                break;
            }
            _readStep = ReadStepMsgTime;
            fallThrough;
        }
        case ReadStepMsgTime:{
            if(_socket->bytesAvailable() < sizeof(MsgTime))
                return;
            reader() >> _readMsgTime;
            _readStep = ReadStepMsgType;
            fallThrough;
        }
        case ReadStepMsgType:{
            if(_socket->bytesAvailable() < sizeof(MsgType))
                return;
            reader() >> reinterpret_cast<int&>(_readMsgType);
            _readStep = ReadStepMsgSize;
            fallThrough;
        }
        case ReadStepMsgSize:{
            if(_socket->bytesAvailable() < sizeof(MsgSize))
                return;
            reader() >> _readMsgSize;
            _readStep = ReadStepMsgBody;
            fallThrough;
        }
        case ReadStepMsgBody:{
            if(_readMsgSize > 0){
                if(_socket->bytesAvailable() < _readMsgSize)
                    return;
                _readMsgBody->clear();
                _readMsgBody->resize(_readMsgSize);
                reader().readRawData(_readMsgBody->data(),_readMsgSize);
            }else{
                _readMsgBody->clear();
            }
            _msgBodyReader->device()->seek(0);
            _readStep = ReadStepMsgHeader;
            emit msgRead();
            fallThrough;
        }
        default:
            break;
        }
    }

}

void BlackboardClient::onSocketConnected()
{
    emit connected();
}

void BlackboardClient::onSocketDisconnected()
{
    emit disconnected();
}
