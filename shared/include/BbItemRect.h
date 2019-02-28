#ifndef BBITEMRECT_H
#define BBITEMRECT_H

#include <QGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemRectData;
class NSB_BLACKBOARD_EXPORT BbItemRect: public QGraphicsRectItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemRectData *_myData;

    qreal _beginX, _beginY;

    qreal _dragX, _dragY;

    QPointF _mousePos;

    bool _square = false;
public:
    BbItemRect();

    BbItemRect(BbItemRectData * data);

    ~BbItemRect() override;

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

    void setSquare(const bool square);

    void toNinety(const QPointF &mousePos, qreal & outX,qreal & outY);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    // IItemIndex interface
public:
    QString id() const override;
    void setId(const QString &id) override;
    BbToolType toolType() const override;
    BlackboardScene *scene() override;
};

#endif // BBITEMRECT_H
