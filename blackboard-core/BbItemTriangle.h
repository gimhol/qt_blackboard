#ifndef BBITEMTRIANGLE_H
#define BBITEMTRIANGLE_H


#include <QGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemTriangleData;
class NSB_BLACKBOARD_EXPORT BbItemTriangle:
        public QGraphicsRectItem,
        public IItemIndex,
        public IStreamWR,
        public IJsonWR
{
    BB_HIDE_POS_SETTER
protected:
    BbItemTriangleData * _data;

    QRectF _rect;

    QPointF _mousePos;

    unsigned char _step = 0;

    QPointF _points[3];

    bool _editing = false;
public:
    BbItemTriangle();

    BbItemTriangle(BbItemData * data);

    virtual ~BbItemTriangle() override;

    void init();

    void begin(const QPointF & point);
    void draw(const QPointF & point);
    void done();
    void repaint() override;

    unsigned char step();

    QPointF point(int index);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void setupRectWithABC();

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
    void absolutize() override;
    bool isEditing() override;
};


#endif // BBITEMTRIANGLE_H
