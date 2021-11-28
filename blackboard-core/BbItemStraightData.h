#ifndef BBITEMSTRAIGHTDATA_H
#define BBITEMSTRAIGHTDATA_H

#include "BbItemData.h"

class NSB_BLACKBOARD_EXPORT BbItemStraightData : public BbItemData
{
public:
    static const qreal &getMinWidth();
    static const qreal &getMaxWidth();
    static const QPen &getDefaultPen();
    static void setMinWidth(const qreal &value);
    static void setMaxWidth(const qreal &value);
    static void setDefaultPen(const QPen &value);

    QPointF a, b;

    explicit BbItemStraightData(CoordMode mode = CM_ABSOLUTE);

    void setWeight(qreal weight) override;

    qreal weight() override;

    // BbItemData interface
public:
    QJsonObject privateData() override;
    void readPrivateData(const QJsonObject &jdata) override;
};

#endif // BBITEMSTRAIGHTDATA_H
