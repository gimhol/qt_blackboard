#ifndef IITEMINDEX_H
#define IITEMINDEX_H

#include "BlackboardHeader.h"
#include <QString>
#include "BbToolType.h"
class BlackboardScene;
class BbItemData;
class NSB_BLACKBOARD_EXPORT IItemIndex
{
public:
    virtual ~IItemIndex(){}
    virtual QString id() const = 0;
    virtual void setId(const QString &id) = 0;
    virtual BbToolType toolType() const = 0;
    virtual BlackboardScene *scene() = 0;
    virtual BbItemData *data() = 0;
    virtual void repaintWithItemData() = 0;
    template<class T> T *data() { return dynamic_cast<T*>(data()); }
    IItemIndex *next = nullptr;
};

#endif // IITEMINDEX_H
