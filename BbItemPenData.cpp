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
    BbItemData(mode),
    pen(defaultPen)
{
    tooltype = BBTT_Pen;
}

void BbItemPenData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);

    QJsonObject jobj;
    jobj["pen_width"] = pen.widthF();
    jobj["pen_color"] = int(pen.color().rgba());
    jobj["pen_style"] = int(pen.style());
    QJsonArray jarrs;
    for(auto coord: coords)
        jarrs << coord;
    jobj["coords"] = jarrs;

    stream << QJsonDocument(jobj).toBinaryData();
}

void BbItemPenData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    QByteArray data;
    stream >> data;
    auto jobj = QJsonDocument::fromBinaryData(data).object();
    pen.setWidthF(jobj["pen_width"].toDouble());
    pen.setStyle(Qt::PenStyle(jobj["pen_style"].toInt()));
    pen.setColor(QColor::fromRgba(QRgb(jobj["pen_color"].toInt())));
    for(auto jval: jobj["coords"].toArray())
        coords.append(jval.toDouble());
    empty = coords.isEmpty();
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
