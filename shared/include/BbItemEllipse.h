#ifndef BBITEMELLIPSE_H
#define BBITEMELLIPSE_H

#include "BlackboardHeader.h"
#include <QGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemEllipseData;
class NSB_BLACKBOARD_EXPORT BbItemEllipse: public QGraphicsEllipseItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemEllipseData *_myData;

    qreal _beginX, _beginY;

    qreal _dragX, _dragY;

    QPointF _mousePos;

    bool _circle = false;
public:
    BbItemEllipse();

    BbItemEllipse(BbItemEllipseData * data);

    ~BbItemEllipse() override;

    void repaintWithItemData();

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;

    void begin(const QPointF & point);

    void drag(const QPointF & point);

    void done();

    void setPenColor(const QColor & color);

    void setWeight(const qreal & weight);

    void setBrushColor(const QColor & color);

    QSizeF size();

    QColor penColor();

    QColor brushColor();

    qreal weight();

    QPointF beginPos();

    QPointF dragPos();

    bool square();

    void setCircle(const bool circle);

    void toNinety(const QPointF &mousePos, qreal & outX,qreal & outY);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // IItemIndex interface
public:
    QString id() const override;
    void setId(const QString &id) override;
    BbToolType toolType() const override;
    BlackboardScene *scene() override;
};

#endif // BBITEMELLIPSE_H
