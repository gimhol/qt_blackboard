#include "BbFactory.h"
#include "BbHelper.h"
#include "BbScene.h"

#include <QDateTime>

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

QString BbFactory::makeItemId()
{
    ++_itemIdCount;
    return QString("item_%1").arg(
                QDateTime::currentMSecsSinceEpoch()+_itemIdCount);
}

QString BbFactory::makeBackgroundId()
{
    ++_bbIdCount;
    return QString("bb_%1").arg(
                QDateTime::currentMSecsSinceEpoch()+_bbIdCount);
}

qreal BbFactory::makeItemZ()
{
    ++_zCount;
    return QDateTime::currentMSecsSinceEpoch() + _zCount;
}
