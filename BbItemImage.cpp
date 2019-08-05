#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BbScene.h"

#include <QPainter>

BbItemImage::BbItemImage():
    QGraphicsRectItem(),
    IItemIndex (nullptr),
    _myData(new BbItemImageData())
{
    init();
}

BbItemImage::BbItemImage(BbItemData *data):
    QGraphicsRectItem(),
    IItemIndex (data),
    _myData(dynamic_cast<BbItemImageData*>(data))
{
    init();
}

BbItemImage::~BbItemImage()
{
    if(_myData)
    {
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemImage::init()
{
    if(!_myData)
    {
        _myData = new BbItemImageData();
    }
    if(!_myData->pixmap.isNull())
    {
        setPixmap(_myData->pixmap);
    }
}

void BbItemImage::setPixmap(const QPixmap &pixmap)
{
    _myData->pixmap = pixmap;
}

const QPixmap &BbItemImage::pixmap()
{
    return _myData->pixmap;
}

Blackboard *BbItemImage::blackboard()
{
    return scene()->blackboard();
}

void BbItemImage::toolDown(const QPointF &pos)
{

}

void BbItemImage::toolDraw(const QPointF &pos)
{

}

void BbItemImage::toolDone(const QPointF &pos)
{

}

void BbItemImage::modifiersChanged(Qt::KeyboardModifiers modifiers)
{

}

qreal BbItemImage::z()
{
    return zValue();
}

void BbItemImage::setZ(const qreal &value)
{
    setZValue(value);
    _myData->z = value;
}

void BbItemImage::toAbsoluteCoords()
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
        _myData->width *= ratio;
        _myData->height *= ratio;
    }
}

void BbItemImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!_myData->pixmap.isNull())
    {
        painter->drawPixmap(rect().toRect(),_myData->pixmap);
    }
    QGraphicsRectItem::paint(painter,option,widget);
}

void BbItemImage::repaint()
{
    setRect(QRectF(0,0,_myData->width,_myData->height));
    setPos(_myData->x,_myData->y);
    setZValue(_myData->z);
    setPixmap(_myData->pixmap);
    update();
}

void BbItemImage::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    _myData->width = rect().width();
    _myData->height = rect().height();
    _myData->z = zValue();
    _myData->writeStream(stream);
}

void BbItemImage::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    toAbsoluteCoords();
    repaint();
}

QString BbItemImage::id() const
{
    return _myData->lid;
}

void BbItemImage::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemImage::toolType() const
{
    return _myData->tooltype;
}

BbScene *BbItemImage::scene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemImage::data()
{
    return _myData;
}
