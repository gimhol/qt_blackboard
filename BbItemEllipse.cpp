#include "BbItemEllipse.h"
#include "BbItemEllipseData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <QDateTime>
#include <QPainter>

BbItemEllipse::BbItemEllipse():
    QGraphicsEllipseItem(),
    _data(new BbItemEllipseData())
{
   init();
}

BbItemEllipse::BbItemEllipse(BbItemData *data):
    QGraphicsEllipseItem(),
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
        _data = new BbItemEllipseData();
    }
    setPen(_data->pen);
    setBrush(_data->brush);
}

void BbItemEllipse::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    if(_circle != (modifiers == Qt::ShiftModifier))
    {
        setCircle(modifiers == Qt::ShiftModifier);
        emit blackboard()->itemChanged(BBIET_ellipseDraw,this);
    }
}

qreal BbItemEllipse::z()
{
    return zValue();
}

void BbItemEllipse::setZ(const qreal &value)
{
    setZValue(value);
    _data->z = value;
}

void BbItemEllipse::absolutize()
{
    if(_data->mode == BbItemData::CM_PERCENTAGE)
    {
        _data->mode = BbItemData::CM_ABSOLUTE;
        qreal ratio = scene()->width() / 100;
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
    setPen(_data->pen);
    setBrush(_data->brush);
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
    setPos(point);
    _data->updatePostion(this);
    _data->updatePrevPostion();
}
void BbItemEllipse::draw(const QPointF &point)
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
    setRect(0,0,std::abs(_dragX-_beginX),std::abs(_dragY-_beginY));
    _data->updatePostion(this);
    _data->updatePrevPostion();
    _data->empty = !size().isEmpty();
}

void BbItemEllipse::done()
{
    _data->updatePostion(this);
    _data->updatePrevPostion();
    _editing = false;
}

void BbItemEllipse::setPenColor(const QColor &color)
{
    _data->pen.setColor(color);
    setPen(_data->pen);
}

void BbItemEllipse::setWeight(const qreal &weight)
{
    _data->setWeight(weight);
    setPen(_data->pen);
}

void BbItemEllipse::setBrushColor(const QColor &color)
{
    _data->brush.setColor(color);
    setBrush(_data->brush);
}

QSizeF BbItemEllipse::size()
{
    return rect().size();
}

QColor BbItemEllipse::penColor()
{
    return _data->pen.color();
}

QColor BbItemEllipse::brushColor()
{
    return _data->brush.color();
}

qreal BbItemEllipse::weight()
{
    return _data->weight();
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
    return _data->lid;
}

void BbItemEllipse::setId(const QString &id)
{
    _data->lid = id;
}

BbToolType BbItemEllipse::toolType() const
{
    return _data->tooltype;
}

Blackboard *BbItemEllipse::blackboard()
{
    return scene()->blackboard();
}

BbScene *BbItemEllipse::scene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemEllipse::data()
{
    return _data;
}

void BbItemEllipse::toolDown(const QPointF &pos)
{
    setZValue(QDateTime::currentMSecsSinceEpoch());
    auto settings = blackboard()->toolSettings<BbItemEllipseData>(BBTT_Ellipse);
    setPenColor(settings->pen.color());
    setBrushColor(settings->brush.color());
    setWeight(settings->weight());

    begin(pos);
    setId(scene()->generatItemId());
    setCircle(scene()->modifiers() & Qt::ShiftModifier);
    scene()->setCurrentItem(this);
    emit blackboard()->itemChanged(BBIET_ellipseDown,this);
}

void BbItemEllipse::toolDraw(const QPointF &pos)
{
    draw(pos);
    emit blackboard()->itemChanged(BBIET_ellipseDraw,this);
}

void BbItemEllipse::toolDone(const QPointF &pos)
{
    Q_UNUSED(pos);
    done();
    emit blackboard()->itemChanged(BBIET_ellipseDone,this);
    scene()->unsetCurrentItem(this);
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
    draw(_mousePos);
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
