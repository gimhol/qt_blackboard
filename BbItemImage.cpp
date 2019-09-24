#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BbScene.h"
#include "Blackboard.h"
#include <QPainter>
#include <QDebug>
#include <QStaticText>

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
    update();
}

Blackboard *BbItemImage::blackboard()
{
    return scene()->blackboard();
}

bool BbItemImage::mouseDown(const QPointF &pos)
{
    if(isUnderMouse())
    {
        _mouseDown = true;
        _mousePos = pos;
        updateCursor(pos);
        _stretchDirection = stretchDirection(pos);
        _stretching = _stretchDirection != Invalid;
        if(_stretching)
        {
            _stretchOffset = stretchOffset(pos);
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
    if(_mouseDown)
    {
        _mousePos = pos;
        if(_stretching)
        {
            if(draw(_mousePos))
            {
                emit blackboard()->itemChanged(BBIET_imageResizing,this);
                _myData->prevWidth = _myData->width;
                _myData->prevHeight = _myData->height;
            }
            return true;
        }
        else
        {
            blackboard()->revertToolCursor();
        }
    }
    else
    {
        updateCursor(pos);
    }
    return false;
}

bool BbItemImage::mouseRelease(const QPointF &pos)
{
    updateCursor(pos);
    if(_mouseDown)
    {
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
        _mouseDown = false;
        return ret;
    }
}

bool BbItemImage::clicked(const QPointF &pos)
{
    Q_UNUSED(pos)
    if(_myData->editable)
    {
        scene()->setEditingItem(this);
        return mouseDown(pos);
    }
    return false;
}

bool BbItemImage::doubleClicked(const QPointF &pos)
{
    Q_UNUSED(pos)
//    if(_myData->editable)
//    {
//        scene()->setEditingItem(this);
//        update();
//        return true;
//    }
    return false;
}

void BbItemImage::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    if(_clampRatio != (modifiers == Qt::ShiftModifier))
    {
        setClampRatio(modifiers == Qt::ShiftModifier);
        emit blackboard()->itemChanged(BBIET_imageResizing,this);
    }
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

qreal BbItemImage::minWidth()
{
    return 3 * _controlDotSize;
}

qreal BbItemImage::minHeight()
{
    return 3 * _controlDotSize;
}
#include <QDebug>

void BbItemImage::setClampRatio(bool clampRatio)
{
    if(_clampRatio == clampRatio)
    {
        return;
    }
    _clampRatio = clampRatio;
    if(_mouseDown)
    {
        draw(_mousePos);
    }
}

void BbItemImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!_myData->pixmap.isNull())
    {
        painter->setPen(Qt::NoPen);
        painter->drawPixmap(rect().toRect(),_myData->pixmap);
    }
    else
    {
        painter->setBrush(QColor(0,0,0,100));
        painter->drawRect(rect().toRect());
        painter->setPen(QColor(255,255,255));
        painter->drawStaticText(1,1,QStaticText("Picture"));
    }
    painter->setPen(Qt::NoPen);
    QGraphicsRectItem::paint(painter,option,widget);
    if(isSelected() && scene()->editingItem() == this)
    {
        painter->setPen(QColor(0,0,0));
        painter->setBrush(QColor(255,255,255));
        auto w = int(_controlDotSize);
        auto h = int(_controlDotSize);
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
    if(width < minWidth())
    {
        width = minWidth();
    }
    if(height < minHeight())
    {
        height = minHeight();
    }
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
    Q_UNUSED(point)
    _lastX = int(this->x());
    _lastY = int(this->y());
    _lastW = int(this->rect().width());
    _lastH = int(this->rect().height());
    _prevX = _lastX;
    _prevY = _lastY;
    _prevW = _lastW;
    _prevH = _lastH;
}

bool BbItemImage::draw(const QPointF &pos)
{
    auto point = pos - _stretchOffset;
    auto x = this->x();
    auto y = this->y();
    auto w = this->rect().width();
    auto h = this->rect().height();
    _prevX = x;
    _prevY = y;
    _prevW = w;
    _prevH = h;

    auto ratio = 1;
    if(!_myData->pixmap.isNull())
    {
        ratio = _myData->pixmap.width() / _myData->pixmap.height();
    }
    auto clampWidth = [&]()
    {
        if(_clampRatio)
        {
            w = h * ratio;
        }
    };
    auto clampHeight = [&]()
    {
        if(_clampRatio)
        {
            h = w / ratio;
        }
    };
    auto clampHeightNW = [&]()
    {
        if(_clampRatio)
        {
            clampHeight();
            y = _prevY + _prevH - h;
        }
    };
    auto stretchN = [&]()
    {
        auto t = int(point.y());
        h = int(y + h - t);
        if(h > minHeight())
        {
            y = t;
        }
        else
        {
            h = minHeight();
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
        if(w > minWidth())
        {
            x = l;
        }
        else
        {
            w = minWidth();
        }
    };
    auto stretchE = [&]()
    {
        w = int(point.x() - x);
    };
    switch(_stretchDirection){
        case N: stretchN(); clampWidth(); break;
        case S: stretchS(); clampWidth(); break;
        case W: stretchW(); clampHeight(); break;
        case E: stretchE(); clampHeight(); break;
        case NE: stretchN(); if(_clampRatio){clampWidth();}else{stretchE();}break;
        case SE: stretchS(); if(_clampRatio){clampWidth();}else{stretchE();}break;
        case SW: stretchW(); if(_clampRatio){clampHeight();}else{stretchS();}break;
        case NW: stretchW(); if(_clampRatio){clampHeightNW();}else{stretchN();}break;
        default:break;
    }

    if(x > _lastX + _lastW - minWidth())
    {
        x = _lastX + _lastW - minWidth();
    }
    if(y > _lastY + _lastH - minHeight())
    {
        y = _lastY + _lastH - minHeight();
    }
    if(w < minWidth())
    {
        w = minWidth();
    }
    if(h < minHeight())
    {
        h = minHeight();
    }
    auto esp = 0.00001;
    auto changed =
            abs(_lastX-x) > esp ||
            abs(_lastY-y) > esp ||
            abs(_lastW-w) > esp ||
            abs(_lastH-h) > esp;


    if(changed)
    {
        setPos(x,y);
        resize(w,h);
        _myData->updatePostion(this);
    }
    return changed;
}

bool BbItemImage::done()
{
    auto x = (this->x());
    auto y = (this->y());
    auto w = (this->rect().width());
    auto h = (this->rect().height());
    auto esp = 0.00001;
    auto changed =
            abs(_lastX-x) > esp ||
            abs(_lastY-y) > esp ||
            abs(_lastW-w) > esp ||
            abs(_lastH-h) > esp;
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

void BbItemImage::makeStretchControlDot(QRectF *rects)
{
    auto w = _controlDotSize;
    auto h = _controlDotSize;
    auto l = (x());
    auto t = (y());
    auto r = l+(this->rect().right()-w);
    auto b = t+(this->rect().bottom()-h);
    auto x = l+(this->rect().right()-w)/2;
    auto y = t+(this->rect().bottom()-h)/2;
    rects [0] = QRectF(l,t,w,h);
    rects [1] = QRectF(l,y,w,h);
    rects [2] = QRectF(l,b,w,h);
    rects [3] = QRectF(x,b,w,h);
    rects [4] = QRectF(r,b,w,h);
    rects [5] = QRectF(r,y,w,h);
    rects [6] = QRectF(r,t,w,h);
    rects [7] = QRectF(x,t,w,h);
}

BbItemImage::StretchDirection BbItemImage::stretchDirection(const QPointF &mousePos)
{
    QRectF rects[8];
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

QPointF BbItemImage::stretchOffset(const QPointF &mousePos)
{
    QPointF offset;
    QRectF rects[8];
    makeStretchControlDot(rects);
    for(auto i = 0;i<8;++i)
    {
        if(rects[i].contains(mousePos))
        {
            auto handleN = [&]()
            {
                offset.setY(mousePos.y() - rects[i].top());
            };
            auto handleS = [&]()
            {
                offset.setY(mousePos.y() - rects[i].bottom());
            };
            auto handleW = [&]()
            {
                offset.setX(mousePos.x() - rects[i].left());
            };
            auto handleE = [&]()
            {
                offset.setX(mousePos.x() - rects[i].right());
            };
            switch(StretchDirection(i)){
                case N: handleN(); break;
                case S: handleS(); break;
                case W: handleW(); break;
                case E: handleE(); break;
                case NE: handleN(); handleE();break;
                case SE: handleS(); handleE();break;
                case NW: handleN(); handleW();break;
                case SW: handleS(); handleW();break;
                default:break;
            }
            break;
        }
    }
    return offset;
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
