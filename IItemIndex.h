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
    virtual QString id() const = 0;
    virtual void setId(const QString &id) = 0;
    virtual BbToolType toolType() const = 0;

    virtual Blackboard *blackboard() = 0;
    virtual BbScene *scene() = 0;
    virtual BbItemData *data() = 0;
    virtual void repaint() = 0;
    template<class T> T *data() { return dynamic_cast<T*>(data()); }
    virtual bool mouseDown(const QPointF &pos){Q_UNUSED(pos);return false; } // 默认不响应。
    virtual bool mouseMove(const QPointF &pos){Q_UNUSED(pos);return false; } // 默认不响应。
    virtual bool mouseRelease(const QPointF &pos){Q_UNUSED(pos);return false; } // 默认不响应。
    virtual bool clicked(const QPointF &pos){Q_UNUSED(pos);return false; } // 默认不响应单击。
    virtual bool doubleClicked(const QPointF &pos){Q_UNUSED(pos);return false; } // 默认不响应双击。
    virtual void toolDown(const QPointF &pos){Q_UNUSED(pos);}
    virtual void toolDraw(const QPointF &pos){Q_UNUSED(pos);}
    virtual void toolDone(const QPointF &pos){Q_UNUSED(pos);}
    virtual void modifiersChanged(Qt::KeyboardModifiers modifiers) = 0;
    virtual void removed() = 0;
    virtual void added() = 0;
    virtual void toAbsoluteCoords() = 0;
    virtual void updatePrevPosition(){data()->updatePrevPostion();}
    virtual qreal z() = 0;
    virtual void setZ(const qreal &value) = 0;

    IItemIndex *last = nullptr;
    IItemIndex *next = nullptr;
};

#endif // IITEMINDEX_H
