#include "BbItemData.h"
#include "IItemIndex.h"
#include "BbScene.h"

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
        auto scene = itemIdx->bbScene();
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
            updatePrevPostion();
        }
    }
}

void BbItemData::updatePrevPostion()
{
    prevX = x;
    prevY = y;
}

void BbItemData::writeStream(QDataStream &stream){
    QJsonObject jobj;
    jobj["coord_mode"] = mode;
    jobj["id"] = lid;
    jobj["x"] = x;
    jobj["y"] = y;
    jobj["z"] = z;
    jobj["prev_x"] = prevX;
    jobj["prev_y"] = prevY;
    jobj["prev_z"] = prevZ;
    stream << QJsonDocument(jobj).toBinaryData();
}

void BbItemData::readStream(QDataStream &stream){
    QByteArray data;
    stream >> data;
    auto jobj = QJsonDocument::fromBinaryData(data).object();
    mode  = CoordMode(jobj["coord_mode"].toInt());
    lid   = jobj["id"].toString();
    x     = jobj["x"].toDouble();
    y     = jobj["y"].toDouble();
    z     = jobj["z"].toDouble();
    prevX = jobj["prev_x"].toDouble();
    prevY = jobj["prev_y"].toDouble();
    prevZ = jobj["prev_z"].toDouble();
}
