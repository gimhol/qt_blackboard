#include "BbItemRectData.h"

static QColor defaultPenColor = QColor(100,100,180);

static qreal minPenWidth = 1.1;

static qreal maxPenWidth = 30;

static QPen defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

static QColor defaultBrushColor = Qt::transparent;

static QBrush defaultBrush = defaultBrushColor;

const QColor &BbItemRectData::getDefaultPenColor()
{
    return defaultPenColor;
}

const qreal &BbItemRectData::getMinPenWidth()
{
    return minPenWidth;
}

const qreal &BbItemRectData::getMaxPenWidth()
{
    return maxPenWidth;
}

const QPen &BbItemRectData::getDefaultPen()
{
    return defaultPen;
}

const QColor &BbItemRectData::getDefaultBrushColor()
{
    return defaultBrushColor;
}

const QBrush &BbItemRectData::getDefaultBrush()
{
    return defaultBrush;
}

void BbItemRectData::setDefaultPenColor(const QColor &value)
{
    defaultPenColor = value;
}

void BbItemRectData::setMinPenWidth(const qreal &value)
{
    minPenWidth = value;
}

void BbItemRectData::setMaxPenWidth(const qreal &value)
{
    maxPenWidth = value;
}

void BbItemRectData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

void BbItemRectData::setDefaultBrushColor(const QColor &value)
{
    defaultBrushColor = value;
}

void BbItemRectData::setDefaultBrush(const QBrush &value)
{
    defaultBrush = value;
}

BbItemRectData::BbItemRectData(CoordMode mode):
    BbItemData(mode),
    pen(defaultPen),
    brush(defaultBrush)
{
    tooltype = BBTT_Rectangle;
}

qreal BbItemRectData::weight()
{
    return (pen.widthF() - minPenWidth)/(maxPenWidth - minPenWidth);
}

void BbItemRectData::setWeight(qreal weight)
{
    pen.setWidthF(minPenWidth + weight * (maxPenWidth - minPenWidth));
}

void BbItemRectData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);
    QJsonObject jobj;
    jobj["pen_width"]   = pen.widthF();
    jobj["pen_color"]   = int(pen.color().rgba());
    jobj["pen_style"]   = int(pen.style());
    jobj["brush_color"] = int(brush.color().rgba());
    jobj["brush_style"] = int(brush.style());
    jobj["width"]       = size.width();
    jobj["height"]      = size.height();
    stream << QJsonDocument(jobj).toBinaryData();
}

void BbItemRectData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    QByteArray data;
    stream >> data;
    auto jobj = QJsonDocument::fromBinaryData(data).object();
    pen.setWidthF(jobj["pen_width"].toDouble());
    pen.setColor(QColor::fromRgba(QRgb(jobj["pen_color"].toInt())));
    pen.setStyle(Qt::PenStyle(jobj["pen_style"].toInt()));
    brush.setColor(QColor::fromRgba(QRgb(jobj["brush_color"].toInt())));
    brush.setStyle(Qt::BrushStyle(jobj["brush_style"].toInt()));
    size.setWidth(jobj["width"].toDouble());
    size.setHeight(jobj["height"].toDouble());
    empty = size.isNull();
}
