#include "BbItemEllipse.h"
#include "BbItemEllipseData.h"
#include "BlackboardScene.h"

#include <QPainter>

BbItemEllipse::BbItemEllipse():QGraphicsEllipseItem()
{
   _myData = new BbItemEllipseData();
   setPen(_myData->pen);
   setBrush(_myData->brush);
}

BbItemEllipse::BbItemEllipse(BbItemEllipseData *data):QGraphicsEllipseItem()
{
    _myData = data;
    setPen(_myData->pen);
    setBrush(_myData->brush);
}

BbItemEllipse::~BbItemEllipse()
{
    if(_myData){
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemEllipse::repaintWithItemData()
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
    setZValue(_myData->z);
    update();
}

void BbItemEllipse::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    _myData->z = zValue();
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

void BbItemEllipse::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    repaintWithItemData();
}

void BbItemEllipse::begin(const QPointF &point)
{
    _mousePos = point;
    _beginX = point.x();
    _beginY = point.y();
    setPos(point);
    _myData->updatePostion(this);
}
void BbItemEllipse::drag(const QPointF &point)
{
    _mousePos = point;
    if(!_circle)
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
    _myData->updatePostion(this);
    setRect(0,0,std::abs(_dragX-_beginX),std::abs(_dragY-_beginY));
}

void BbItemEllipse::done()
{
    // maybe we dont need "done".
}

void BbItemEllipse::setPenColor(const QColor &color)
{
    _myData->pen.setColor(color);
    setPen(_myData->pen);
}

void BbItemEllipse::setWeight(const qreal &weight)
{
    _myData->setWeight(weight);
    setPen(_myData->pen);
}

void BbItemEllipse::setBrushColor(const QColor &color)
{
    _myData->brush.setColor(color);
    setBrush(_myData->brush);
}

QSizeF BbItemEllipse::size()
{
    return rect().size();
}

QColor BbItemEllipse::penColor()
{
    return _myData->pen.color();
}

QColor BbItemEllipse::brushColor()
{
    return _myData->brush.color();
}

qreal BbItemEllipse::weight()
{
    return _myData->weight();
}

QPointF BbItemEllipse::beginPos()
{
    return QPointF(_beginX,_beginY);
}

QPointF BbItemEllipse::dragPos()
{
    return QPointF(_dragX,_dragY);
}

QString BbItemEllipse::id() const
{
    return _myData->lid;
}

void BbItemEllipse::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemEllipse::toolType() const
{
    return _myData->tooltype;
}

BlackboardScene *BbItemEllipse::scene()
{
    return dynamic_cast<BlackboardScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemEllipse::data()
{
    return _myData;
}

bool BbItemEllipse::square()
{
    return _circle;
}

void BbItemEllipse::setCircle(const bool circle)
{
    if(_circle == circle)
    {
        return;
    }
    _circle = circle;
    drag(_mousePos);
}

void BbItemEllipse::toNinety(const QPointF &point, qreal &outX, qreal &outY)
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

void BbItemEllipse::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    QGraphicsEllipseItem::paint(painter,option,widget);
}
