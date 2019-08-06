#include "BbItemEllipse.h"
#include "BbItemEllipseData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <QDateTime>
#include <QPainter>

BbItemEllipse::BbItemEllipse():
    QGraphicsEllipseItem(),
    IItemIndex(nullptr),
    _myData(new BbItemEllipseData())
{
   init();
}

BbItemEllipse::BbItemEllipse(BbItemData *data):
    QGraphicsEllipseItem(),
    IItemIndex(data),
    _myData(dynamic_cast<BbItemEllipseData*>(data))
{
    init();
}

BbItemEllipse::~BbItemEllipse()
{
    if(_myData){
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemEllipse::init()
{
    if(!_myData)
    {
        _myData = new BbItemEllipseData();
    }
    setPen(_myData->pen);
    setBrush(_myData->brush);
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
    _myData->z = value;
}

void BbItemEllipse::toAbsoluteCoords()
{
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        _myData->mode = BbItemData::CM_ABSOLUTE;
        qreal ratio = scene()->width() / 100;
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
        if(_myData->size.isValid())
        {
            _myData->size *= ratio;
        }
    }
}

void BbItemEllipse::repaint()
{
    setPen(_myData->pen);
    setBrush(_myData->brush);
    qreal x = _myData->x;
    qreal y = _myData->y;
    QSizeF size = _myData->size;
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
    _myData->writeStream(stream);
}

void BbItemEllipse::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    toAbsoluteCoords();
    repaint();
}

void BbItemEllipse::begin(const QPointF &point)
{
    _mousePos = point;
    _beginX = point.x();
    _beginY = point.y();
    setPos(point);
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
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
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
    _myData->empty = !size().isEmpty();
}

void BbItemEllipse::done()
{
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
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
    return _myData;
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
    setCircle(scene()->onlyShiftDown());
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
