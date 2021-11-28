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

    // BbItemData interface
public:
    QJsonObject privateData() override;
    void readPrivateData(const QJsonObject &jdata) override;
};

#endif // BBITEMIMAGEDATA_H
