#include "BbItemData.h"

BbItemData::BbItemData(CoordMode mode):
    mode(mode)
{

}

BbItemData::~BbItemData()
{

}

void BbItemData::writeStream(QDataStream &stream){
    stream << static_cast<int>(mode) << lid << x << y << z;
}

void BbItemData::readStream(QDataStream &stream){
    int imode;
    stream >> imode >> lid >> x >> y >> z;
    mode = static_cast<CoordMode>(imode);
}
