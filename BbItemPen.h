#ifndef CANVASELEMENTPEN3_HPP
#define CANVASELEMENTPEN3_HPP

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "IStreamWR.h"
#include "IItemIndex.h"

#define LINE_SMOOTHING 0 // 1 开启线条圆滑模式，在一笔的轨迹过长的时候，会卡顿。
#define LINE_SMOOTHING_UNIT 20 // 开启线条圆滑后，圆滑曲线中每一小段直线的长度
#define SAVE_TO_PIXMAP_WHEN_DONE 0  // 1 以位图的形式保存一个书写轨迹，在放大缩小窗口时，会出现轨迹失真（锯齿）的情况。
class BbItemPenData;
class BbItemPen : public QGraphicsRectItem, public IStreamWR, public IItemIndex{

protected:
#if SAVE_TO_PIXMAP_WHEN_DONE
    QPixmap *_pixmap = nullptr;
#endif

#if LINE_SMOOTHING
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

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void straightLineDragging(const QPointF &point);

    void addPointToPath(const QPointF &point);

#if LINE_SMOOTHING
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
