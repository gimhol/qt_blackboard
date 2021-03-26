#ifndef BBITEMPENDATA2_H
#define BBITEMPENDATA2_H

#include "BbItemData.h"
#include <QMessageBox>
/**
 * @brief The BbItemPenData2 class
 *      笔迹 的数据
 */
class NSB_BLACKBOARD_EXPORT BbItemPenData2 : public BbItemData
{
protected:
public:
    static const qreal &getMinWidth();
    static const qreal &getMaxWidth();
    static const QPen &getDefaultPen();
    static const bool &getDefaultCubic();
    static void setMinWidth(const qreal &value);
    static void setMaxWidth(const qreal &value);
    static void setDefaultPen(const QPen &value);
    static void setDefaultCubic(const bool &value);

    /**
     * @brief cubic 曲线
     */
    bool cubic;

    /**
     * @brief coords 轨迹的点，[x0,y0,x1,y1...]
     */
    QList<qreal> coords;

    /**
     * @brief BbItemPenData2 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemPenData2(CoordMode mode = CM_ABSOLUTE);

    void setColor(const QColor &color);

    void setWeight(qreal weight) override;

    qreal weight() override;

    virtual QJsonObject toJsonObject() override;

    virtual void fromJsonObject(const QJsonObject &jobj) override;
};

#endif // BBITEMPENDATA2_H
