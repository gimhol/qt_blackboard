#ifndef CITEMPENDATA_H
#define CITEMPENDATA_H

#include "BbItemData.h"
#include <QMessageBox>
/**
 * @brief The BbItemPenData class
 *      笔迹 的数据
 */
class NSB_BLACKBOARD_EXPORT BbItemPenData : public BbItemData
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
     * @brief BbItemPenData 构造函数
     * @param mode 坐标模式
     */
    explicit BbItemPenData(CoordMode mode = CM_ABSOLUTE);

    void setColor(const QColor &color);

    void setWeight(qreal weight) override;

    qreal weight() override;

    // BbItemData interface
public:
    QJsonObject privateData() override;
    void readPrivateData(const QJsonObject &jdata) override;
};

class NSB_BLACKBOARD_EXPORT BbItemPenData2 : public BbItemPenData {
public:
    BbItemPenData2(CoordMode mode = CM_ABSOLUTE):
        BbItemPenData(mode){
        tooltype = BBTT_Pen2;
    }
};

#endif // CITEMPENDATA_H
