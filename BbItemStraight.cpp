#include "BbItemStraight.h"
#include "BbItemStraightData.h"
#include "BlackboardScene.h"

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

BbItemStraight::BbItemStraight():
    QGraphicsRectItem()
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    _myData = new BbItemStraightData();
}

BbItemStraight::BbItemStraight(BbItemStraightData *data):
    QGraphicsRectItem()
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    _myData = data;
}

BbItemStraight::~BbItemStraight()
{
    if(_myData)
    {
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemStraight::begin(const QPointF &point)
{
    _myData->a = point;
    _myData->b = point;
    _mousePos = point;
    _myData->isEmpty = false;

    setupRectWithAB();
}

void BbItemStraight::drag(const QPointF &point)
{
    _mousePos = point;
    if(_fortyFive)
    {
        _myData->b = toFortyFive(point);
    }
    else
    {
        _myData->b = point;
    }
    setupRectWithAB();
}

void BbItemStraight::done()
{
}

void BbItemStraight::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter,option,widget);
    if(!_myData->isEmpty)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(_myData->pen);
        painter->setRenderHint(QPainter::HighQualityAntialiasing);

        if(_myData->a == _myData->b)
        {
            qreal halfPenW = 0.5 * _myData->pen.widthF();
            painter->setBrush(QBrush(_myData->pen.color()));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(_myData->a.x() - halfPenW,
                                 _myData->a.y() - halfPenW,
                                 2*halfPenW,
                                 2*halfPenW);
        }
        else
        {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(_myData->pen);
            painter->drawLine(_myData->a,_myData->b);
        }
    }
}

QPointF BbItemStraight::toFortyFive(const QPointF &point)
{
    QPointF vec2 = (_myData->a - point);
    qreal degree = std::atan2(vec2.y(), vec2.x());
    if(abs(degree) <= 0.3925 || abs(degree) > 2.7475)
    {
        // horizontal
        vec2.setX(point.x());
        vec2.setY(_myData->a.y());
    }
    else if(abs(degree) >= 1.1775 && abs(degree) <= 1.9625)
    {
        // vertical
        vec2.setY(point.y());
        vec2.setX(_myData->a.x());
    }
    else if((degree >= 0.3925 && degree <= 1.1775) || (degree <= -1.9625 && degree >= -2.7475))
    {
        // left-up ~ right-down
        qreal b0 = _myData->a.y() - _myData->a.x();
        qreal b1 = point.y() + point.x();
        vec2.setY((b0 + b1) / 2);
        vec2.setX((b0 - b1) / -2);
    }
    else
    {
        // left-bottom ~ right-top
        qreal b0 = _myData->a.y() + _myData->a.x();
        qreal b1 = point.y() - point.x();
        vec2.setY((b0 + b1) / 2);
        vec2.setX((b1 - b0) / -2);
    }
    return vec2;
}

void BbItemStraight::setupRectWithAB()
{
    qreal halfPenW = 0.5 * _myData->pen.widthF();
    qreal x = -halfPenW+std::min(_myData->a.x()-pos().x(), _myData->b.x()-pos().x());
    qreal y = -halfPenW+std::min(_myData->a.y()-pos().y(), _myData->b.y()-pos().y());
    qreal w = halfPenW+std::max(_myData->a.x()-pos().x(), _myData->b.x()-pos().x()) - x;
    qreal h = halfPenW+std::max(_myData->a.y()-pos().y(), _myData->b.y()-pos().y()) - y;
    setRect(x,y,w,h);
}

void BbItemStraight::repaintWithItemData()
{
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal width = scene()->width();
        qreal ratio = width / 100;
        _myData->a *= ratio;
        _myData->b *= ratio;
    }
    setupRectWithAB();
    if( _myData->x > -9998 && _myData->y > -9998 )
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
    update();
}

void BbItemStraight::setFortyFive(const bool &fortyFive)
{
    if(_fortyFive == fortyFive)
    {
        return;
    }
    _fortyFive = fortyFive;
    if(_fortyFive)
    {
        _myData->b = toFortyFive(_myData->b);
    }
    else
    {
        _myData->b = _mousePos;
    }
    update();
}

QPointF BbItemStraight::a()
{
    return _myData->a;
}

QPointF BbItemStraight::b()
{
    return _myData->b;
}

QColor BbItemStraight::color(){
    return _myData->pen.color();
}

void BbItemStraight::setColor(const QColor &color){
    _myData->pen.setColor(color);
}

qreal BbItemStraight::penWidth()
{
    return _myData->pen.widthF();
}

qreal BbItemStraight::weight()
{
    return (_myData->pen.widthF() - BbItemStraightData::minWidth)  / (BbItemStraightData::maxWidth - BbItemStraightData::minWidth);
}

void BbItemStraight::setPenWidth(qreal width)
{
    _myData->pen.setWidthF(width);
}

void BbItemStraight::setWeight(qreal weight)
{
    setPenWidth(BbItemStraightData::minWidth + weight * (BbItemStraightData::maxWidth - BbItemStraightData::minWidth));
}

void BbItemStraight::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _myData->x /= ratio;
        _myData->y /= ratio;
    }
    _myData->writeStream(stream);
}

void BbItemStraight::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    repaintWithItemData();
}

QString BbItemStraight::id() const
{
    return _myData->lid;
}

void BbItemStraight::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemStraight::toolType() const
{
    return _myData->tooltype;
}

BlackboardScene *BbItemStraight::scene()
{
    return dynamic_cast<BlackboardScene *>(QGraphicsRectItem::scene());
}
