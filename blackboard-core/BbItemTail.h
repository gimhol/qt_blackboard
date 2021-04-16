#ifndef BBITEMTAIL_H
#define BBITEMTAIL_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTimer>
#include "IStreamWR.h"
#include "IItemIndex.h"
#include "IJsonWR.h"

class BbItemTailData;
class NSB_BLACKBOARD_EXPORT BbItemTail :
        public QObject,
        public QGraphicsRectItem,
        public IItemIndex{
    BB_HIDE_POS_SETTER
protected:
    QPainterPath _path;

    QList<QPointF> _changed;

    QRectF _rect;

    BbItemTailData * _data;

    /*直线相关的变量*/
    bool _straight = false;     // 是否开启了直线模式。

    QPointF _straightFrom = QPointF(-999999,-999999);  // 直线的起点。
    QPointF _straightTo = QPointF(-999999,-999999);    // 直线的终点。
    QPointF _mousePos;
public:
    BbItemTail();

    BbItemTail(BbItemData * data);

    virtual ~BbItemTail() override;

    void init();

    QList<QPointF>* changed();

    bool isEmpty();

    void penDown(const QPointF &point);

    void penDraw(const QPointF &point);

    void penStraighting(const QPointF &point);

    void done();

    void repaint() override;

    bool straight();

    void setStraight(const bool & straight);

    QPointF straightTo();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void straightLineDragging(const QPointF &point);

    void addPointToPath(const QPointF &point);

    // IItemIndex interface
public:
   BbItemData *data() override;
    void toolDown(const QPointF &pos) override;
    void toolDraw(const QPointF &pos) override;
    void toolUp(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void absolutize() override;
};

#endif // BBITEMTAIL_H
