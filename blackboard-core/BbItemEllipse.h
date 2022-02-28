#ifndef BBITEMELLIPSE_H
#define BBITEMELLIPSE_H

#include "BbHeader.h"
#include <QGraphicsItem>

#include "IItemIndex.h"

class BbItemEllipseData;
class NSB_BLACKBOARD_EXPORT BbItemEllipse:
        public QGraphicsRectItem,
        public IItemIndex,
        
        public IJsonWR
{
    BB_HIDE_POS_SETTER
protected:
    BbItemEllipseData *_data;

    qreal _beginX, _beginY;

    qreal _dragX, _dragY;

    QPointF _mousePos;

    bool _circular = false;
    bool _startFromCenter = false;
    bool _pointcut = false;
    bool _editing = false;
public:
    BbItemEllipse();

    BbItemEllipse(BbItemData * data);

    ~BbItemEllipse() override;

    void init();

    void repaint() override;

    void begin(const QPointF & point);

    void draw(const QPointF & point);

    void done();

    bool isCircular();
    bool isStartFromCenter();
    bool isPointcut();

    void setCircular(bool circle,bool drawImmediately = false);

    void setStartFromCenter(bool startFromCenter,bool drawImmediately = false);

    void setPointcut(bool pointcut,bool drawImmediately = false);

    void toCircular(const QPointF &mousePos, qreal & outX,qreal & outY);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

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

#endif // BBITEMELLIPSE_H
