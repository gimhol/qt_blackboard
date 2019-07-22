#ifndef BBITEMSTRAIGHTDATA_H
#define BBITEMSTRAIGHTDATA_H

#include "BbItemData.h"
#include <QPen>

class NSB_BLACKBOARD_EXPORT BbItemStraightData : public BbItemData
{
public:
    static const qreal &getMinWidth();
    static const qreal &getMaxWidth();
    static const QPen &getDefaultPen();
    static void setMinWidth(const qreal &value);
    static void setMaxWidth(const qreal &value);
    static void setDefaultPen(const QPen &value);

    /**
     * @brief pen 笔的样式
     */
    QPen pen;

    /**
     * @brief a 起点
     */
    QPointF a;
    /**
     * @brief b 终点
     */
    QPointF b;

    explicit BbItemStraightData(CoordMode mode = CM_ABSOLUTE);

    /**
     * @brief writeStream 从数据流读取
     * @param stream 数据流
     */
    virtual void writeStream(QDataStream &stream) override;

    /**
     * @brief readStream 写入数据流
     * @param stream 数据流
     */
    virtual void readStream(QDataStream &stream) override;
};

#endif // BBITEMSTRAIGHTDATA_H
