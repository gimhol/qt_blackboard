#ifndef BBITEMTRIANGLEDATA_H
#define BBITEMTRIANGLEDATA_H


#include "BbItemData.h"
#include <QPen>
#include <QBrush>

class BbItemTriangleData: public BbItemData
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

    QPointF points[3];

    explicit BbItemTriangleData(CoordMode mode = CM_ABSOLUTE);

    qreal weight();

    void setWeight(qreal weight);

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;
};
#endif // BBITEMTRIANGLEDATA_H
