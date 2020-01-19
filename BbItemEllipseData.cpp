#include "BbItemEllipseData.h"

static QColor defaultPenColor = QColor(100,100,180);

static qreal minPenWidth = 1.1;

static qreal maxPenWidth = 30;

static QPen defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

static QColor defaultBrushColor = Qt::transparent;

static QBrush defaultBrush = defaultBrushColor;

const QColor &BbItemEllipseData::getDefaultPenColor()
{
    return defaultPenColor;
}

const qreal &BbItemEllipseData::getMinPenWidth()
{
    return minPenWidth;
}

const qreal &BbItemEllipseData::getMaxPenWidth()
{
    return maxPenWidth;
}

const QPen &BbItemEllipseData::getDefaultPen()
{
    return defaultPen;
}

const QColor &BbItemEllipseData::getDefaultBrushColor()
{
    return defaultBrushColor;
}

const QBrush &BbItemEllipseData::getDefaultBrush()
{
    return defaultBrush;
}

void BbItemEllipseData::setDefaultPenColor(const QColor &value)
{
    defaultPenColor = value;
}

void BbItemEllipseData::setMinPenWidth(const qreal &value)
{
    minPenWidth = value;
}

void BbItemEllipseData::setMaxPenWidth(const qreal &value)
{
    maxPenWidth = value;
}

void BbItemEllipseData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

void BbItemEllipseData::setDefaultBrushColor(const QColor &value)
{
    defaultBrushColor = value;
}

void BbItemEllipseData::setDefaultBrush(const QBrush &value)
{
    defaultBrush = value;
}
BbItemEllipseData::BbItemEllipseData(CoordMode mode):
    BbItemData(mode),
    pen(defaultPen),
    brush(defaultBrush)
{
    tooltype = BBTT_Ellipse;
}

qreal BbItemEllipseData::weight()
{
    return (pen.widthF() - minPenWidth)/(maxPenWidth - minPenWidth);
}

void BbItemEllipseData::setWeight(qreal weight)
{
    pen.setWidthF(minPenWidth + weight * (maxPenWidth - minPenWidth));
}

void BbItemEllipseData::writeStream(QDataStream &stream)
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

void BbItemEllipseData::readStream(QDataStream &stream)
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
