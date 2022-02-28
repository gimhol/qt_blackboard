#include "BbItemPenData.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


static qreal minWidth = 1.1;
static qreal maxWidth = 30;
static QPen defaultPen = QPen(QColor(100,100,180), minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
static bool defaultCubic = true;
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

const bool &BbItemPenData::getDefaultCubic()
{
    return defaultCubic;
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
void BbItemPenData::setDefaultCubic(const bool &value)
{
    defaultCubic = value;
}

BbItemPenData::BbItemPenData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Pen;
    needPen = true;
    pen = defaultPen;
    cubic = defaultCubic;
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

QJsonObject BbItemPenData::privateData()
{
    QJsonObject jdata;
    QJsonArray jarrs;
    for(auto coord: coords)
        jarrs << coord;
    jdata["coords"] = jarrs;
    jdata["cubic"] = cubic;
    return jdata;
}

void BbItemPenData::readPrivateData(const QJsonObject &jdata)
{
    for(auto jval: jdata["coords"].toArray())
        coords.append(jval.toDouble());
    cubic = jdata["cubic"].toBool(false);
    empty = coords.isEmpty();
}
