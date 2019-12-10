#ifndef BBITEMIMAGE_H
#define BBITEMIMAGE_H

#include <QGraphicsPixmapItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemImageData;
class NSB_BLACKBOARD_EXPORT BbItemImage :public QGraphicsRectItem, public IStreamWR, public IItemIndex
{
    BB_HIDE_POS_SETTER
public:
    // N = north, W = west, S = south, E = east
    enum Direction
    {
       NW,W,SW,S,SE,E,NE,N,Invalid,
    };

    BbItemImage();
    BbItemImage(BbItemData * data);
    virtual ~BbItemImage() override;
    qreal minWidth();
    qreal minHeight();
    bool ratioLock();
    void setRatioLock(bool ratioLock);
    void setPixmap(const QPixmap &pixmap);
    void setProgress(qreal progress);
    void setText(QString text);
    qreal ratio();
    QString url();
    QString path();
    void setUrl(QString url);
    void setPath(QString path);
    Direction stretchDirection(const QPointF &mousePos);
    QPointF stretchOffset(const QPointF &mousePos);
    void updateDots(QRectF *rects);
    void updateCursor(Direction direction);
    void updateCursor(const QPointF &mousePos);
    void resize(qreal width, qreal height);
    void updatePrevSize();
    void begin(const QPointF &point);
    bool draw(const QPointF &point);
    bool done();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

public: // IStreamWR
    void writeStream(QDataStream &stream) override;
    void readStream(QDataStream &stream) override;

public: //IItemIndex
    BbItemData *data() override;
    void repaint() override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void added() override;
    void absolutize() override;
    void init();

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    BbItemImageData * _data = nullptr;
    bool _stretching = false;
    Direction _direction = Invalid;
    QPointF _stretchOffset;
    qreal _lastX = 0; // 移动前的X坐标
    qreal _lastY = 0; // 移动前的Y坐标
    qreal _lastW = 0; // 移动前的宽度
    qreal _lastH = 0; // 移动前的高度
    qreal _dotSize = 5;
    bool _ratioLock = false; // 原比例缩放
    bool _centerLock = false; // 中心为原点缩放
    QPointF _mousePos; // 记录鼠标位置。
    bool _mouseDown = false;
    qreal _progress = 100;
    QString _text;
};

#endif // BBITEMIMAGE_H
