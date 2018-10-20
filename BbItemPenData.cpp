#include "BbItemPenData.h"


qreal BbItemPenData::minWidth = 1.1;

qreal BbItemPenData::maxWidth = 30;

QPen BbItemPenData::defaultPen = QPen(QColor(100,100,180), BbItemPenData::minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

BbItemPenData::BbItemPenData(CoordMode mode):
    BbItemData(mode)
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

    for(auto coord: coords){
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

    isEmpty = coordsCount == 0;

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

void BbItemPenData::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}
