#ifndef BBITEMDATABASE_H
#define BBITEMDATABASE_H

#include "BbHeader.h"
#include "IStreamWR.h"
#include "BbToolType.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class IItemIndex;
/**
 * @brief The BbItemData class
 *      黑板上item的数据，
 *      位置、类型、id等基本信息。
 */
class NSB_BLACKBOARD_EXPORT BbItemData: public IStreamWR
{
public:
    enum CoordMode
    {
        CM_ABSOLUTE,    // 绝对坐标，本地使用。
        CM_PERCENTAGE   // 百分比坐标，通讯用。percentageX = absoluteX * 100 / width; percentageY = absoluteY * 100 / width;
    };

    /**
     * @brief mode 坐标模式
     */
    CoordMode mode = CM_ABSOLUTE;

    /**
     * @brief lid 黑板上item的id
     */
    QString lid = "";

    /**
     * @brief x 移动位置的x坐标， -9999代表没有移动
     */
    qreal x = -9999;

    /**
     * @brief y 移动位置的y坐标,-9999代表没有移动
     */
    qreal y = -9999;

    /**
     * @brief z 层次，越大的越在上面
     */
    qreal z = -9999;

    /**
     * @brief x 上一次的x坐标， -9999代表没有移动
     */
    qreal prevX = -9999;

    /**
     * @brief y 上一次的y坐标,-9999代表没有移动
     */
    qreal prevY = -9999;

    /**
     * @brief z 上一次的层次，越大的越在上面
     */
    qreal prevZ = -9999;

    /**
     * @brief empty 是否为空
     */
    bool empty = true;

    /**
     * @brief editable 是否允许被编辑
     */
    bool editable = false;

    /**
     * @brief tooltype 工具类型
     */
    BbToolType tooltype = BBTT_None;

    explicit BbItemData(CoordMode mode = CM_ABSOLUTE);

    virtual ~BbItemData() override;

    QPointF position() { return QPointF(x,y); }
    QPointF prevPosition() { return QPointF(prevX,prevY); }

    virtual bool isPositionValid();

    virtual bool isPrevPositionValid();

    virtual void updatePostion(IItemIndex *itemIdx);

    virtual void updatePrevPostion();
    // IStreamWR interface
public:
    virtual void writeStream(QDataStream &stream) override;

    virtual void readStream(QDataStream &stream) override;
};

#endif // BBITEMDATABASE_H
