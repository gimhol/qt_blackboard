#ifndef BBITEMTAILDATA_H
#define BBITEMTAILDATA_H

#include "BbItemData.h"

/**
 * @brief The BbItemTailData class
 */
class NSB_BLACKBOARD_EXPORT BbItemTailData : public BbItemData
{
protected:
public:
    static const qreal &getMinWidth();
    static const qreal &getMaxWidth();
    static const QPen &getDefaultPen();
    static const bool &getDefaultCubic();
    static const int &getDefaultFadeOutDuration();
    static const int &getDefaultFadeOutDelay();
    static void setMinWidth(const qreal &value);
    static void setMaxWidth(const qreal &value);
    static void setDefaultPen(const QPen &value);
    static void setDefaultCubic(const bool &value);
    static void setDefaultFadeOutDuration(const int &value);      // mesc, default: 1000ms
    static void setDefaultFadeOutDelay(const int &value);         // mesc, default: 1000ms

    /**
     * @brief fadeOutDelay 消失过程的持续时间，单位:毫秒
     */
    int fadeOutDuration;

    /**
     * @brief fadeOutDelay 多久后消失，单位:毫秒
     */
    int fadeOutDelay;

    /**
     * @brief cubic 曲线
     */
    bool cubic;

    /**
     * @brief coords 轨迹的点，[x0,y0,x1,y1...]
     */
    QList<qreal> coords;
    /**
     * @brief BbItemTailData 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemTailData(CoordMode mode = CM_ABSOLUTE);

    void setColor(const QColor &color);

    void setWeight(qreal weight) override;

    qreal weight() override;
};

#endif // BBITEMTAILDATA_H
