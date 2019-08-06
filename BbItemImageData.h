#ifndef BBITEMIMAGEDATA_H
#define BBITEMIMAGEDATA_H

#include "BbItemData.h"
#include <QPixmap>

class NSB_BLACKBOARD_EXPORT BbItemImageData : public BbItemData
{
public:
    qreal width = 0;

    qreal height = 0;

    qreal prevWidth = 0;

    qreal prevHeight = 0;

    QPixmap pixmap;

    void updatePrevSize();

    explicit BbItemImageData(CoordMode mode = CM_ABSOLUTE);

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;
};

#endif // BBITEMIMAGEDATA_H
