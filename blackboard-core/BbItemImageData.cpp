#include "BbItemImageData.h"

static bool ratioLocked = false;

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

BbItemImageData::BbItemImageData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Image;
    editable = true;
    needSize = true;
}

QJsonObject BbItemImageData::privateData()
{
    QJsonObject jdata;
    jdata["path"] = path;
    jdata["url"] = url;
    return jdata;
}

void BbItemImageData::readPrivateData(const QJsonObject &jdata)
{
    path = jdata["path"].toString();
    url = jdata["url"].toString();
}
