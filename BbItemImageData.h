#ifndef BBITEMIMAGEDATA_H
#define BBITEMIMAGEDATA_H

#include "BbItemData.h"
#include <QPixmap>

class BbItemImageData : public BbItemData
{
public:
    QPixmap pixmap;

    explicit BbItemImageData(CoordMode mode = CM_ABSOLUTE);

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;
};

#endif // BBITEMIMAGEDATA_H
