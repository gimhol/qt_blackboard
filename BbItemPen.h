#ifndef CANVASELEMENTPEN3_HPP
#define CANVASELEMENTPEN3_HPP

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemPenData;
class NSB_BLACKBOARD_EXPORT BbItemPen : public QGraphicsRectItem, public IStreamWR, public IItemIndex{

protected:
#if SAVE_TO_PIXMAP_WHEN_DONE
    QPixmap *_pixmap = nullptr;
#endif

#if NSB_BLACKBOARD_PEN_ITEM_SMOOTHING
    QList<QPointF> _temp;

    float _distances[3];
#endif

    QPainterPath *_path = nullptr;

    QList<QPointF> _changed;

    QRectF _rect;

    BbItemPenData * _myData;

    /*直线相关的变量*/
    bool _straight = false;     // 是否开启了直线模式。

    QPointF _straightLineFrom = QPointF(-999999,-999999);    // 直线的终点。
    QPointF _straightLineTo = QPointF(-999999,-999999);    // 直线的终点。
    QPointF _mousePos;
public:
    BbItemPen();

    BbItemPen(BbItemPenData * penData);

    virtual ~BbItemPen() override;

    QList<QPointF>* changed();

    bool isEmpty();

    void penDown(const QPointF &point);

    void penDraw(const QPointF &point);

    void penStraighting(const QPointF &point);

    void done();

    QColor color();

    void setColor(const QColor &color);

    qreal penWidth();

    qreal weight();

    void setPenWidth(qreal width);

    void setWeight(qreal weight);

    void repaintWithItemData();

    bool straight();

    void setStraight(const bool & straight);

    QPointF straightTo();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void straightLineDragging(const QPointF &point);

    void addPointToPath(const QPointF &point);

#if NSB_BLACKBOARD_PEN_ITEM_SMOOTHING
    void appendPointSmoothing(const QPointF &point);
#endif

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &steam) override;
    virtual void readStream(QDataStream &steam) override;


    // IItemIndex interface
public:
    virtual QString id() const override;
    virtual void setId(const QString &id) override;
    virtual BbToolType toolType() const override;
    virtual BlackboardScene *scene() override;
};

#endif // CANVASELEMENTPEN3_HPP
