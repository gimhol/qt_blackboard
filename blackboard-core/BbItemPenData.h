#ifndef CITEMPENDATA_H
#define CITEMPENDATA_H

#include "BbItemData.h"

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
     * @brief coords 轨迹的点，[x0,y0,x1,y1...]
     */
    QList<qreal> coords;

    /**
     * @brief BbItemPenData 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemPenData(CoordMode mode = CM_ABSOLUTE);

    void setColor(const QColor &color);

    void setWeight(qreal weight) override;

    qreal weight() override;

    virtual QJsonObject toJsonObject() override;

    virtual void fromJsonObject(const QJsonObject &jobj) override;
};

#endif // CITEMPENDATA_H
