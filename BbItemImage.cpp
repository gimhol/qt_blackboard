#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BbScene.h"

BbItemImage::BbItemImage():
    QGraphicsPixmapItem(),
    IItemIndex (nullptr),
    _myData(new BbItemImageData())
{
    init();
}

BbItemImage::BbItemImage(BbItemData *data):
    QGraphicsPixmapItem(),
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

void BbItemImage::repaint()
{
    qreal x = _myData->x;
    qreal y = _myData->y;
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal width = scene()->width();
        qreal ratio = width / 100;
        x *= ratio;
        y *= ratio;
    }
    setPos(x,y);
    setZValue(_myData->z);
    setPixmap(_myData->pixmap);
    update();
}

void BbItemImage::writeStream(QDataStream &stream)
{
    _myData->pixmap = pixmap();
    _myData->x = x();
    _myData->y = y();
    _myData->z = zValue();
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _myData->x /= ratio;
        _myData->y /= ratio;
    }
    _myData->writeStream(stream);
}

void BbItemImage::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
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
