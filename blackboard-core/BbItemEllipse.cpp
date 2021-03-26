#include "BbItemEllipse.h"
#include "BbItemEllipseData.h"
#include "Blackboard.h"
#include "BbScene.h"
#include "BbItemInnerDataKey.h"
#include <QPainter>

BbItemEllipse::BbItemEllipse():
    QGraphicsRectItem(),
    _data(new BbItemEllipseData())
{
   init();
}

BbItemEllipse::BbItemEllipse(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemEllipseData*>(data))
{
    init();
}

BbItemEllipse::~BbItemEllipse()
{
    if(_data)
    {
        delete _data;
        _data = nullptr;
    }
}

void BbItemEllipse::init()
{
    Q_ASSERT(nullptr != _data);
    if(!_data)
    {
        _data = Blackboard::defaultFactory()->createItemData<BbItemEllipseData>(BBTT_Ellipse);
    }
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    setData(BBIIDK_ITEM_IS_SHAPE,true);
}

void BbItemEllipse::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    bool pointcut = modifiers & Qt::ShiftModifier;
    setPointcut(pointcut);

    bool circular = !pointcut && modifiers & Qt::ControlModifier;
    setCircular(circular);

    bool startFromCenter = !pointcut && !(modifiers & Qt::AltModifier);
    setStartFromCenter(startFromCenter);

    draw(_mousePos);
    emit blackboard()->itemChanged(BBIET_ellipseDraw,this);
}

void BbItemEllipse::absolutize()
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
        if(_data->size.isValid())
        {
            _data->size *= ratio;
        }
    }
}

bool BbItemEllipse::isEditing()
{
    return _editing;
}

void BbItemEllipse::repaint()
{
    qreal x = _data->x;
    qreal y = _data->y;
    QSizeF size = _data->size;
    setPos(x,y);
    setRect(0,0,size.width(),size.height());
    setSelected(false);
    setEnabled(true);
    setZValue(_data->z);
    update();
}

void BbItemEllipse::writeStream(QDataStream &stream)
{
    _data->x = x();
    _data->y = y();
    _data->z = zValue();
    _data->size = rect().size();
    _data->writeStream(stream);
}

void BbItemEllipse::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
}

void BbItemEllipse::begin(const QPointF &point)
{
    _editing = true;
    _mousePos = point;
    _beginX = point.x();
    _beginY = point.y();
    moveToPosition(point);
    updatePrevPosition();
}
void BbItemEllipse::draw(const QPointF &point)
{
    _mousePos = point;
    if(_circular){
        toCircular(point,_dragX,_dragY);
    }
    else{
        _dragX = point.x();
        _dragY = point.y();
    }
    if(_startFromCenter){
        _data->x = _beginX - std::abs(_dragX-_beginX);
        _data->y = _beginY - std::abs(_dragY-_beginY);
        _data->size.setWidth(2 * std::abs(_dragX-_beginX));
        _data->size.setHeight(2 * std::abs(_dragY-_beginY));

    }else if(_pointcut){
        qreal radius = std::sqrt(std::pow(std::abs(_dragX-_beginX),2)+
                                 std::pow(std::abs(_dragY-_beginY),2));
        _data->x = _beginX-radius;
        _data->y = _beginY-radius;
        _data->size.setWidth(2*radius);
        _data->size.setHeight(2*radius);
    }else{
        _data->x = std::min(_dragX,_beginX);
        _data->y = std::min(_dragY,_beginY);
        _data->size.setWidth(std::abs(_dragX-_beginX));
        _data->size.setHeight(std::abs(_dragY-_beginY));
    }
    _data->updatePrevPostion();
    setPos(_data->x,_data->y);
    setRect(0,0,_data->size.width(),_data->size.height());
    _data->empty = !rect().size().isEmpty();
}

void BbItemEllipse::done()
{
    _data->updatePostion(this);
    _data->updatePrevPostion();
    _data->updatePrevSize();
    _editing = false;
    update();
}

BbItemData *BbItemEllipse::data()
{
    return _data;
}

void BbItemEllipse::toolDown(const QPointF &pos)
{
    setId(blackboard()->factory()->makeItemId(toolType()));
    setZ(blackboard()->factory()->makeItemZ(toolType()));
    updatePrevZ();

    auto settings = blackboard()->toolSettings<BbItemEllipseData>(BBTT_Ellipse);
    _data->pen = settings->pen;
    _data->brush = settings->brush;
    _data->setWeight(settings->weight());

    begin(pos);
    auto modifiers = bbScene()->modifiers();

    bool pointcut = modifiers & Qt::ShiftModifier;
    setPointcut(pointcut);

    bool circular = !pointcut && modifiers & Qt::ControlModifier;
    setCircular(circular);

    bool startFromCenter = !pointcut && !(modifiers & Qt::AltModifier);
    setStartFromCenter(startFromCenter);

    bbScene()->setCurrentItem(this);
    emit blackboard()->itemChanged(BBIET_ellipseDown,this);
}

void BbItemEllipse::toolDraw(const QPointF &pos)
{
    draw(pos);
    emit blackboard()->itemChanged(BBIET_ellipseDraw,this);
}

void BbItemEllipse::toolDone(const QPointF &pos)
{
    Q_UNUSED(pos)
    done();
    emit blackboard()->itemChanged(BBIET_ellipseDone,this);
    bbScene()->unsetCurrentItem(this);
}

bool BbItemEllipse::isCircular()
{
    return _circular;
}

bool BbItemEllipse::isStartFromCenter()
{
    return _startFromCenter;
}

bool BbItemEllipse::isPointcut()
{
    return _pointcut;
}

void BbItemEllipse::setCircular(bool circle,bool drawImmediately)
{
    if(_circular == circle)
        return;
    _circular = circle;
    if(drawImmediately)
        draw(_mousePos);
}

void BbItemEllipse::setStartFromCenter(bool startFromCenter,bool drawImmediately)
{
    if(_startFromCenter == startFromCenter)
        return;
    _startFromCenter = startFromCenter;
    if(drawImmediately)
        draw(_mousePos);
}

void BbItemEllipse::setPointcut(bool pointcut, bool drawImmediately)
{
    if(_pointcut == pointcut)
        return;
    _pointcut = pointcut;
    if(drawImmediately)
        draw(_mousePos);
}

void BbItemEllipse::toCircular(const QPointF &point, qreal &outX, qreal &outY)
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
    painter->drawEllipse(r);

    if((isSelected() || _editing) && (w > 8 && h > 8)){ // 圆心
        painter->setBrush(_data->brush.color());
        painter->setPen(_data->pen.color());
        auto center = rect().center();
        painter->drawEllipse(center,3.0,3.0);
    }
    QGraphicsRectItem::paint(painter,option,widget); // 仅用于绘制选取的虚线。
}

QJsonObject BbItemEllipse::toJsonObject()
{
    _data->x = x();
    _data->y = y();
    _data->z = zValue();
    _data->size = rect().size();
    return _data->toJsonObject();
}

void BbItemEllipse::fromJsonObject(const QJsonObject &stream)
{
    _data->fromJsonObject(stream);
    absolutize();
    repaint();
}
