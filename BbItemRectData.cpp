#include "BbItemRectData.h"

QColor BbItemRectData::defaultPenColor = QColor(100,100,180);

qreal BbItemRectData::minPenWidth = 1.1;

qreal BbItemRectData::maxPenWidth = 30;

QPen BbItemRectData::defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

QColor BbItemRectData::defaultBrushColor = Qt::transparent;

QBrush BbItemRectData::defaultBrush = defaultBrushColor;

BbItemRectData::BbItemRectData(CoordMode mode):
    BbItemData(mode)
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
    pen.setColor(penRgba);

    brush.setColor(brushRgba);
    size.setHeight(height);
    size.setWidth(width);
}
