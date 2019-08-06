#include "BbItemPenData.h"


static qreal minWidth = 1.1;
static qreal maxWidth = 30;
static QPen defaultPen = QPen(QColor(100,100,180), minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

const qreal & BbItemPenData::getMinWidth()
{
    return minWidth;
}
const qreal & BbItemPenData::getMaxWidth()
{
    return maxWidth;
}
const QPen & BbItemPenData::getDefaultPen()
{
    return defaultPen;
}
void BbItemPenData::setMinWidth(const qreal &value)
{
    minWidth = value;
}
void BbItemPenData::setMaxWidth(const qreal &value)
{
    maxWidth = value;
}

void BbItemPenData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

BbItemPenData::BbItemPenData(CoordMode mode):
    BbItemData(mode),
    pen(defaultPen)
{
    tooltype = BBTT_Pen;
}

void BbItemPenData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);

    stream << pen.widthF()
           << pen.color().rgba()
           << static_cast<short>(pen.style())
           << coords.size();

    for(auto coord: coords)
    {
        stream << coord;
    }
}

void BbItemPenData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);

    qreal penWidth;
    QRgb rgba;
    short penStyle;
    int coordsCount;

    stream >> penWidth >> rgba >> penStyle
           >> coordsCount;

    empty = coordsCount == 0;

    pen.setWidthF(penWidth);
    pen.setStyle(static_cast<Qt::PenStyle>(penStyle));
    pen.setColor(rgba);

    qreal crood;
    for(int i = 0;i < coordsCount; ++i)
    {
        stream >> crood;
        coords.append(crood);
    }
}

void BbItemPenData::setColor(const QColor &color)
{
    pen.setColor(color);
}

void BbItemPenData::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}

qreal BbItemPenData::weight()
{
    return (pen.widthF() - minWidth) / (maxWidth - minWidth);
}
