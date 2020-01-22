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

void BbItemImageData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);
    QJsonObject jobj;
    jobj["path"] = path;
    jobj["url"] = url;
    stream << pixmap << QJsonDocument(jobj).toBinaryData();
}

void BbItemImageData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    QByteArray data;
    stream >> pixmap >> data;
    auto jobj = QJsonDocument::fromBinaryData(data).object();
    path = jobj["path"].toString();
    url = jobj["url"].toString();

}
