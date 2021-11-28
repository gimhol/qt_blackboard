#ifndef BBITEMDATABASE_H
#define BBITEMDATABASE_H

#include "BbHeader.h"

#include "IJsonWR.h"
#include "BbToolType.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPen>
#include <QBrush>

class IItemIndex;
/**
 * @brief The BbItemData class
 *      黑板上item的数据，
 *      位置、类型、id等基本信息。
 */
class NSB_BLACKBOARD_EXPORT BbItemData:  public IJsonWR
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
     * @brief x 上一次的x坐标， -9999代表没有移动, 在移动的事件中，此值将仍未为移动前的位置，直至移动完成。
     */
    qreal prevX = -9999;

    /**
     * @brief y 上一次的y坐标,-9999代表没有移动, 在移动的事件中，此值将仍未为移动前的位置，直至移动完成。
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

    /**
     * @brief needPen 是否用到笔。
     */
    bool needPen = false;
    /**
     * @brief pen 笔的样式
     */
    QPen pen;
    /**
     * @brief needBrush 是否用到画刷。
     */
    bool needBrush = false;
    /**
     * @brief brush 画刷样式
     */
    QBrush brush;
    /**
     * @brief needSize 是否用到尺寸。
     */
    bool needSize = false;
    /**
     * @brief width, height item的尺寸
     */
    qreal width = -1, height = -1;

    /**
     * @brief prevWidth, prevHeight
     *      修改前的尺寸, 拖拽图片边缘改变图片尺寸时，此值应为修改前的尺寸，直至松开鼠标。
     */
    qreal prevWidth = -1, prevHeight = -1;

    /**
     * @brief persistence 是否能被持久化贮存
     */
    bool persistence = true;

    /**
     * @brief locked 被锁定的图形仅可被选中。
     */
    bool locked = false;

    /**
     * @brief needPrivateData 是否需要读写自己特有的数据。
     */
    bool needPrivateData = true;

    virtual qreal weight(){return 1;}

    virtual void setWeight(qreal){}

    explicit BbItemData(CoordMode mode = CM_ABSOLUTE);

    virtual ~BbItemData() override;

    virtual QPointF position() { return QPointF(x,y); }

    virtual QPointF prevPosition() { return QPointF(prevX,prevY); }

    virtual bool isPositionValid() const;

    virtual bool isPrevPositionValid() const;

    virtual bool isSizeValid() const;

    virtual bool isPrevSizeValid() const;

    virtual bool isSizeNull() const;

    virtual bool isSizeEmpty() const;

    virtual bool isPrevSizeNull() const;

    virtual bool isPrevSizeEmpty() const;

    /**
     * @brief fixPostion 更新data中的x与y，同时修改prevX与prevY。
     * @param index 用于读取x与y的item。
     */
    virtual void fixPostion(const qreal &x, const qreal &y);
    /**
     * @brief updatePostion 更新data中的x与y，当prevX与prevY为非正常值（或未初始化）时，同时修改prevX与prevY。
     * @param index 用于读取x与y的item。
     */
    virtual void updatePostion(const qreal &x, const qreal &y);
    /**
     * @brief fixPostion 更新data中的x与y，同时修改prevX与prevY。
     * @param index 用于读取x与y的item。
     */
    virtual void fixPostion(IItemIndex *index);
    /**
     * @brief updatePostion 更新data中的x与y，当prevX与prevY为非正常值（或未初始化）时，同时修改prevX与prevY。
     * @param index 用于读取x与y的item。
     */
    virtual void updatePostion(IItemIndex *index);
    /**
     * @brief updatePrevPostion prevX = x; prevY = y;
     */
    virtual void updatePrevPostion();

    virtual void updatePrevSize();

    // IJsonWR interface
public:
    virtual QJsonObject toJsonObject() override;

    virtual void fromJsonObject(const QJsonObject &jobj) override;

    // item.data Json interface;
public:
    virtual QJsonObject privateData() { return QJsonObject(); }

    virtual void readPrivateData(const QJsonObject &jdata) { Q_UNUSED(jdata); }
};

#endif // BBITEMDATABASE_H
