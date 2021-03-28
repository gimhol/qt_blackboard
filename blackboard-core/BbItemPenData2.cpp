#include "BbItemPenData2.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


static qreal minWidth = 1.1;
static qreal maxWidth = 30;
static QPen defaultPen = QPen(QColor(100,100,180), minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
static bool defaultCubic = true;
const qreal & BbItemPenData2::getMinWidth()
{
    return minWidth;
}
const qreal & BbItemPenData2::getMaxWidth()
{
    return maxWidth;
}
const QPen & BbItemPenData2::getDefaultPen()
{
    return defaultPen;
}

const bool &BbItemPenData2::getDefaultCubic()
{
    return defaultCubic;
}
void BbItemPenData2::setMinWidth(const qreal &value)
{
    minWidth = value;
}
void BbItemPenData2::setMaxWidth(const qreal &value)
{
    maxWidth = value;
}

void BbItemPenData2::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}
void BbItemPenData2::setDefaultCubic(const bool &value)
{
    defaultCubic = value;
}

BbItemPenData2::BbItemPenData2(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Pen;
    needPen = true;
    pen = defaultPen;
    cubic = defaultCubic;
}

void BbItemPenData2::setColor(const QColor &color)
{
    pen.setColor(color);
}

void BbItemPenData2::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}

qreal BbItemPenData2::weight()
{
    return (pen.widthF() - minWidth) / (maxWidth - minWidth);
}

QJsonObject BbItemPenData2::toJsonObject()
{
    auto jobj = BbItemData::toJsonObject();
    QJsonArray jarrs;
    for(auto coord: coords)
        jarrs << coord;
    jobj["coords"] = jarrs;
    jobj["cubic"] = cubic;
    return jobj;
}

void BbItemPenData2::fromJsonObject(const QJsonObject &jobj)
{
    BbItemData::fromJsonObject(jobj);
    for(auto jval: jobj["coords"].toArray())
        coords.append(jval.toDouble());
    cubic = jobj["cubic"].toBool(false);
    empty = coords.isEmpty();
}
