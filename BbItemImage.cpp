#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BbScene.h"
#include "Blackboard.h"
#include <QPainter>
#include <QDebug>

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

Blackboard *BbItemImage::blackboard()
{
    return scene()->blackboard();
}

bool BbItemImage::mouseDown(const QPointF &pos)
{
    if(isUnderMouse())
    {
        _stretchDirection = stretchDirection(pos);
        _stretching = _stretchDirection != Invalid;
        if(_stretching)
        {
            begin(pos);
        }
        return _stretching;
    }
    else
    {
        scene()->unsetEditingItem(this);
        update();
        return false;
    }
}

bool BbItemImage::mouseMove(const QPointF &pos)
{
    if(_stretching)
    {
        if(draw(pos))
        {
            emit blackboard()->itemChanged(BBIET_imageResizing,this);
            _myData->prevWidth = _myData->width;
            _myData->prevHeight = _myData->height;
        }
        return true;
    }
    else
    {
        updateCursor(pos);
        return false;
    }
}

bool BbItemImage::mouseRelease(const QPointF &pos)
{
    updateCursor(pos);
    auto ret = _stretching;
    if(ret)
    {
        if(done())
        {
            emit blackboard()->itemChanged(BBIET_imageResized,this);
            _myData->updatePrevPostion();
            _myData->updatePrevSize();
        }
    }
    _stretching = false;
    return ret;
}

bool BbItemImage::doubleClicked(const QPointF &pos)
{
    Q_UNUSED(pos);
    if(_myData->editable)
    {
        scene()->setEditingItem(this);
        update();
        return true;
    }
    return false;
}

void BbItemImage::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers); // TODO: 提供一些快捷键来锁定尺寸比例
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
    painter->setPen(Qt::NoPen);
    QGraphicsRectItem::paint(painter,option,widget);
    qDebug() << isSelected() << (scene()->editingItem() == this);
    if(isSelected() && scene()->editingItem() == this)
    {
        painter->setPen(QColor(0,0,0));
        painter->setBrush(QColor(255,255,255));
        auto w = 5;
        auto h = 5;
        auto r = int(this->rect().right()-w);
        auto b = int(this->rect().bottom()-h);
        auto x = r/2;
        auto y = b/2;
        painter->drawRect(0,0,w,h);
        painter->drawRect(0,y,w,h);
        painter->drawRect(0,b,w,h);
        painter->drawRect(x,b,w,h);
        painter->drawRect(r,b,w,h);
        painter->drawRect(r,y,w,h);
        painter->drawRect(r,0,w,h);
        painter->drawRect(x,0,w,h);
    }
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

void BbItemImage::resize(qreal width, qreal height)
{
    _myData->width = width;
    _myData->height = height;
    setRect(0,0,width,height);
}

void BbItemImage::updatePrevSize()
{
    _myData->updatePrevSize();
}

void BbItemImage::begin(const QPointF &point)
{
    Q_UNUSED(point);
    _lastX = int(this->x());
    _lastY = int(this->y());
    _lastW = int(this->rect().width());
    _lastH = int(this->rect().height());
    _prevX = _lastX;
    _prevY = _lastY;
    _prevW = _lastW;
    _prevH = _lastH;
}

bool BbItemImage::draw(const QPointF &point)
{
    int x = int(this->x());
    int y = int(this->y());
    int w = int(this->rect().width());
    int h = int(this->rect().height());
    _prevX = x;
    _prevY = y;
    _prevW = w;
    _prevH = h;
    auto stretchN = [&]()
    {
        auto t = int(point.y());
        h = int(y + h - t);
        if(h > 10)
        {
            y = t;
        }
        else
        {
            h = 10;
        }
    };
    auto stretchS = [&]()
    {
        h = int(point.y() - y);
    };
    auto stretchW = [&]()
    {
        auto l = int(point.x());
        w = int(x + w - l);
        if(w > 10)
        {
            x = l;
        }
        else
        {
            w = 10;
        }
    };
    auto stretchE = [&]()
    {
        w = int(point.x() - x);
    };
    switch(_stretchDirection){
        case N: stretchN(); break;
        case S: stretchS(); break;
        case W: stretchW(); break;
        case E: stretchE(); break;
        case NE: stretchN(); stretchE();break;
        case SE: stretchS(); stretchE();break;
        case NW: stretchN(); stretchW();break;
        case SW: stretchS(); stretchW();break;
        default:break;
    }
    auto changed = _lastX != x || _lastY != y || _lastW != w || _lastH != h;
    if(changed)
    {
        resize(w,h);
        setPos(x,y);
        _myData->updatePostion(this);
    }
    return changed;
}

bool BbItemImage::done()
{
    auto x = int(this->x());
    auto y = int(this->y());
    auto w = int(this->rect().width());
    auto h = int(this->rect().height());
    auto changed = _lastX != x || _lastY != y || _lastW != w || _lastH != h;
    _lastX = x;
    _lastY = y;
    _lastW = w;
    _lastH = h;
    _myData->width = w;
    _myData->height = h;
    return changed;
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

void BbItemImage::makeStretchControlDot(QRect *rects)
{
    auto w = 5;
    auto h = 5;
    auto l = int(x());
    auto t = int(y());
    auto r = l+int(this->rect().right()-w);
    auto b = t+int(this->rect().bottom()-h);
    auto x = l+int(this->rect().right()-w)/2;
    auto y = t+int(this->rect().bottom()-h)/2;
    rects [0] = QRect(l,t,w,h);
    rects [1] = QRect(l,y,w,h);
    rects [2] = QRect(l,b,w,h);
    rects [3] = QRect(x,b,w,h);
    rects [4] = QRect(r,b,w,h);
    rects [5] = QRect(r,y,w,h);
    rects [6] = QRect(r,t,w,h);
    rects [7] = QRect(x,t,w,h);
}

BbItemImage::StretchDirection BbItemImage::stretchDirection(const QPointF &mousePos)
{
    QRect rects[8];
    makeStretchControlDot(rects);
    for(auto i = 0;i<8;++i)
    {
        if(rects[i].contains(mousePos.toPoint()))
        {
            return StretchDirection(i);
        }
    }
    return Invalid;
}

void BbItemImage::updateCursor(const QPointF &mousePos)
{
    updateCursor(stretchDirection(mousePos));
}
void BbItemImage::updateCursor(StretchDirection direction)
{
    if(W == direction || E == direction)
    {
        blackboard()->setCursor(Qt::SizeHorCursor);
    }
    else if(N == direction || S == direction)
    {
        blackboard()->setCursor(Qt::SizeVerCursor);
    }
    else if(NW == direction || SE == direction)
    {
        blackboard()->setCursor(Qt::SizeFDiagCursor);
    }
    else if(NE == direction || SW == direction)
    {
        blackboard()->setCursor(Qt::SizeBDiagCursor);
    }
    else
    {
        blackboard()->revertToolCursor();
    }
}
