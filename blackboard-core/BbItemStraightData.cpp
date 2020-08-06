#include "BbItemStraightData.h"

static qreal minWidth = 1.1;
static qreal maxWidth = 30;
static QPen defaultPen = QPen(QColor(100,100,180), minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

const qreal &BbItemStraightData::getMinWidth()
{
    return minWidth;
}
const qreal &BbItemStraightData::getMaxWidth()
{
    return maxWidth;
}
const QPen &BbItemStraightData::getDefaultPen()
{
    return defaultPen;
}
void BbItemStraightData::setMinWidth(const qreal &value)
{
    minWidth = value;
}
void BbItemStraightData::setMaxWidth(const qreal &value)
{
    maxWidth = value;
}

void BbItemStraightData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

BbItemStraightData::BbItemStraightData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Straight;
    needPen = true;
    pen = defaultPen;
}

void BbItemStraightData::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}

qreal BbItemStraightData::weight()
{
    return (pen.widthF() - minWidth) / (maxWidth - minWidth);
}

QJsonObject BbItemStraightData::toJsonObject()
{
    auto jobj = BbItemData::toJsonObject();
    jobj["a_x"] = a.x();
    jobj["a_y"] = a.y();
    jobj["b_x"] = b.x();
    jobj["b_y"] = b.y();
    return jobj;
}

void BbItemStraightData::fromJsonObject(const QJsonObject &jobj)
{
    BbItemData::fromJsonObject(jobj);
    a.setX(jobj["a_x"].toDouble());
    a.setY(jobj["a_y"].toDouble());
    b.setX(jobj["b_x"].toDouble());
    b.setY(jobj["b_y"].toDouble());
    empty = false;
}
