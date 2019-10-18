#include "BbItemStraight.h"
#include "BbItemStraightData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QDateTime>

BbItemStraight::BbItemStraight():
    QGraphicsRectItem(),
    _myData(new BbItemStraightData())
{
    init();
}

BbItemStraight::BbItemStraight(BbItemData *data):
    QGraphicsRectItem(),
    _myData(dynamic_cast<BbItemStraightData*>(data))
{
    init();
}

BbItemStraight::~BbItemStraight()
{
    if(_myData)
    {
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemStraight::init()
{
    if(!_myData)
    {
        _myData = new BbItemStraightData();
    }
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}

Blackboard *BbItemStraight::blackboard()
{
    return bbScene()->blackboard();
}

void BbItemStraight::toolDown(const QPointF &pos)
{
    setZValue(QDateTime::currentMSecsSinceEpoch());

    auto settings = blackboard()->toolSettings<BbItemStraightData>(BBTT_Straight);
    setColor(settings->pen.color());
    setWeight(settings->weight());

    begin(pos);
    setId(bbScene()->generatItemId());
    setFortyFive(bbScene()->onlyShiftDown());
    bbScene()->setCurrentItem(this);
    emit blackboard()->itemChanged(BBIET_straightDown,this);
}

void BbItemStraight::toolDraw(const QPointF &pos)
{
    draw(pos);
    emit blackboard()->itemChanged(BBIET_straightDraw,this);
}

void BbItemStraight::toolDone(const QPointF &pos)
{
    Q_UNUSED(pos);
    done();
    emit blackboard()->itemChanged(BBIET_straightDone,this);
    bbScene()->unsetCurrentItem(this);
}

void BbItemStraight::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    if(_fortyFive != (modifiers == Qt::ShiftModifier))
    {
        setFortyFive(modifiers == Qt::ShiftModifier);
        emit blackboard()->itemChanged(BBIET_straightDraw,this);
    }
}

qreal BbItemStraight::z()
{
    return zValue();
}

void BbItemStraight::setZ(const qreal &value)
{
    setZValue(value);
    _myData->z = value;
}

void BbItemStraight::absolutize()
{
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        _myData->mode = BbItemData::CM_ABSOLUTE;
        qreal ratio = bbScene()->width() / 100;
        if(_myData->isPositionValid())
        {
            _myData->x *= ratio;
            _myData->y *= ratio;
        }
        if(_myData->isPrevPositionValid())
        {
            _myData->prevX *= ratio;
            _myData->prevY *= ratio;
        }
        if(!_myData->a.isNull())
        {
            _myData->a *= ratio;
        }
        if(!_myData->b.isNull())
        {
            _myData->b *= ratio;
        }
    }
}

bool BbItemStraight::isEditing()
{
    return _editing;
}

void BbItemStraight::begin(const QPointF &point)
{
    _editing = true;
    setPos(point);
    _myData->a = point;
    _myData->b = point;
    _myData->empty = false;
    _mousePos = point;
    setupRectWithAB();
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
}

void BbItemStraight::draw(const QPointF &point)
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
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
}

void BbItemStraight::done()
{
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
    _editing = false;
}

void BbItemStraight::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter,option,widget);
    if(!_myData->empty)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(_myData->pen);
        painter->setRenderHint(QPainter::Antialiasing);

        if(_myData->a == _myData->b)
        {
            qreal halfPenW = 0.5 * _myData->pen.widthF();
            painter->setBrush(QBrush(_myData->pen.color()));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(int(halfPenW),
                                 int(halfPenW),
                                 int(2*halfPenW),
                                 int(2*halfPenW));
        }
        else
        {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(_myData->pen);
            painter->drawLine(_a,_b);
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
    qreal x = -halfPenW+std::min(_myData->a.x(), _myData->b.x());
    qreal y = -halfPenW+std::min(_myData->a.y(), _myData->b.y());
    qreal w = halfPenW+std::max(_myData->a.x(), _myData->b.x()) - x;
    qreal h = halfPenW+std::max(_myData->a.y(), _myData->b.y()) - y;
    setPos(x+halfPenW,y+halfPenW);
    setRect(-halfPenW,-halfPenW,w,h);
    _a = _myData->a - pos();
    _b = _myData->b - pos();
    update();
}

void BbItemStraight::repaint()
{
    setupRectWithAB();
    if( _myData->isPositionValid())
    {
        setPos(_myData->x,_myData->y);
        _myData->updatePrevPostion();
    }
    setZValue(_myData->z);
    update();
}

void BbItemStraight::setFortyFive(const bool &fortyFive)
{
    if(_fortyFive == fortyFive)
    {
        return;
    }
    _fortyFive = fortyFive;
    draw(_mousePos);
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
    return _myData->weight();
}

void BbItemStraight::setWeight(qreal weight)
{
    _myData->setWeight(weight);
}

void BbItemStraight::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    _myData->z = zValue();
    _myData->writeStream(stream);
}

void BbItemStraight::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    absolutize();
    repaint();
}

QString BbItemStraight::id()
{
    return _myData->lid;
}

void BbItemStraight::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemStraight::toolType()
{
    return _myData->tooltype;
}

BbScene *BbItemStraight::bbScene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemStraight::data()
{
    return _myData;
}
