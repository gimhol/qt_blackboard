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

QJsonObject BbItemStraightData::privateData()
{
    QJsonObject jdata;
    jdata["a_x"] = a.x();
    jdata["a_y"] = a.y();
    jdata["b_x"] = b.x();
    jdata["b_y"] = b.y();
    return jdata;
}

void BbItemStraightData::readPrivateData(const QJsonObject &jdata)
{
    a.setX(jdata["a_x"].toDouble());
    a.setY(jdata["a_y"].toDouble());
    b.setX(jdata["b_x"].toDouble());
    b.setY(jdata["b_y"].toDouble());
    empty = false;
}
