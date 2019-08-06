#include "BbItemImageData.h"

void BbItemImageData::updatePrevSize()
{
    prevWidth = width;
    prevHeight = height;
}

BbItemImageData::BbItemImageData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Image;
}

void BbItemImageData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);
    stream << width << height << prevWidth << prevHeight << pixmap;
}

void BbItemImageData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    stream >> width >> height >> prevWidth >> prevHeight >> pixmap;
}
