#include "BbItemTriangleData.h"


static QColor defaultPenColor = QColor(100,100,180);

static qreal minPenWidth = 1.1;

static qreal maxPenWidth = 30;

static QPen defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

static QColor defaultBrushColor = Qt::transparent;

static QBrush defaultBrush = defaultBrushColor;

const QColor &BbItemTriangleData::getDefaultPenColor()
{
    return defaultPenColor;
}

const qreal &BbItemTriangleData::getMinPenWidth()
{
    return minPenWidth;
}

const qreal &BbItemTriangleData::getMaxPenWidth()
{
    return maxPenWidth;
}

const QPen &BbItemTriangleData::getDefaultPen()
{
    return defaultPen;
}

const QColor &BbItemTriangleData::getDefaultBrushColor()
{
    return defaultBrushColor;
}

const QBrush &BbItemTriangleData::getDefaultBrush()
{
    return defaultBrush;
}

void BbItemTriangleData::setDefaultPenColor(const QColor &value)
{
    defaultPenColor = value;
}

void BbItemTriangleData::setMinPenWidth(const qreal &value)
{
    minPenWidth = value;
}

void BbItemTriangleData::setMaxPenWidth(const qreal &value)
{
    maxPenWidth = value;
}

void BbItemTriangleData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

void BbItemTriangleData::setDefaultBrushColor(const QColor &value)
{
    defaultBrushColor = value;
}

void BbItemTriangleData::setDefaultBrush(const QBrush &value)
{
    defaultBrush = value;
}

BbItemTriangleData::BbItemTriangleData(CoordMode mode):
    BbItemData(mode),
    pen(defaultPen),
    brush(defaultBrush)
{
    tooltype = BBTT_Triangle;
}

qreal BbItemTriangleData::weight()
{
    return (pen.widthF() - minPenWidth)/(maxPenWidth - minPenWidth);
}

void BbItemTriangleData::setWeight(qreal weight)
{
    pen.setWidthF(minPenWidth + weight * (maxPenWidth - minPenWidth));
}

void BbItemTriangleData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);

    stream << pen.widthF()
           << pen.color().rgba()
           << static_cast<short>(pen.style())
           << brush.color().rgba();

    for(int i = 0; i < 3; ++i)
    {
        stream << points[i].x() << points[i].y();
    }
}

void BbItemTriangleData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    isEmpty = false;
    qreal penWidth;
    QRgb penRgba,brushRgba;
    short penStyle;

    stream >> penWidth
           >> penRgba
           >> penStyle
           >> brushRgba;

    for(int i = 0; i < 3; ++i)
    {
        qreal x,y;
        stream >> x >> y;
        points[i] = QPointF(x,y);
    }

    pen.setWidthF(penWidth);
    pen.setStyle(static_cast<Qt::PenStyle>(penStyle));
    pen.setColor(QColor::fromRgba(penRgba));
    brush.setColor(QColor::fromRgba(brushRgba));
}
