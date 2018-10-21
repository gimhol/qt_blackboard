#ifndef BBITEMRECTDATA_H
#define BBITEMRECTDATA_H

#include "BbItemData.h"
#include <QPen>
#include <QBrush>

class BbItemRectData : public BbItemData
{
public:
    static QColor defaultPenColor;

    static qreal minPenWidth;

    static qreal maxPenWidth;

    static QPen defaultPen;

    static QColor defaultBrushColor;

    static QBrush defaultBrush;

    QPen pen = defaultPen;

    QBrush brush = defaultBrush;

    QSizeF size;

    explicit BbItemRectData(CoordMode mode = CM_ABSOLUTE);

    qreal weight();

    void setWeight(qreal weight);

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;
};

#endif // BBITEMRECTDATA_H
