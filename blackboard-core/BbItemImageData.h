#ifndef BBITEMIMAGEDATA_H
#define BBITEMIMAGEDATA_H

#include "BbItemData.h"
#include <QPixmap>

class NSB_BLACKBOARD_EXPORT BbItemImageData : public BbItemData
{
public:
    static const bool &isRatioLocked();

    static void setRatioLocked(const bool &value);
    static void unlockRatio();
    static void lockRatio();

    QString path; // 本地目录

    QString url; // 图片地址

    QPixmap pixmap;

    explicit BbItemImageData(CoordMode mode = CM_ABSOLUTE);

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;

    // IJsonWR interface
public:
    virtual QJsonObject toJsonObject() override;

    virtual void fromJsonObject(const QJsonObject &jobj) override;

};

#endif // BBITEMIMAGEDATA_H
