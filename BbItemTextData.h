#ifndef BBITEMTEXTDATA_H
#define BBITEMTEXTDATA_H

#include "BbItemData.h"
#include <QColor>
#include <QFont>

/**
 * @brief The BbItemTextData class
 *      文本 的数据
 */
class BbItemTextData : public BbItemData
{
public:
    static qreal minPointSize;

    static qreal maxPointSize;

    /**
     * @brief defalutFont 创建文本时，文本的字体
     */
    static QFont defalutFont;

    /**
     * @brief defalutColor 创建文本时，文本的颜色
     */
    static QColor defalutColor;

    /**
     * @brief font 当前字体
     */
    QFont font = defalutFont;

    /**
     * @brief color 当前颜色
     */
    QColor color = defalutColor;

    /**
     * @brief text 当前文本内容
     */
    QString text = "";

    /**
     * @brief BbItemTextData 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemTextData(CoordMode mode = CM_ABSOLUTE);

    /**
     * @brief writeStream 从数据流读取
     * @param stream 数据流
     */
    void writeStream(QDataStream &stream);

    /**
     * @brief readStream 写入数据流
     * @param stream 数据流
     */
    void readStream(QDataStream &stream);

    void setPointWeight(qreal pointWeight);

    qreal pointWeight();
};

#endif // BBITEMTEXTDATA_H
