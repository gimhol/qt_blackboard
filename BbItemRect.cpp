#include "BbItemRect.h"
#include "BbItemRectData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <QDateTime>
#include <QPainter>

BbItemRect::BbItemRect():
    QGraphicsRectItem(),
    _myData(new BbItemRectData())
{
   init();
}

BbItemRect::BbItemRect(BbItemData *data):
    QGraphicsRectItem(),
    _myData(dynamic_cast<BbItemRectData*>(data))
{
    init();
}

BbItemRect::~BbItemRect()
{
    if(_myData){
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemRect::init()
{
    if(!_myData)
    {
        _myData = new BbItemRectData();
    }
    setPen(_myData->pen);
    setBrush(_myData->brush);
}

void BbItemRect::repaint()
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

void BbItemRect::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    _myData->z = z();
    _myData->size = rect().size();
    _myData->writeStream(stream);
}

void BbItemRect::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
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
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
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
    _myData->updatePostion(this);
    setRect(0,0,std::abs(_dragX-_beginX),std::abs(_dragY-_beginY));
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
    _myData->empty = !size().isEmpty();
}

void BbItemRect::done()
{
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
    _editing = false;
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

QString BbItemRect::id()
{
    return _myData->lid;
}

void BbItemRect::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemRect::toolType()
{
    return _myData->tooltype;
}

Blackboard *BbItemRect::blackboard()
{
    return bbScene()->blackboard();
}

BbScene *BbItemRect::bbScene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemRect::data()
{
    return _myData;
}

void BbItemRect::toolDown(const QPointF &pos)
{
    setId(bbScene()->generatItemId());
    setZValue(QDateTime::currentMSecsSinceEpoch());
    auto settings = blackboard()->toolSettings<BbItemRectData>(BBTT_Rectangle);
    setPenColor(settings->pen.color());
    setBrushColor(settings->brush.color());
    setWeight(settings->weight());

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

void BbItemRect::toolDone(const QPointF &pos)
{
    Q_UNUSED(pos);
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

qreal BbItemRect::z()
{
    return zValue();
}

void BbItemRect::setZ(const qreal &value)
{
    setZValue(value);
    _myData->z = value;
}

void BbItemRect::absolutize()
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
        if(_myData->size.isValid())
        {
            _myData->size *= ratio;
        }
    }
}

bool BbItemRect::isEditing()
{
    return _editing;
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
    painter->setRenderHint(QPainter::Antialiasing, true);
    QGraphicsRectItem::paint(painter,option,widget);
}


