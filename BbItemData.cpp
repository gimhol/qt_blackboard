#include "BbItemData.h"
#include "IItemIndex.h"
#include "BlackboardScene.h"

BbItemData::BbItemData(CoordMode mode):
    mode(mode)
{

}

BbItemData::~BbItemData()
{

}

bool BbItemData::isPositionValid()
{
    return x > -9998 && y > -9998;
}

bool BbItemData::isPrevPositionValid()
{
    return prevX > -9998 && prevY > -9998;
}

void BbItemData::updatePostion(IItemIndex *itemIdx)
{
    auto itemBase = dynamic_cast<QGraphicsItem*>(itemIdx);
    if(itemBase)
    {
        auto scene = itemIdx->scene();
        if(scene && CM_PERCENTAGE == mode)
        {
            qreal ratio = scene->width() / 100;
            x = itemBase->x() / ratio;
            y = itemBase->y() / ratio;
        }
        else
        {
            x = itemBase->x();
            y = itemBase->y();
        }
        if(!isPrevPositionValid())
        {
            prevX = x;
            prevY = y;
        }
    }
}

void BbItemData::writeStream(QDataStream &stream){
    stream << static_cast<int>(mode) << lid << x << y << z << prevX << prevY << prevZ;
}

void BbItemData::readStream(QDataStream &stream){
    int imode;
    stream >> imode >> lid >> x >> y >> z >> prevX >> prevY >> prevZ;
    mode = static_cast<CoordMode>(imode);
}
