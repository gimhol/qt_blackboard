#ifndef BBITEMSTRAIGHTDATA_H
#define BBITEMSTRAIGHTDATA_H

#include "BbItemData.h"
#include <QPen>

class BbItemStraightData : public BbItemData
{
public:
    static qreal minWidth;
    static qreal maxWidth;
    /**
     * @brief defaultPen 创建笔时，笔的样式
     */
    static QPen defaultPen;

    /**
     * @brief pen 笔的样式
     */
    QPen pen = defaultPen;

    QPointF a;

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
