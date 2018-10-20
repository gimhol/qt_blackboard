#ifndef IITEMINDEX_H
#define IITEMINDEX_H

#include <QString>
#include "BbToolType.h"
class BlackboardScene;
class IItemIndex
{
public:
    virtual ~IItemIndex(){}
    virtual QString id() const = 0;
    virtual void setId(const QString &id) = 0;
    virtual BbToolType toolType() const = 0;
    virtual BlackboardScene *scene() = 0;
};

#endif // IITEMINDEX_H
