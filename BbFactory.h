#ifndef BBFACTORY_H
#define BBFACTORY_H

#include <BbToolType.h>
class BbScene;
class IItemIndex;
class BbItemData;
class Blackboard;
class BbFactory
{
public:
    BbFactory();
    virtual ~BbFactory(){}
    virtual BbScene *createScene(Blackboard *blackboard);
    virtual IItemIndex *createItemWhenToolDown(BbToolType bbtt);
    virtual IItemIndex *createItem(BbToolType bbtt);
    virtual IItemIndex *createItem(BbItemData *data);
    virtual BbItemData *createToolSettings(BbToolType bbtt);
};

#endif // BBFACTORY_H
