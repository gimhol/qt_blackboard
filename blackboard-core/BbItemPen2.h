#ifndef BBITEMPEN2_H
#define BBITEMPEN2_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"
#include "IJsonWR.h"

class BbItemPenData2;
class NSB_BLACKBOARD_EXPORT BbItemPen2 :
        public QAbstractGraphicsShapeItem,
        public IItemIndex,
        public IStreamWR,
        public IJsonWR{
    BB_HIDE_POS_SETTER
protected:
#ifdef NSB_SAVE_PEN_TO_PIXMAP_WHEN_DONE
    QPixmap _pixmap;
#endif

#ifdef NSB_BLACKBOARD_PEN_ITEM_SMOOTHING
    QList<QPointF> _temp;

    float _distances[3];
#endif

    QPainterPath _path;

    QList<QPointF> _changed;

    QRectF _rect;

    BbItemPenData2 * _data;

    /*直线相关的变量*/
    bool _straight = false;     // 是否开启了直线模式。

    QPointF _straightFrom = QPointF(-999999,-999999);  // 直线的起点。
    QPointF _straightTo = QPointF(-999999,-999999);    // 直线的终点。
    QPointF _mousePos;
    bool _editing = false;

    bool _straighting = false;
public:
    BbItemPen2();

    BbItemPen2(BbItemData * data);

    virtual ~BbItemPen2() override;

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

    QRectF boundingRect() const override;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void straightLineDragging(const QPointF &point);

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &steam) override;
    virtual void readStream(QDataStream &steam) override;


    // IJsonWR interface
public:
    virtual QJsonObject toJsonObject() override;
    virtual void fromJsonObject(const QJsonObject &jobj) override;

    // IItemIndex interface
public:
   BbItemData *data() override;
    void toolDown(const QPointF &pos) override;
    void toolDraw(const QPointF &pos) override;
    void toolUp(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void absolutize() override;
    bool isEditing() override;
};

#endif // BBITEMPEN2_H
