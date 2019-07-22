#include "BbItemTriangle.h"
#include "BbItemTriangleData.h"
#include "BlackboardScene.h"

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

BbItemTriangle::BbItemTriangle():
    QGraphicsRectItem()
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    _myData = new BbItemTriangleData();
}

BbItemTriangle::BbItemTriangle(BbItemTriangleData *data):
    QGraphicsRectItem()
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    _myData = data;
}

BbItemTriangle::~BbItemTriangle()
{
    if(_myData)
    {
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemTriangle::begin(const QPointF &point)
{
    _myData->points[0] = point;
    _myData->points[1] = point;
    _myData->points[2] = point;
    _mousePos = point;
    _myData->isEmpty = false;

    setupRectWithABC();
    _myData->updatePostion(this);
}

void BbItemTriangle::drag(const QPointF &point)
{
    _mousePos = point;

    if(_step == 0)
    {
        _myData->points[1] = point;
    }
    else
    {
        _myData->points[2] = point;
    }
    setupRectWithABC();
    _myData->updatePostion(this);
}

void BbItemTriangle::done()
{
    ++_step;
}

void BbItemTriangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter,option,widget);
    if(!_myData->isEmpty)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(_myData->pen);
        painter->setRenderHint(QPainter::Antialiasing);

        if(_myData->points[0] == _myData->points[1])  // 一个点。
        {
            qreal halfPenW = 0.5 * _myData->pen.widthF();
            painter->setBrush(QBrush(_myData->pen.color()));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(int(halfPenW),
                                 int(halfPenW),
                                 int(2*halfPenW),
                                 int(2*halfPenW));
        }
        else if(_myData->points[1] == _myData->points[2]) // 两个点。
        {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(_myData->pen);
            painter->drawLine(_points[0],_points[1]);
        }
        else // 三个点。
        {
            painter->setBrush(_myData->brush);
            painter->setPen(_myData->pen);
            painter->drawPolygon(_points,3);
        }
    }
}

void BbItemTriangle::setupRectWithABC()
{
    qreal halfPenW = 0.5 * _myData->pen.widthF();

    qreal minx = _myData->points[0].x();
    qreal maxx = _myData->points[0].x();
    qreal miny = _myData->points[0].y();
    qreal maxy = _myData->points[0].y();
    for(int i=1;i<3;++i)
    {
        if(minx > _myData->points[i].x())
        {
            minx = _myData->points[i].x();
        }
        if(maxx < _myData->points[i].x())
        {
            maxx = _myData->points[i].x();
        }
        if(miny > _myData->points[i].y())
        {
            miny = _myData->points[i].y();
        }
        if(maxy < _myData->points[i].y())
        {
            maxy = _myData->points[i].y();
        }
    }
    qreal x = -halfPenW + minx;
    qreal y = -halfPenW + miny;
    qreal w = halfPenW + maxx - x;
    qreal h = halfPenW + maxy - y;

    setPos(x+halfPenW,y+halfPenW);
    setRect(-halfPenW,-halfPenW,w,h);
    for(int i=0;i<3;++i)
    {
        _points[i] = _myData->points[i] - pos();
    }
    update();
}

void BbItemTriangle::repaintWithItemData()
{
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal width = scene()->width();
        qreal ratio = width / 100;
        _myData->points[0] *= ratio;
        _myData->points[1] *= ratio;
        _myData->points[2] *= ratio;
    }
    setupRectWithABC();
    if( _myData->isPositionValid() )
    {
        qreal x = _myData->x;
        qreal y = _myData->y;
        if(_myData->mode == BbItemData::CM_PERCENTAGE)
        {
            qreal width = scene()->width();
            qreal ratio = width / 100;
            x *= ratio;
            y *= ratio;
        }
        setPos(x,y);
    }
    setZValue(_myData->z);
    update();
}

unsigned char BbItemTriangle::step()
{
    return _step;
}

QPointF BbItemTriangle::point(int index)
{
    return _myData->points[index];
}

QColor BbItemTriangle::penColor(){
    return _myData->pen.color();
}

void BbItemTriangle::setPenColor(const QColor &color){
    _myData->pen.setColor(color);
}

QColor BbItemTriangle::brushColor(){
    return _myData->brush.color();
}

void BbItemTriangle::setBrushColor(const QColor &color){
    _myData->brush.setColor(color);
}

qreal BbItemTriangle::penWidth()
{
    return _myData->pen.widthF();
}

qreal BbItemTriangle::weight()
{
    return _myData->weight();
}

void BbItemTriangle::setPenWidth(qreal width)
{
    _myData->pen.setWidthF(width);
}

void BbItemTriangle::setWeight(qreal weight)
{
    _myData->setWeight(weight);
}

void BbItemTriangle::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    _myData->z = zValue();
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _myData->x /= ratio;
        _myData->y /= ratio;
    }
    _myData->writeStream(stream);
}

void BbItemTriangle::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    repaintWithItemData();
}

QString BbItemTriangle::id() const
{
    return _myData->lid;
}

void BbItemTriangle::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemTriangle::toolType() const
{
    return _myData->tooltype;
}

BlackboardScene *BbItemTriangle::scene()
{
    return dynamic_cast<BlackboardScene *>(QGraphicsRectItem::scene());
}

BbItemData *BbItemTriangle::data()
{
    return _myData;
}
