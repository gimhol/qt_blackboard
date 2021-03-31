#ifndef BBITEMSTRAIGHT_H
#define BBITEMSTRAIGHT_H

#include <QGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemStraightData;
class NSB_BLACKBOARD_EXPORT BbItemStraight:
        public QGraphicsRectItem,
        public IItemIndex,
        public IStreamWR,
        public IJsonWR
{
    BB_HIDE_POS_SETTER
protected:
    BbItemStraightData * _data;

    QRectF _rect;

    bool _fortyFive = false;

    QPointF _mousePos;
    QPointF _a;
    QPointF _b;
    bool _editing = false;
public:
    BbItemStraight();

    BbItemStraight(BbItemData *data);

    virtual ~BbItemStraight() override;

    void init();

    void begin(const QPointF & point);
    void draw(const QPointF & point);
    void done();
    void repaint() override;
    void setFortyFive(const bool & fortyFive);

    QPointF a();

    QPointF b();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    QPointF toFortyFive(const QPointF & point);

    void setupRectWithAB();

    // IStreamWR interface
public:
    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;

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

#endif // BBITEMSTRAIGHT_H
