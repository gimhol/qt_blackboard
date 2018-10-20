#ifndef BBITEMDATABASE_H
#define BBITEMDATABASE_H

#include "IStreamWR.h"
#include <QDataStream>
#include "BbToolType.h"

/**
 * @brief The BbItemData class
 *      黑板上item的数据，
 *      位置、类型、id等基本信息。
 */
class BbItemData: public IStreamWR
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
     * @brief isEmpty 是否为空
     */
    bool isEmpty = true;

    /**
     * @brief tooltype 工具类型
     */
    BbToolType tooltype = BBTT_None;

    explicit BbItemData(CoordMode mode = CM_ABSOLUTE);

    virtual ~BbItemData() override;

    // IStreamWR interface
public:
    virtual void writeStream(QDataStream &stream) override;

    virtual void readStream(QDataStream &stream) override;
};

#endif // BBITEMDATABASE_H
