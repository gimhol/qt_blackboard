#include "BbItemRect.h"
#include "BbItemRectData.h"
#include "BlackboardScene.h"

BbItemRect::BbItemRect():QGraphicsRectItem()
{
   _myData = new BbItemRectData();
   setPen(_myData->pen);
   setBrush(_myData->brush);
}

BbItemRect::BbItemRect(BbItemRectData *data):QGraphicsRectItem()
{
    _myData = data;
    setPen(_myData->pen);
    setBrush(_myData->brush);
}

BbItemRect::~BbItemRect()
{
    if(_myData){
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemRect::repaintWithItemData()
{
    setPen(_myData->pen);
    setBrush(_myData->brush);
    qreal x = _myData->x;
    qreal y = _myData->y;
    QSizeF size = _myData->size;
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal width = scene()->width();
        qreal ratio = width / 100;
        x *= ratio;
        y *= ratio;
        size *= ratio;
    }
    setPos(x,y);
    setRect(0,0,size.width(),size.height());
    setSelected(false);
    setEnabled(true);
    update();
}

void BbItemRect::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    _myData->size = rect().size();
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _myData->x /= ratio;
        _myData->y /= ratio;
        _myData->size /= ratio;
    }
    _myData->writeStream(stream);
}

void BbItemRect::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    repaintWithItemData();
}

void BbItemRect::begin(const QPointF &point)
{
    _mousePos = point;
    _beginX = point.x();
    _beginY = point.y();
    setPos(point);
}
void BbItemRect::drag(const QPointF &point)
{
    _mousePos = point;
    if(!_square)
    {
        _dragX = point.x();
        _dragY = point.y();
    }
    else
    {
        toNinety(point,_dragX,_dragY);
    }
    qreal l = std::min(_dragX,_beginX);
    qreal t = std::min(_dragY,_beginY);
    setPos(l,t);
    setRect(0,0,std::abs(_dragX-_beginX),std::abs(_dragY-_beginY));
}

void BbItemRect::done()
{
    // maybe we dont need "done".
}

void BbItemRect::setPenColor(const QColor &color)
{
    _myData->pen.setColor(color);
    setPen(_myData->pen);
}

void BbItemRect::setWeight(const qreal &weight)
{
    _myData->setWeight(weight);
    setPen(_myData->pen);
}

void BbItemRect::setBrushColor(const QColor &color)
{
    _myData->brush.setColor(color);
    setBrush(_myData->brush);
}

QSizeF BbItemRect::size()
{
    return rect().size();
}

QColor BbItemRect::penColor()
{
    return _myData->pen.color();
}

QColor BbItemRect::brushColor()
{
    return _myData->brush.color();
}

qreal BbItemRect::weight()
{
    return _myData->weight();
}

QPointF BbItemRect::beginPos()
{
    return QPointF(_beginX,_beginY);
}

QPointF BbItemRect::dragPos()
{
    return QPointF(_dragX,_dragY);
}

QString BbItemRect::id() const
{
    return _myData->lid;
}

void BbItemRect::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemRect::toolType() const
{
    return _myData->tooltype;
}

BlackboardScene *BbItemRect::scene()
{
    return dynamic_cast<BlackboardScene *>(QGraphicsItem::scene());
}

bool BbItemRect::square()
{
    return _square;
}

void BbItemRect::setSquare(const bool square)
{
    if(_square == square)
    {
        return;
    }
    _square = square;
    drag(_mousePos);
}

void BbItemRect::toNinety(const QPointF &point, qreal &outX, qreal &outY)
{
    QPointF vec2(_beginX-point.x(),_beginY-point.y());
    qreal degree = std::atan2(vec2.y(), vec2.x());

    if((degree >= 0 && degree < 1.5707) || degree <= -1.5707)
    {
        // left-up ~ right-down
        qreal b0 = _beginY - _beginX;
        qreal b1 = point.y() + point.x();
        outY = (b0 + b1) / 2;
        outX = (b0 - b1) / -2;
    }
    else
    {
        // left-bottom ~ right-top
        qreal b0 = _beginY + _beginX;
        qreal b1 = point.y() - point.x();
        outY = (b0 + b1) / 2;
        outX = (b1 - b0) / -2;
    }
}
