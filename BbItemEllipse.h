#ifndef BBITEMELLIPSE_H
#define BBITEMELLIPSE_H

#include "BbHeader.h"
#include <QGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemEllipseData;
class NSB_BLACKBOARD_EXPORT BbItemEllipse: public QGraphicsEllipseItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemEllipseData *_data;

    qreal _beginX, _beginY;

    qreal _dragX, _dragY;

    QPointF _mousePos;

    bool _circle = false;

    bool _editing = false;
public:
    BbItemEllipse();

    BbItemEllipse(BbItemData * data);

    ~BbItemEllipse() override;

    void init();

    void repaint() override;

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;

    void begin(const QPointF & point);

    void draw(const QPointF & point);

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
    BbItemData *data() override;
    void toolDown(const QPointF &pos) override;
    void toolDraw(const QPointF &pos) override;
    void toolDone(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void removed() override { }
    void added() override { }
    void absolutize() override;
    bool isEditing() override;
};

#endif // BBITEMELLIPSE_H
