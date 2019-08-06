#ifndef BBITEMIMAGE_H
#define BBITEMIMAGE_H

#include <QGraphicsPixmapItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemImageData;
class NSB_BLACKBOARD_EXPORT BbItemImage : public QGraphicsRectItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemImageData * _myData;
     // N = north, W = west, S = south, E = east
    enum StretchDirection {
        NW,W,SW,S,SE,E,NE,N,Invalid,
    };
    bool _stretching = false;
    StretchDirection _stretchDirection = Invalid;
    int _lastX; // 移动前的X坐标
    int _lastY; // 移动前的Y坐标
    int _lastW; // 移动前的宽度
    int _lastH; // 移动前的高度
    int _prevX; // 上一个X坐标
    int _prevY; // 上一个Y坐标
    int _prevW; // 上一个宽度
    int _prevH; // 上一个高度
public:
    BbItemImage();

    BbItemImage(BbItemData * data);

    virtual ~BbItemImage() override;
    void init();
    void setPixmap(const QPixmap &pixmap);
    void repaint() override;
    void writeStream(QDataStream &stream) override;
    void readStream(QDataStream &stream) override;
    void resize(qreal width, qreal height);
    void updatePrevSize();
    void begin(const QPointF &point);
    bool draw(const QPointF &point);
    bool done();
    QString id() const override;

    void setId(const QString &id) override;
    BbToolType toolType() const override;
    BbScene *scene() override;
    BbItemData *data() override;
    Blackboard *blackboard() override;
    bool mouseDown(const QPointF &pos) override;
    bool mouseMove(const QPointF &pos) override;
    bool mouseRelease(const QPointF &pos) override;
    bool doubleClicked(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void added() override {}
    void removed() override {}
    qreal z() override;
    void setZ(const qreal &value) override;
    void toAbsoluteCoords() override;

protected:
    void makeStretchControlDot(QRect *rects);
    StretchDirection stretchDirection(const QPointF &mousePos);
    void updateCursor(StretchDirection direction);
    void updateCursor(const QPointF &mousePos);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

};

#endif // BBITEMIMAGE_H
