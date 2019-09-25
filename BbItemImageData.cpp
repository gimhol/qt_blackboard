#include "BbItemImageData.h"

static bool ratioLocked = true;

const bool &BbItemImageData::isRatioLocked()
{
    return ratioLocked;
}

void BbItemImageData::setRatioLocked(const bool &value)
{
    ratioLocked = value;
}

void BbItemImageData::unlockRatio()
{
    ratioLocked = false;
}

void BbItemImageData::lockRatio()
{
    ratioLocked = true;
}

void BbItemImageData::updatePrevSize()
{
    prevWidth = width;
    prevHeight = height;
}

BbItemImageData::BbItemImageData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Image;
    editable = true;
}

void BbItemImageData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);
    stream << width << height
           << prevWidth << prevHeight
           << path << url
           << pixmap;
}

void BbItemImageData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    stream >> width >> height
            >> prevWidth >> prevHeight
            >> path >> url
            >> pixmap;
}
