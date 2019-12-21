#ifndef BLACKBOARDNETWORK_H
#define BLACKBOARDNETWORK_H
#include <QDateTime>

typedef qint64 MsgTime;
typedef int MsgSize;
enum MsgHeader{
    MsgHeaderInvaild = 0,
    MsgHeaderDefault = -1010101010,
};
enum MsgType{
    MsgTypeInvaild = 0,
    MsgTypeBlackboardScrolled = -1,
    MsgTypeBlackboardPointerShown = -2,
    MsgTypeBlackboardPointerMoving = -3,
    MsgTypeBlackboardPointerMoved = -4,
    MsgTypeBlackboardPointerHidden = -5,
};
#endif // BLACKBOARDNETWORK_H
