#include "BbItemStraightData.h"

qreal BbItemStraightData::minWidth = 1;

qreal BbItemStraightData::maxWidth = 30;

QPen BbItemStraightData::defaultPen = QPen(QColor(100,100,180), BbItemStraightData::minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);;

BbItemStraightData::BbItemStraightData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_STRAIGHT;
}

void BbItemStraightData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);

    stream << pen.widthF()
           << pen.color().rgba()
           << static_cast<short>(pen.style())
           << a.x() << a.y()
           << b.x() << b.y();
}

void BbItemStraightData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);

    qreal penWidth;
    QRgb rgba;
    short penStyle;
    qreal ax,ay,bx,by;

    stream >> penWidth
           >> rgba
           >> penStyle
           >> ax >> ay
           >> bx >> by;

    isEmpty = false;

    pen.setWidthF(penWidth);
    pen.setStyle(static_cast<Qt::PenStyle>(penStyle));
    pen.setColor(rgba);

    a.setX(ax);
    a.setY(ay);
    b.setX(bx);
    b.setY(by);
}
