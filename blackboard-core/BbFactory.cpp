#include "BbFactory.h"
#include "BbHelper.h"
#include "BbScene.h"

#include <QDateTime>

Q_GLOBAL_STATIC(BbFactory,bbFactory)

BbFactory *BbFactory::get()
{
    return bbFactory();
}

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

IItemIndex *BbFactory::createItem(const QJsonObject &jobj)
{
    auto type = jobj["type"].toInt();
    auto index = createItem(BbToolType(type));
    Q_ASSERT(index);

    if(!index)
        return nullptr;

    auto reader = dynamic_cast<IJsonWR*>(index);
    Q_ASSERT(reader);
    if(reader)
        reader->fromJsonObject(jobj);

    return index;
}

BbItemData *BbFactory::createToolSettings(BbToolType bbtt)
{
    return BbHelper::createToolSettings(bbtt);
}

QString BbFactory::makeItemId(BbToolType bbtt)
{
    ++_itemIdCount;
    auto toolTypeName = BbHelper::toolTypeName(bbtt);
    auto msec = QDateTime::currentMSecsSinceEpoch();
    return QString("%1_%2_%3").arg(toolTypeName).arg(msec).arg(_itemIdCount);
}

QString BbFactory::makeBackgroundId()
{
    ++_bbIdCount;
    auto msec = QDateTime::currentMSecsSinceEpoch();
    return QString("bb_%1_%2").arg(msec).arg(_bbIdCount);
}

qreal BbFactory::makeItemZ(BbToolType bbtt)
{
    auto z = QDateTime::currentMSecsSinceEpoch();
    if(z > _prevZ){
        _zCount = 0;
    }else{
        ++_zCount;
        z += _zCount;
    }
    _prevZ = z;
    return z;
}

BbItemData *BbFactory::createItemData(BbToolType bbtt){
    return BbHelper::createItemData(bbtt);
}

QString BbFactory::makeGroupId()
{
    ++_bbIdCount;
    auto msec = QDateTime::currentMSecsSinceEpoch();
    return QString("group_%1_%2").arg(msec).arg(_bbIdCount);
}
