#include "BbItemPenData.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


static qreal minWidth = 1.1;
static qreal maxWidth = 30;
static QPen defaultPen = QPen(QColor(100,100,180), minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

const qreal & BbItemPenData::getMinWidth()
{
    return minWidth;
}
const qreal & BbItemPenData::getMaxWidth()
{
    return maxWidth;
}
const QPen & BbItemPenData::getDefaultPen()
{
    return defaultPen;
}
void BbItemPenData::setMinWidth(const qreal &value)
{
    minWidth = value;
}
void BbItemPenData::setMaxWidth(const qreal &value)
{
    maxWidth = value;
}

void BbItemPenData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

BbItemPenData::BbItemPenData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Pen;
    needPen = true;
    pen = defaultPen;
}

void BbItemPenData::setColor(const QColor &color)
{
    pen.setColor(color);
}

void BbItemPenData::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}

qreal BbItemPenData::weight()
{
    return (pen.widthF() - minWidth) / (maxWidth - minWidth);
}

QJsonObject BbItemPenData::toJsonObject()
{
    auto jobj = BbItemData::toJsonObject();
    QJsonArray jarrs;
    for(auto coord: coords)
        jarrs << coord;
    jobj["coords"] = jarrs;
    return jobj;
}

void BbItemPenData::fromJsonObject(QJsonObject jobj)
{
    BbItemData::fromJsonObject(jobj);
    for(auto jval: jobj["coords"].toArray())
        coords.append(jval.toDouble());
    empty = coords.isEmpty();
}
