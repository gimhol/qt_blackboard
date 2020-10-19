#include "BbItemStraight.h"
#include "BbItemStraightData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <cmath>
#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

BbItemStraight::BbItemStraight():
    QGraphicsRectItem(),
    _data(new BbItemStraightData())
{
    init();
}

BbItemStraight::BbItemStraight(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemStraightData*>(data))
{
    init();
}

BbItemStraight::~BbItemStraight()
{
    if(_data)
    {
        delete _data;
        _data = nullptr;
    }
}

void BbItemStraight::init()
{
    if(!_data)
    {
        _data = Blackboard::defaultFactory()->createItemData<BbItemStraightData>(BBTT_Straight);
        _data = new BbItemStraightData();
    }
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}

void BbItemStraight::toolDown(const QPointF &pos)
{
    setId(blackboard()->factory()->makeItemId(toolType()));
    setZ(blackboard()->factory()->makeItemZ(toolType()));
    updatePrevZ();

    auto settings = blackboard()->toolSettings<BbItemStraightData>(BBTT_Straight);
    _data->pen = settings->pen;
    _data->setWeight(settings->weight());

    begin(pos);
    setFortyFive(bbScene()->modifiers()&Qt::ShiftModifier);
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
    Q_UNUSED(pos)
    done();
    emit blackboard()->itemChanged(BBIET_straightDone,this);
    bbScene()->unsetCurrentItem(this);
}

void BbItemStraight::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    if(_fortyFive != (modifiers & Qt::ShiftModifier))
    {
        setFortyFive(modifiers & Qt::ShiftModifier);
        emit blackboard()->itemChanged(BBIET_straightDraw,this);
    }
}

void BbItemStraight::absolutize()
{
    if(_data->mode == BbItemData::CM_PERCENTAGE)
    {
        _data->mode = BbItemData::CM_ABSOLUTE;
        qreal ratio = bbScene()->width() / 100;
        if(_data->isPositionValid())
        {
            _data->x *= ratio;
            _data->y *= ratio;
        }
        if(_data->isPrevPositionValid())
        {
            _data->prevX *= ratio;
            _data->prevY *= ratio;
        }
        if(!_data->a.isNull())
        {
            _data->a *= ratio;
        }
        if(!_data->b.isNull())
        {
            _data->b *= ratio;
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
    _data->a = point;
    _data->b = point;
    _data->empty = false;
    _mousePos = point;
    setupRectWithAB();
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

void BbItemStraight::draw(const QPointF &point)
{
    _mousePos = point;
    if(_fortyFive)
    {
        _data->b = toFortyFive(point);
    }
    else
    {
        _data->b = point;
    }
    setupRectWithAB();
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

void BbItemStraight::done()
{
    _data->updatePostion(this);
    _data->updatePrevPostion();
    _editing = false;
}

void BbItemStraight::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter,option,widget);
    if(!_data->empty)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        if(_data->a == _data->b)
        {
            qreal halfPenW = 0.5 * _data->pen.widthF();
            painter->setBrush(QBrush(_data->pen.color()));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(int(-halfPenW),
                                 int(-halfPenW),
                                 int(2*halfPenW),
                                 int(2*halfPenW));
        }
        else
        {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(_data->pen);
            painter->drawLine(_a,_b);
        }
    }
}

QPointF BbItemStraight::toFortyFive(const QPointF &point)
{
    QPointF vec2 = (_data->a - point);
    qreal degree = std::atan2(vec2.y(), vec2.x());
    if(abs(degree) <= 0.3925 || abs(degree) > 2.7475)
    {
        // horizontal
        vec2.setX(point.x());
        vec2.setY(_data->a.y());
    }
    else if(abs(degree) >= 1.1775 && abs(degree) <= 1.9625)
    {
        // vertical
        vec2.setY(point.y());
        vec2.setX(_data->a.x());
    }
    else if((degree >= 0.3925 && degree <= 1.1775) || (degree <= -1.9625 && degree >= -2.7475))
    {
        // left-up ~ right-down
        qreal b0 = _data->a.y() - _data->a.x();
        qreal b1 = point.y() + point.x();
        vec2.setY((b0 + b1) / 2);
        vec2.setX((b0 - b1) / -2);
    }
    else
    {
        // left-bottom ~ right-top
        qreal b0 = _data->a.y() + _data->a.x();
        qreal b1 = point.y() - point.x();
        vec2.setY((b0 + b1) / 2);
        vec2.setX((b1 - b0) / -2);
    }
    return vec2;
}

void BbItemStraight::setupRectWithAB()
{
    qreal halfPenW = 0.5 * _data->pen.widthF();
    qreal x = -halfPenW+std::min(_data->a.x(), _data->b.x());
    qreal y = -halfPenW+std::min(_data->a.y(), _data->b.y());
    qreal w = halfPenW+std::max(_data->a.x(), _data->b.x()) - x;
    qreal h = halfPenW+std::max(_data->a.y(), _data->b.y()) - y;
    setPos(x+halfPenW,y+halfPenW);
    setRect(-halfPenW,-halfPenW,w,h);
    _a = _data->a - pos();
    _b = _data->b - pos();
    update();
}

void BbItemStraight::repaint()
{
    setupRectWithAB();
    if( _data->isPositionValid())
    {
        setPos(_data->x,_data->y);
        _data->updatePrevPostion();
    }
    setZValue(_data->z);
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
    return _data->a;
}

QPointF BbItemStraight::b()
{
    return _data->b;
}

void BbItemStraight::writeStream(QDataStream &stream)
{
    _data->x = x();
    _data->y = y();
    _data->z = zValue();
    _data->writeStream(stream);
}

void BbItemStraight::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
}

QJsonObject BbItemStraight::toJsonObject()
{
    _data->x = x();
    _data->y = y();
    _data->z = zValue();
    return _data->toJsonObject();
}

void BbItemStraight::fromJsonObject(const QJsonObject &jobj)
{
    _data->fromJsonObject(jobj);
    absolutize();
    repaint();
}

BbItemData *BbItemStraight::data()
{
    return _data;
}
