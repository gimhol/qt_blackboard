#ifndef IITEMINDEX_H
#define IITEMINDEX_H

#include "BbHeader.h"
#include <QPointF>
#include <QString>
#include "BbItemData.h"
#include "BbToolType.h"
class Blackboard;
class BbScene;
class NSB_BLACKBOARD_EXPORT IItemIndex
{
public:
    IItemIndex(BbItemData *){}
    virtual ~IItemIndex()
    {
        if(last)
        {
            last->next = next;
        }
        if(next)
        {
            next->last = last;
        }
    }
    /**
     * @brief id 唯一标识符
     * @return 唯一标识符
     */
    virtual QString id() const = 0;
    /**
     * @brief setId 设置唯一标识符
     * @param id 唯一标识符
     */
    virtual void setId(const QString &id) = 0;
    /**
     * @brief toolType 工具类型。
     * @return 工具类型。
     */
    virtual BbToolType toolType() const = 0;
    /**
     * @brief blackboard 本item的所处的Blackboard
     * @return 本item的所处的Blackboard
     */
    virtual Blackboard *blackboard() = 0;
    /**
     * @brief scene 本item的所处的 BbScene
     * @return 本item的所处的 BbScene
     */
    virtual BbScene *scene() = 0;
    /**
     * @brief data 本item的数据基类指针
     * @return 本item的数据基类指针
     */
    virtual BbItemData *data() = 0;
    /**
     * @brief repaint 根据data重绘item的逻辑。
     */
    virtual void repaint() = 0;
    /**
     * @brief mouseDown 当本item就是编辑中item，鼠标左键在黑板按下时会被调用
     * @param pos 鼠标位置
     * @return
     *      返回true时，鼠标左键按下的事件将不会继续处理。
     *      返回false时，将会继续分发。
     */
    virtual bool mouseDown(const QPointF &pos){Q_UNUSED(pos);return false; }
    /**
     * @brief mouseMove 当本item就是编辑中item时，鼠标在黑板中移动时会被调用（即使不点击也会）
     *      如：返回false时，move事件将不能移动此item。
     * @param pos 鼠标位置
     * @return
     *      返回true时，鼠标移动按下的事件将不会继续处理。
     *      返回false时，将会继续分发。
     */
    virtual bool mouseMove(const QPointF &pos){Q_UNUSED(pos);return false; }
    /**
     * @brief mouseRelease 当本item就是编辑中item，鼠标左键在黑板抬起时会被调用
     * @param pos 鼠标位置
     * @return
     *      返回true时，鼠标左键抬起的事件将不会继续处理。
     *      返回false时，将会继续分发。
     */
    virtual bool mouseRelease(const QPointF &pos){Q_UNUSED(pos);return false; }

    /**
     * @brief clicked Picker工具下，被鼠标左键点击时被调用
     *      鼠标左键按下时，会按Z值从高到低的遍历underMouse的item并调用其clicked，
     *      直到某个item的clicked返回true。
     * @param pos 鼠标位置
     * @return
     *      返回true时，表示本item会处理。
     *      返回false时，表示本item不处理。
     */
    virtual bool clicked(const QPointF &pos){Q_UNUSED(pos);return false; }
    /**
     * @brief doubleClicked Picker工具下，被鼠标左键双击时被调用
     *      双击时，会按Z值从高到低的遍历underMouse的item并调用其doubleClicked，
     *      直到某个item的doubleClicked返回true。
     * @param pos 鼠标位置
     * @return
     *      返回true时，表示本item会处理。
     *      返回false时，表示本item不处理。
     */
    virtual bool doubleClicked(const QPointF &pos){Q_UNUSED(pos);return false; }
    /**
     * @brief toolDown
     * @param pos 鼠标位置
     */
    virtual void toolDown(const QPointF &pos){Q_UNUSED(pos);}
    /**
     * @brief toolDraw
     * @param pos 鼠标位置
     */
    virtual void toolDraw(const QPointF &pos){Q_UNUSED(pos);}
    /**
     * @brief toolDraw
     * @param pos 鼠标位置
     */
    virtual void toolDone(const QPointF &pos){Q_UNUSED(pos);}
    virtual void modifiersChanged(Qt::KeyboardModifiers modifiers) = 0;
    /**
     * @brief removed 被移除时调用
     */
    virtual void removed() = 0;
    /**
     * @brief added 被添加时调用
     */
    virtual void added() = 0;
    /**
     * @brief toAbsoluteCoords 实现百分比数据转为绝对数据的逻辑。
     */
    virtual void toAbsoluteCoords() = 0;
    /**
     * @brief updatePrevPosition 更新前一个位置记录。
     */
    virtual void updatePrevPosition(){data()->updatePrevPostion();}
    /**
     * @brief z值，越大越在上方
     * @return z值
     */
    virtual qreal z() = 0;
    /**
     * @brief setZ 设置Z值
     * @param value z值，越大越在上方。
     */
    virtual void setZ(const qreal &value) = 0;
    /**
     * @brief last 前一个item
     *      仅应该在BbScene的enumSelected与enumAll中被重置，
     *      用于快速的遍历一系列item
     */
    IItemIndex *last = nullptr;
    /**
     * @brief next 下一个item
     *      仅应该在BbScene的enumSelected与enumAll中被重置，
     *      用于快速的遍历一系列item
     */
    IItemIndex *next = nullptr;
};

#endif // IITEMINDEX_H
