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

    stream << pen.widthF()
           << pen.color().rgba()
           << static_cast<short>(pen.style())
           << brush.color().rgba()
           << size.width()
           << size.height();
}

void BbItemRectData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    qreal penWidth;
    QRgb penRgba,brushRgba;
    short penStyle;
    qreal width,height;

    stream >> penWidth >> penRgba >> penStyle >> brushRgba >> width >> height;

    pen.setWidthF(penWidth);
    pen.setStyle(static_cast<Qt::PenStyle>(penStyle));
    pen.setColor(QColor::fromRgba(penRgba));

    brush.setColor(QColor::fromRgba(brushRgba));
    size.setHeight(height);
    size.setWidth(width);
    isEmpty = !size.isEmpty();
}
