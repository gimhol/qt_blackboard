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
    //Weiqi:
    unsigned int flags = 0x900dbeef;
public:
    ~BbItemPenData() override{
        if(flags==0xdeadbeef){
            QMessageBox::critical(nullptr, QStringLiteral("黑板故障"), QStringLiteral(
                                      "二次析构笔迹数据。"
                                      "如果您是老师，请记住前几秒的使用在线课堂上课的操作并将本错误拍照告知牛师帮在线课堂。\n"
                                      "如果您是学生，请拍照并告诉老师。"));
            qFatal(__FUNCTION__"0xdeadbeef");
        }
        if(flags!=0x900dbeef){
            QMessageBox::critical(nullptr, QStringLiteral("黑板故障"), QStringLiteral(
                                     "析构野指针笔迹数据。"
                                     "如果您是老师，请记住前几秒的使用在线课堂上课的操作并将本错误拍照告知牛师帮在线课堂。\n"
                                     "如果您是学生，请拍照并告诉老师。"));
            qFatal(__FUNCTION__"no 0x900dbeef");
        }
        flags=0xdeadbeef;
    }
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
