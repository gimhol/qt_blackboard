#ifndef IITEMINDEX_H
#define IITEMINDEX_H

#include "BbHeader.h"
#include <QPointF>
#include <QString>
#include "BbToolType.h"
class Blackboard;
class BbScene;
class BbItemData;
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

    virtual void toolDown(const QPointF &pos) = 0;
    virtual void toolDraw(const QPointF &pos) = 0;
    virtual void toolDone(const QPointF &pos) = 0;
    virtual void modifiersChanged(Qt::KeyboardModifiers modifiers) = 0;
    virtual void removed() = 0;
    virtual void added() = 0;
    IItemIndex *last = nullptr;
    IItemIndex *next = nullptr;
};

#endif // IITEMINDEX_H
