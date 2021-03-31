#include "BbItemRect.h"
#include "BbItemRectData.h"
#include "Blackboard.h"
#include "BbScene.h"
#include "BbItemInnerDataKey.h"
#include <QPainter>

BbItemRect::BbItemRect():
    QGraphicsRectItem(),
    _data(new BbItemRectData())
{
   init();
}

BbItemRect::BbItemRect(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemRectData*>(data))
{
    init();
}

BbItemRect::~BbItemRect()
{
    if(_data){
        delete _data;
        _data = nullptr;
    }
}

void BbItemRect::init()
{
    Q_ASSERT(nullptr != _data);
    if(!_data)
    {
        _data = Blackboard::defaultFactory()->createItemData<BbItemRectData>(BBTT_Rectangle);
    }
    setId(_data->lid);
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    setData(BBIIDK_ITEM_IS_SHAPE,true);
}

void BbItemRect::repaint()
{
    setPos(_data->x, _data->y);
    setRect(0,0,_data->width,_data->height);
    setSelected(false);
    setEnabled(true);
    setZValue(_data->z);
    update();
}

void BbItemRect::writeStream(QDataStream &stream)
{
    _data->x = x();
    _data->y = y();
    _data->z = z();
    _data->width = rect().width();
    _data->height = rect().height();
    _data->writeStream(stream);
}

void BbItemRect::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
}

void BbItemRect::begin(const QPointF &point)
{
    _editing = true;
    _mousePos = point;
    _beginX = point.x();
    _beginY = point.y();
    setPos(point);
    _data->fixPostion(this);
}
void BbItemRect::draw(const QPointF &point)
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
    _data->updatePostion(this);
    setRect(0,0,
            std::abs(_dragX-_beginX),
            std::abs(_dragY-_beginY));
    _data->width = (std::abs(_dragX-_beginX));
    _data->height = (std::abs(_dragY-_beginY));
    _data->fixPostion(this);
    _data->empty = !rect().size().isEmpty();
}

void BbItemRect::done()
{
    _data->fixPostion(this);
    _data->updatePrevSize();
    _editing = false;
    update();
}

BbItemData *BbItemRect::data()
{
    return _data;
}

void BbItemRect::toolDown(const QPointF &pos)
{
    setId(blackboard()->factory()->makeItemId(toolType()));
    setZ(blackboard()->factory()->makeItemZ(toolType()));
    updatePrevZ();

    auto settings = blackboard()->toolSettings<BbItemRectData>(BBTT_Rectangle);
    _data->pen = settings->pen;
    _data->brush = settings->brush;
    _data->setWeight(settings->weight());

    bbScene()->setCurrentItem(this);
    begin(pos);
    setSquare(bbScene()->modifiers() & Qt::ShiftModifier);
    emit blackboard()->itemChanged(BBIET_rectDown,this);
}

void BbItemRect::toolDraw(const QPointF &pos)
{
    draw(pos);
    emit blackboard()->itemChanged(BBIET_rectDraw,this);
}

void BbItemRect::toolUp(const QPointF &pos)
{
    Q_UNUSED(pos)
    done();
    emit blackboard()->itemChanged(BBIET_rectDone,this);
    bbScene()->unsetCurrentItem(this);
}

void BbItemRect::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    if(_square != (modifiers == Qt::ShiftModifier))
    {
        setSquare(modifiers == Qt::ShiftModifier);
        emit blackboard()->itemChanged(BBIET_rectDraw,this);
    }
}

void BbItemRect::absolutize()
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
        if(_data->isSizeValid())
        {
            _data->width *= ratio;
            _data->height *= ratio;
        }
    }
}

bool BbItemRect::isEditing()
{
    return _editing;
}

QJsonObject BbItemRect::toJsonObject()
{
    _data->x = x();
    _data->y = y();
    _data->z = z();
    _data->width = rect().width();
    _data->height = rect().height();
    return _data->toJsonObject();
}

void BbItemRect::fromJsonObject(const QJsonObject &jobj)
{
    _data->fromJsonObject(jobj);
    absolutize();
    repaint();
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
    draw(_mousePos);
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

void BbItemRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto penW = _data->pen.widthF();
    auto w = rect().width();
    auto h = rect().height();
    QRectF r;
    painter->setRenderHint(QPainter::Antialiasing, true);
    if(w > penW*2 && h > penW*2){
        painter->setPen(_data->pen);
        painter->setBrush(_data->brush);
        r.setRect(penW/2, penW/2, w-penW, h-penW);
    }else{
        painter->setPen(Qt::NoPen);
        painter->setBrush(_data->pen.color());
        r.setRect(0,0,w,h);
    }
    painter->drawRect(r);

    QGraphicsRectItem::paint(painter,option,widget); // 仅用于绘制选取的虚线。
}


