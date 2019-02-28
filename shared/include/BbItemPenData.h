#ifndef CITEMPENDATA_H
#define CITEMPENDATA_H

#include "BbItemData.h"
#include <QPen>

/**
 * @brief The BbItemPenData class
 *      笔迹 的数据
 */
class NSB_BLACKBOARD_EXPORT BbItemPenData : public BbItemData
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
     * @brief coords 轨迹的点，[x0,y0,x1,y1...]
     */
    QList<qreal> coords;

    /**
     * @brief BbItemPenData 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemPenData(CoordMode mode = CM_ABSOLUTE);

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

    void setWeight(qreal weight);
};

#endif // CITEMPENDATA_H
