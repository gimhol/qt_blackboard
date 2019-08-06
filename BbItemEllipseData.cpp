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

    stream << pen.widthF()
           << pen.color().rgba()
           << static_cast<short>(pen.style())
           << brush.color().rgba()
           << size.width()
           << size.height();
}

void BbItemEllipseData::readStream(QDataStream &stream)
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
