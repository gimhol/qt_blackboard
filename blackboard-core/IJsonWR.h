#ifndef IJSONWR_H
#define IJSONWR_H

#include "BbHeader.h"
#include <QJsonObject>

class NSB_BLACKBOARD_EXPORT IJsonW
{
public:
    virtual ~IJsonW(){}
    virtual QJsonObject toJsonObject() = 0;
};

class NSB_BLACKBOARD_EXPORT IJsonR
{
public:
    virtual ~IJsonR(){}
    virtual void fromJsonObject(const QJsonObject &jobj) = 0;
};

class NSB_BLACKBOARD_EXPORT IJsonWR: public IJsonW, public IJsonR
{
};

#endif // IJSONWR_H
