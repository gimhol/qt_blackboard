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
    BbItemData(mode),
    pen(defaultPen)
{
    tooltype = BBTT_Straight;
}

void BbItemStraightData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);

    QJsonObject jobj;
    jobj["pen_width"] = pen.widthF();
    jobj["pen_color"] = int(pen.color().rgba());
    jobj["pen_style"] = int(pen.style());
    jobj["a_x"] = a.x();
    jobj["a_y"] = a.y();
    jobj["b_x"] = b.x();
    jobj["b_y"] = b.y();
    stream << QJsonDocument(jobj).toBinaryData();
}

void BbItemStraightData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    QByteArray data;
    stream >> data;
    auto jobj = QJsonDocument::fromBinaryData(data).object();
    pen.setWidthF(jobj["pen_width"].toDouble());
    pen.setStyle(Qt::PenStyle(jobj["pen_style"].toInt()));
    pen.setColor(QColor::fromRgba(QRgb(jobj["pen_color"].toInt())));
    a.setX(jobj["a_x"].toDouble());
    a.setY(jobj["a_y"].toDouble());
    b.setX(jobj["b_x"].toDouble());
    b.setY(jobj["b_y"].toDouble());
    empty = false;
}

void BbItemStraightData::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}

qreal BbItemStraightData::weight()
{
    return (pen.widthF() - minWidth) / (maxWidth - minWidth);
}
