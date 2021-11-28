#ifndef BBITEMTRIANGLEDATA_H
#define BBITEMTRIANGLEDATA_H

#include "BbItemData.h"

class NSB_BLACKBOARD_EXPORT BbItemTriangleData: public BbItemData
{
public:
    static const QColor &getDefaultPenColor();
    static const qreal &getMinPenWidth();
    static const qreal &getMaxPenWidth();
    static const QPen &getDefaultPen();
    static const QColor &getDefaultBrushColor();
    static const QBrush &getDefaultBrush();

    static void setDefaultPenColor(const QColor &value);
    static void setMinPenWidth(const qreal &value);
    static void setMaxPenWidth(const qreal &value);
    static void setDefaultPen(const QPen &value);
    static void setDefaultBrushColor(const QColor &value);
    static void setDefaultBrush(const QBrush &value);

    QPointF points[3];

    explicit BbItemTriangleData(CoordMode mode = CM_ABSOLUTE);

    qreal weight() override;

    void setWeight(qreal weight) override;

    // BbItemData interface
public:
    QJsonObject privateData() override;
    void readPrivateData(const QJsonObject &jdata) override;
};
#endif // BBITEMTRIANGLEDATA_H
