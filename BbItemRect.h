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

    bool _editing = false;
public:
    BbItemRect();

    BbItemRect(BbItemData *data);

    ~BbItemRect() override;

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

    void setSquare(const bool square);

    void toNinety(const QPointF &mousePos, qreal & outX,qreal & outY);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    // IItemIndex interface
public:
    QString id() const override;
    void setId(const QString &id) override;
    BbToolType toolType() const override;
    Blackboard *blackboard() override;
    BbScene *scene() override;
    BbItemData *data() override;
    void toolDown(const QPointF &pos) override;
    void toolDraw(const QPointF &pos) override;
    void toolDone(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void removed() override { }
    void added() override { }
    qreal z() override;
    void setZ(const qreal &value) override;
    void absolutize() override;
    bool isEditing() override;
};

#endif // BBITEMRECT_H
