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

void BbItemStraightData::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}

qreal BbItemStraightData::weight()
{
    return (pen.widthF() - minWidth) / (maxWidth - minWidth);
}
