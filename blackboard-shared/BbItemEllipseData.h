#ifndef BBITEMELLIPSEDATA_H
#define BBITEMELLIPSEDATA_H

#include "BbItemData.h"
/**
 * @brief The BbItemEllipseData class
 *      '椭圆'的数据类
 */
class NSB_BLACKBOARD_EXPORT BbItemEllipseData: public BbItemData
{
public:
    /**
     * @brief getDefaultPenColor 获取默认的描边颜色
     * @return 描边颜色
     */
    static const QColor &getDefaultPenColor();
    /**
     * @brief getDefaultPenColor 获取最小描边宽度
     * @return 最小描边宽度
     */
    static const qreal &getMinPenWidth();
    /**
     * @brief getDefaultPenColor 获取最大描边宽度
     * @return 最大描边宽度
     */
    static const qreal &getMaxPenWidth();
    /**
     * @brief getDefaultPen 获取默认描边样式
     * @return 默认描边样式
     */
    static const QPen &getDefaultPen();
    /**
     * @brief getDefaultPen 获取默认画刷颜色
     * @return 默认画刷颜色
     */
    static const QColor &getDefaultBrushColor();
    /**
     * @brief getDefaultPen 获取默认画刷样式
     * @return 默认画刷样式
     */
    static const QBrush &getDefaultBrush();
    /**
     * @brief setDefaultPenColor 设置默认描边颜色
     * @param value 默认描边颜色
     */
    static void setDefaultPenColor(const QColor &value);
    /**
     * @brief setMinPenWidth 设置最小描边宽度
     * @param value 最小描边宽度
     */
    static void setMinPenWidth(const qreal &value);
    /**
     * @brief setMaxPenWidth 设置最大描边宽度
     * @param value 最大描边宽度
     */
    static void setMaxPenWidth(const qreal &value);
    /**
     * @brief setDefaultPen 设置默认描边样式
     * @param value 默认描边样式
     */
    static void setDefaultPen(const QPen &value);
    /**
     * @brief setDefaultBrushColor 设置默认填充颜色
     * @param value 默认填充颜色
     */
    static void setDefaultBrushColor(const QColor &value);
    /**
     * @brief setDefaultBrush 设置默认填充样式
     * @param value 默认填充样式
     */
    static void setDefaultBrush(const QBrush &value);
    /**
     * @brief BbItemEllipseData 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemEllipseData(CoordMode mode = CM_ABSOLUTE);
    /**
     * @brief weight 获取描边粗细程度
     * @return 描边粗细程度（0~1）
     */
    qreal weight() override;
    /**
     * @brief setWeight 获取设置粗细程度
     * @param weight 描边粗细程度（0~1）
     */
    void setWeight(qreal weight) override;

    void fromJsonObject(const QJsonObject &jobj) override;
};

#endif // BBITEMELLIPSEDATA_H
