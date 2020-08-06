#ifndef BBITEMTEXTDATA_H
#define BBITEMTEXTDATA_H

#include "BbItemData.h"
#include <QColor>
#include <QFont>

/**
 * @brief The BbItemTextData class
 *      文本 的数据
 */
class NSB_BLACKBOARD_EXPORT BbItemTextData : public BbItemData
{
public:
    static const qreal &getMinPointSize();
    static const qreal &getMaxPointSize();
    static const QFont &getDefalutFont();
    static const QColor &getDefalutColor();
    static void setMinPointSize(const qreal &value);
    static void setMaxPointSize(const qreal &value);
    static void setDefalutFont(const QFont &value);
    static void setDefalutColor(const QColor &value);

    /**
     * @brief font 当前字体
     */
    QFont font;
    /**
     * @brief color 当前颜色
     */
    QColor color;

    /**
     * @brief prevText 修改前的文本内容
     */
    QString prevText;

    /**
     * @brief text 当前文本内容
     */
    QString text;

    /**
     * @brief BbItemTextData 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemTextData(CoordMode mode = CM_ABSOLUTE);

    void setPointWeight(qreal pointWeight);

    qreal pointWeight();

    QJsonObject toJsonObject() override;

    void fromJsonObject(const QJsonObject &jobj) override;
};

#endif // BBITEMTEXTDATA_H
