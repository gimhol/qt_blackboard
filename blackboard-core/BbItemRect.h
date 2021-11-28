#ifndef BBITEMRECT_H
#define BBITEMRECT_H

#include <QGraphicsItem>

#include "IItemIndex.h"

class BbItemRectData;
class NSB_BLACKBOARD_EXPORT BbItemRect:
        public QGraphicsRectItem,
        public IItemIndex,
        
        public IJsonWR
{
    BB_HIDE_POS_SETTER
protected:
    BbItemRectData *_data;

    qreal _beginX, _beginY;

    qreal _dragX, _dragY;

    QPointF _mousePos;

    bool _square = false;

    bool _editing = false;
public:
    BbItemRect();

    BbItemRect(BbItemData *data);

    ~BbItemRect() override;

    void init();

    void repaint() override;

    void begin(const QPointF & point);

    void draw(const QPointF & point);

    void done();

    bool square();

    void setSquare(const bool square);

    void toNinety(const QPointF &mousePos, qreal & outX,qreal & outY);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    // IItemIndex interface
public:
    BbItemData *data() override;
    void toolDown(const QPointF &pos) override;
    void toolDraw(const QPointF &pos) override;
    void toolUp(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void absolutize() override;
    bool isEditing() override;

    // IJsonWR interface
public:
    virtual QJsonObject toJsonObject() override;
    virtual void fromJsonObject(const QJsonObject &jobj) override;

};

#endif // BBITEMRECT_H
