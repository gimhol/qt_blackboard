#ifndef BBITEMSTRAIGHT_H
#define BBITEMSTRAIGHT_H

#include <QGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemStraightData;
class NSB_BLACKBOARD_EXPORT BbItemStraight: public QGraphicsRectItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemStraightData * _myData;

    QRectF _rect;

    bool _fortyFive = false;

    QPointF _mousePos;
public:
    BbItemStraight();

    BbItemStraight(BbItemStraightData * dta);

    virtual ~BbItemStraight() override;

    void begin(const QPointF & point);
    void drag(const QPointF & point);
    void done();
    void repaintWithItemData();
    void setFortyFive(const bool & fortyFive);

    QPointF a();

    QPointF b();

    QColor color();

    void setColor(const QColor &color);

    qreal penWidth();

    qreal weight();

    void setPenWidth(qreal width);

    void setWeight(qreal weight);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    QPointF toFortyFive(const QPointF & point);

    void setupRectWithAB();

    // IStreamW interface
public:
    void writeStream(QDataStream &stream) override;

    // IStreamR interface
public:
    void readStream(QDataStream &stream) override;

    // IItemIndex interface
public:
    QString id() const override;
    void setId(const QString &id) override;
    BbToolType toolType() const override;
    BlackboardScene *scene() override;
};

#endif // BBITEMSTRAIGHT_H
