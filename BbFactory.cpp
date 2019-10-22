#include "BbFactory.h"
#include "BbHelper.h"
#include "BbScene.h"

BbFactory::BbFactory()
{

}

BbFactory::~BbFactory(){}

BbScene *BbFactory::createScene(Blackboard *blackboard)
{
    return new BbScene(blackboard);
}

IItemIndex *BbFactory::createItemWhenToolDown(BbToolType bbtt)
{
    return BbHelper::createItemWhenToolDown(bbtt);
}

IItemIndex *BbFactory::createItem(BbToolType bbtt)
{
    return BbHelper::createItem(bbtt);
}

IItemIndex *BbFactory::createItem(BbItemData *data)
{
    return BbHelper::createItem(data);
}

BbItemData *BbFactory::createToolSettings(BbToolType bbtt)
{
    return BbHelper::createToolSettings(bbtt);
}
