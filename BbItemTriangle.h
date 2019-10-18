#ifndef BBITEMTRIANGLE_H
#define BBITEMTRIANGLE_H


#include <QGraphicsItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemTriangleData;
class NSB_BLACKBOARD_EXPORT BbItemTriangle: public QGraphicsRectItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemTriangleData * _myData;

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

    QColor penColor();

    void setPenColor(const QColor &penColor);

    QColor brushColor();

    void setBrushColor(const QColor &color);

    qreal penWidth();

    qreal weight();

    void setWeight(qreal weight);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void setupRectWithABC();

    // IStreamW interface
public:
    void writeStream(QDataStream &stream) override;

    // IStreamR interface
public:
    void readStream(QDataStream &stream) override;

    // IItemIndex interface
public:
    QString id() override;
    void setId(const QString &id) override;
    BbToolType toolType() override;
    BbScene *bbScene() override;
    BbItemData *data() override;
    Blackboard *blackboard() override;
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


#endif // BBITEMTRIANGLE_H
