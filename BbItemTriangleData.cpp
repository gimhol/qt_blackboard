#include "BbItemTriangleData.h"

QColor BbItemTriangleData::defaultPenColor = QColor(100,100,180);

qreal BbItemTriangleData::minPenWidth = 1.1;

qreal BbItemTriangleData::maxPenWidth = 30;

QPen BbItemTriangleData::defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

QColor BbItemTriangleData::defaultBrushColor = Qt::transparent;

QBrush BbItemTriangleData::defaultBrush = defaultBrushColor;

BbItemTriangleData::BbItemTriangleData(CoordMode mode):
    BbItemData(mode)
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
