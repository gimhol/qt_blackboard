#include "BbItemTriangle.h"
#include "BbItemTriangleData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

BbItemTriangle::BbItemTriangle():
    QGraphicsRectItem(),
    _data(new BbItemTriangleData)
{
    init();
}

BbItemTriangle::BbItemTriangle(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemTriangleData*>(data))
{
    init();
}

BbItemTriangle::~BbItemTriangle()
{
    if(_data)
    {
        delete _data;
        _data = nullptr;
    }
}

void BbItemTriangle::init()
{
    if(!_data)
    {
        _data = new BbItemTriangleData();
    }
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}

void BbItemTriangle::toolDown(const QPointF &pos)
{
    if(step() == 0)
    {
        setId(blackboard()->factory()->makeItemId(toolType()));
        setZ(blackboard()->factory()->makeItemZ(toolType()));
        updatePrevZ();

        auto settings = blackboard()->toolSettings<BbItemTriangleData>(BBTT_Triangle);
        _data->pen = settings->pen;
        _data->brush = settings->brush;
        _data->setWeight(settings->weight());

        begin(pos);
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
    Q_UNUSED(pos)
    done();
    emit blackboard()->itemChanged(BBIET_triangleDone,this);
    if(step() > 1)
    {
        bbScene()->unsetCurrentItem(this);
    }
}

void BbItemTriangle::absolutize()
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
        for(int i = 0; i < 3; ++i)
        {
            if(!_data->points[i].isNull())
            {
                _data->points[i] *= ratio;
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
    _data->points[0] = point;
    _data->points[1] = point;
    _data->points[2] = point;
    _mousePos = point;
    _data->empty = false;

    setupRectWithABC();
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

void BbItemTriangle::draw(const QPointF &point)
{
    _mousePos = point;
    if(_step == 0)
    {
        _data->points[1] = point;
    }
    else
    {
        _data->points[2] = point;
    }
    setupRectWithABC();
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

void BbItemTriangle::done()
{
    ++_step;
    _data->updatePostion(this);
    _data->updatePrevPostion();
    if(_step == 2)
    {
        _editing = false;
    }
}

void BbItemTriangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter,option,widget);
    if(!_data->empty)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(_data->pen);
        painter->setRenderHint(QPainter::Antialiasing);

        if(_data->points[0] == _data->points[1])  // 一个点。
        {
            qreal halfPenW = 0.5 * _data->pen.widthF();
            painter->setBrush(QBrush(_data->pen.color()));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(int(halfPenW),
                                 int(halfPenW),
                                 int(2*halfPenW),
                                 int(2*halfPenW));
        }
        else if(_data->points[1] == _data->points[2]) // 两个点。
        {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(_data->pen);
            painter->drawLine(_points[0],_points[1]);
        }
        else // 三个点。
        {
            painter->setBrush(_data->brush);
            painter->setPen(_data->pen);
            painter->drawPolygon(_points,3);
        }
    }
}

void BbItemTriangle::setupRectWithABC()
{
    qreal halfPenW = 0.5 * _data->pen.widthF();

    qreal minx = _data->points[0].x();
    qreal maxx = _data->points[0].x();
    qreal miny = _data->points[0].y();
    qreal maxy = _data->points[0].y();
    for(int i=1;i<3;++i)
    {
        if(minx > _data->points[i].x())
        {
            minx = _data->points[i].x();
        }
        if(maxx < _data->points[i].x())
        {
            maxx = _data->points[i].x();
        }
        if(miny > _data->points[i].y())
        {
            miny = _data->points[i].y();
        }
        if(maxy < _data->points[i].y())
        {
            maxy = _data->points[i].y();
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
        _points[i] = _data->points[i] - pos();
    }
    update();
}

void BbItemTriangle::repaint()
{
    setupRectWithABC();
    if( _data->isPositionValid() )
    {
        setPos(_data->x,_data->y);
    }
    setZValue(_data->z);
    update();
}

unsigned char BbItemTriangle::step()
{
    return _step;
}

QPointF BbItemTriangle::point(int index)
{
    return _data->points[index];
}

void BbItemTriangle::writeStream(QDataStream &stream)
{
    _data->x = x();
    _data->y = y();
    _data->z = zValue();
    _data->writeStream(stream);
}

void BbItemTriangle::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
}

QJsonObject BbItemTriangle::toJsonObject()
{
    _data->x = x();
    _data->y = y();
    _data->z = zValue();
    return _data->toJsonObject();
}

void BbItemTriangle::fromJsonObject(const QJsonObject &jobj)
{
    _data->fromJsonObject(jobj);
    absolutize();
    repaint();
}

BbItemData *BbItemTriangle::data()
{
    return _data;
}
