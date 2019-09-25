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
    QPointF _stretchOffset;
    qreal _lastX; // 移动前的X坐标
    qreal _lastY; // 移动前的Y坐标
    qreal _lastW; // 移动前的宽度
    qreal _lastH; // 移动前的高度
    qreal _prevX; // 上一个X坐标
    qreal _prevY; // 上一个Y坐标
    qreal _prevW; // 上一个宽度
    qreal _prevH; // 上一个高度
    qreal _controlDotSize = 10;
    bool _clampRatio = false; // 原比例缩放
    QPointF _mousePos; // 记录鼠标位置。
    bool _mouseDown = false;
    qreal _progress = 100;
    QString _text;
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
    bool clicked(const QPointF &pos) override;
    bool doubleClicked(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void added() override {}
    void removed() override {}
    qreal z() override;
    void setZ(const qreal &value) override;
    void toAbsoluteCoords() override;
    qreal minWidth();
    qreal minHeight();
    void setClampRatio(bool clampRatio);
    void setProgress(qreal progress);
    void setText(QString text);
    QString url();
    QString path();
    void setUrl(QString url);
    void setPath(QString path);
    bool clampRatio();
protected:
    void makeStretchControlDot(QRectF *rects);
    StretchDirection stretchDirection(const QPointF &mousePos);
    QPointF stretchOffset(const QPointF &mousePos);
    void updateCursor(StretchDirection direction);
    void updateCursor(const QPointF &mousePos);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

};

#endif // BBITEMIMAGE_H
