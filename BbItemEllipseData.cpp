#include "BbItemEllipseData.h"

QColor BbItemEllipseData::defaultPenColor = QColor(100,100,180);

qreal BbItemEllipseData::minPenWidth = 1.1;

qreal BbItemEllipseData::maxPenWidth = 30;

QPen BbItemEllipseData::defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

QColor BbItemEllipseData::defaultBrushColor = Qt::transparent;

QBrush BbItemEllipseData::defaultBrush = defaultBrushColor;

BbItemEllipseData::BbItemEllipseData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Rectangle;
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
    pen.setColor(penRgba);

    brush.setColor(brushRgba);
    size.setHeight(height);
    size.setWidth(width);
}
