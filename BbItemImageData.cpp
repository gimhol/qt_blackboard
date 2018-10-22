#include "BbItemImageData.h"

BbItemImageData::BbItemImageData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Image;
}

void BbItemImageData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);
    stream << pixmap;
}

void BbItemImageData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    stream >> pixmap;
}
