#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BlackboardScene.h"

BbItemImage::BbItemImage():
    QGraphicsPixmapItem()
{
    _myData = new BbItemImageData();
}

BbItemImage::BbItemImage(BbItemImageData *data):
    QGraphicsPixmapItem()
{
    _myData = data;
    setPixmap(data->pixmap);
}

BbItemImage::~BbItemImage()
{
    if(_myData)
    {
        delete _myData;
        _myData = nullptr;
    }
}

void BbItemImage::repaintWithItemData()
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
    repaintWithItemData();
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

BlackboardScene *BbItemImage::scene()
{
    return dynamic_cast<BlackboardScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemImage::data()
{
    return _myData;
}
