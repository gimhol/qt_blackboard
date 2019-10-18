#include "BbItemTriangle.h"
#include "BbItemTriangleData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QDateTime>

BbItemTriangle::BbItemTriangle():
    QGraphicsRectItem(),
    _myData(new BbItemTriangleData)
{
    init();
}

BbItemTriangle::BbItemTriangle(BbItemData *data):
    QGraphicsRectItem(),
    _myData(dynamic_cast<BbItemTriangleData*>(data))
{
    init();
}

BbItemTriangle::~BbItemTriangle()
{
    if(_myData)
    {
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemTriangle::init()
{
    if(!_myData)
    {
        _myData = new BbItemTriangleData();
    }
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}

Blackboard *BbItemTriangle::blackboard()
{
    return bbScene()->blackboard();
}

void BbItemTriangle::toolDown(const QPointF &pos)
{
    if(step() == 0)
    {
        setZValue(QDateTime::currentMSecsSinceEpoch());
        auto settings = blackboard()->toolSettings<BbItemTriangleData>(BBTT_Triangle);
        setPenColor(settings->pen.color());
        setBrushColor(settings->brush.color());
        setWeight(settings->weight());
        begin(pos);
        setId(bbScene()->generatItemId());
        bbScene()->setCurrentItem(this);
        emit blackboard()->itemChanged(BBIET_triangleDown,this);
    }
}

void BbItemTriangle::toolDraw(const QPointF &pos)
{
    draw(pos);
    emit blackboard()->itemChanged(BBIET_triangleDraw,this);
}

void BbItemTriangle::toolDone(const QPointF &pos)
{
    Q_UNUSED(pos);
    done();
    emit blackboard()->itemChanged(BBIET_triangleDone,this);
    if(step() > 1)
    {
        bbScene()->unsetCurrentItem(this);
    }
}

void BbItemTriangle::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    // do nothing
}

qreal BbItemTriangle::z()
{
    return zValue();
}

void BbItemTriangle::setZ(const qreal &value)
{
    setZValue(value);
}

void BbItemTriangle::absolutize()
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
        for(int i = 0; i < 3; ++i)
        {
            if(!_myData->points[i].isNull())
            {
                _myData->points[i] *= ratio;
            }
        }
    }
}

bool BbItemTriangle::isEditing()
{
    return _editing;
}

void BbItemTriangle::begin(const QPointF &point)
{
    _editing = true;
    _myData->points[0] = point;
    _myData->points[1] = point;
    _myData->points[2] = point;
    _mousePos = point;
    _myData->empty = false;

    setupRectWithABC();
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
}

void BbItemTriangle::draw(const QPointF &point)
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
    _myData->updatePrevPostion();
}

void BbItemTriangle::done()
{
    ++_step;
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
    if(_step == 2)
    {
        _editing = false;
    }
}

void BbItemTriangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter,option,widget);
    if(!_myData->empty)
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

void BbItemTriangle::repaint()
{
    setupRectWithABC();
    if( _myData->isPositionValid() )
    {
        setPos(_myData->x,_myData->y);
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

QColor BbItemTriangle::penColor()
{
    return _myData->pen.color();
}

void BbItemTriangle::setPenColor(const QColor &color)
{
    _myData->pen.setColor(color);
}

QColor BbItemTriangle::brushColor()
{
    return _myData->brush.color();
}

void BbItemTriangle::setBrushColor(const QColor &color)
{
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

void BbItemTriangle::setWeight(qreal weight)
{
    _myData->setWeight(weight);
}

void BbItemTriangle::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    _myData->z = zValue();
    _myData->writeStream(stream);
}

void BbItemTriangle::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    absolutize();
    repaint();
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

BbScene *BbItemTriangle::bbScene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemTriangle::data()
{
    return _myData;
}
