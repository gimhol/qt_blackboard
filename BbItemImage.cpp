#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BbScene.h"
#include "Blackboard.h"
#include <QPainter>
#include <QDebug>
#include <QStaticText>

static auto esp = 0.000001;
static bool equal(const qreal &a,const qreal &b)
{
    return abs(a-b) < esp;
}

BbItemImage::BbItemImage():
    QGraphicsRectItem(),
    _data(new BbItemImageData()){}

BbItemImage::BbItemImage(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemImageData*>(data))
{
    if(!_data)
    {
        new BbItemImageData();
    }
}

BbItemImage::~BbItemImage()
{
    if(_data)
    {
        delete _data;
        _data = nullptr;
    }
}

qreal BbItemImage::minWidth()
{
    if(_data->height > 0 && _data->height > _data->width && ratioLock())
    {
        return 3 * _dotSize * _data->width / _data->height;
    }
    return 3 * _dotSize;
}

qreal BbItemImage::minHeight()
{
    if(_data->width > 0 && _data->width > _data->height && ratioLock())
    {
        return 3 * _dotSize * _data->height / _data->width;
    }
    return 3 * _dotSize;
}

bool BbItemImage::ratioLock()
{
    return _ratioLock || BbItemImageData::isRatioLocked();
}

void BbItemImage::setRatioLock(bool ratioLock)
{
    if(_ratioLock == ratioLock)
    {
        return;
    }
    _ratioLock = ratioLock;
    if(_mouseDown)
    {
        draw(_mousePos);
    }
}

void BbItemImage::setPixmap(const QPixmap &pixmap)
{
    _data->pixmap = pixmap;
    update();
}

void BbItemImage::setProgress(qreal progress)
{
    if(abs(_progress - progress) < 0.00001)
    {
        _progress = progress;
        update();
    }
}

void BbItemImage::setText(QString text)
{
    if(_text != text)
    {
        _text = text;
        update();
    }
}

qreal BbItemImage::ratio()
{
    if(_data->pixmap.isNull())
    {
        if(equal(0,_data->height))
        {
            return 1;
        }
        else
        {
            return _data->width / _data->height;
        }
    }
    else
    {
        return 1.0 * _data->pixmap.width() / _data->pixmap.height();
    }
}

QString BbItemImage::url()
{
    return _data->url;
}

QString BbItemImage::path()
{
    return _data->path;
}

void BbItemImage::setUrl(QString url)
{
    _data->url = url;
}

void BbItemImage::setPath(QString path)
{
    _data->path = path;
}

BbItemImage::Direction BbItemImage::stretchDirection(const QPointF &mousePos)
{
    QRectF rects[8];
    updateDots(rects);
    for(int i=0;i<8;++i)
    {
        if(rects[i].contains(mousePos.toPoint()))
        {
            return Direction(i);
        }
    }
    return Invalid;
}

QPointF BbItemImage::stretchOffset(const QPointF &mousePos)
{
    QPointF offset;
    QRectF rects[8];
    updateDots(rects);

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
            switch(Direction(i)){
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

void BbItemImage::updateDots(QRectF *rects)
{
    auto w = _dotSize;
    auto h = _dotSize;
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

void BbItemImage::updateCursor(const QPointF &mousePos)
{
    updateCursor(stretchDirection(mousePos));
}

void BbItemImage::updateCursor(Direction direction)
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
    _data->width = width;
    _data->height = height;
    setRect(0,0,width,height);
}

void BbItemImage::updatePrevSize()
{
    _data->updatePrevSize();
}

void BbItemImage::begin(const QPointF &point)
{
    Q_UNUSED(point)
    modifiersChanged(bbScene()->modifiers());
    _lastX = int(this->x());
    _lastY = int(this->y());
    _lastW = int(this->rect().width());
    _lastH = int(this->rect().height());
}

bool BbItemImage::draw(const QPointF &mousePos)
{
    auto x = this->x();
    auto y = this->y();
    auto width = this->rect().width();
    auto height = this->rect().height();

    auto right = x + width;     // 图片下侧X坐标
    auto bottom = y + height;    // 图片右侧Y坐标
    auto centerX = x + width/2; // 图片中心X坐标
    auto centerY = y + height/2; // 图片中心Y坐标

    auto ratio = this->ratio();
    auto point = mousePos - _stretchOffset;
    auto clampWidthFromMiddle = [&]()
    {
        if(ratioLock())
        {
            width = height * ratio;
            x = centerX - width / 2;
        }
    };
    auto clampHeightFromMiddle = [&]()
    {
        if( ratioLock() )
        {
            height = width / ratio;
            y = centerY - height / 2;
        }
    };
    auto stretchN = [&]()
    {
        if(bottom - point.y() < minHeight())
        {
            height = minHeight();
            y = bottom - height;
        }
        else
        {
            y = point.y();
            height = bottom - y;
        }
    };
    auto stretchS = [&]()
    {
        height = std::max(point.y()-y, minHeight());
    };
    auto stretchW = [&]()
    {
        if(right - point.x() < minWidth())
        {
            width = minWidth();
            x = right - width;
        }
        else
        {
            x = point.x();
            width = right - x;
        }
    };
    auto stretchE = [&]()
    {
        width = std::max(point.x()-x, minWidth());
    };
    auto clampSize = [&]()
    {
        if(width/height > ratio)
        {
            height = width / ratio;
        }
        else
        {
            width = height * ratio;
        }
    };
    auto clampFromTopLeft = [&]()
    {
        if(ratioLock())
        {
            clampSize();
        }
    };
    auto clampFromTopRight = [&]()
    {
        if(ratioLock())
        {
            clampSize();
            x = right - width;
        }
    };
    auto clampFromBottomLeft = [&]()
    {
        if(ratioLock())
        {
            clampSize();
            y = bottom - height;
        }
    };
    auto clampFromBottomRight = [&]()
    {
        if(ratioLock())
        {
            clampSize();
            y = bottom - height;
            x = right - width;
        }
    };
    switch(_direction)
    {
    case N: {stretchN(); clampWidthFromMiddle(); break;}
    case S: {stretchS(); clampWidthFromMiddle(); break;}
    case W: {stretchW(); clampHeightFromMiddle(); break;}
    case E: {stretchE(); clampHeightFromMiddle(); break;}
    case NE:{stretchN(); stretchE(); clampFromBottomLeft(); break;}
    case SE:{stretchS(); stretchE(); clampFromTopLeft(); break;}
    case SW:{stretchW(); stretchS(); clampFromTopRight(); break;}
    case NW:{stretchW(); stretchN(); clampFromBottomRight(); break;}
    default:{break;}
    }
    if(_centerLock)
    {
        y = centerY - height/2;
        x = centerX - width/2;
    }
    auto changed =
            !equal(_lastX,x) || !equal(_lastY,y) ||
            !equal(_lastW,width) || !equal(_lastH,height);

    if(changed)
    {
        setPos(x,y);
        resize(width,height);
        _data->updatePostion(this);
    }
    return changed;
}

bool BbItemImage::done()
{
    auto x = (this->x());
    auto y = (this->y());
    auto w = (this->rect().width());
    auto h = (this->rect().height());
    auto changed =
            !equal(_lastX,x) ||
            !equal(_lastY,y) ||
            !equal(_lastW,w) ||
            !equal(_lastH,h);
    _lastX = x;
    _lastY = y;
    _lastW = w;
    _lastH = h;
    _data->width = w;
    _data->height = h;
    return changed;
}

Blackboard *BbItemImage::blackboard()
{
    return bbScene()->blackboard();
}

bool BbItemImage::mouseDown(const QPointF &pos)
{
    if(isUnderMouse())
    {
        _mouseDown = true;
        _mousePos = pos;
        updateCursor(pos);
        _direction = stretchDirection(pos);
        _stretching = _direction != Invalid;
        if(_stretching)
        {
            _stretchOffset = stretchOffset(pos);
            begin(pos);
        }
        return _stretching;
    }
    else
    {
        bbScene()->unsetEditingItem(this);
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
                _data->prevWidth = _data->width;
                _data->prevHeight = _data->height;
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
                _data->updatePrevPostion();
                _data->updatePrevSize();
            }
        }
        _stretching = false;
        _mouseDown = false;
        return ret;
    }
    return false;
}

bool BbItemImage::clicked(const QPointF &pos)
{
    Q_UNUSED(pos)
    if(_data->editable)
    {
        bbScene()->setEditingItem(this);
        return mouseDown(pos);
    }
    return false;
}

void BbItemImage::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    auto ratioLock = bool(modifiers & Qt::ShiftModifier);
    auto centerLock = bool(modifiers & Qt::AltModifier);
    bool changed = _ratioLock != ratioLock || _centerLock != centerLock;
    if(_ratioLock != ratioLock)
    {
        _ratioLock = ratioLock;
    }
    if(_centerLock != centerLock)
    {
        _centerLock = centerLock;
    }
    if(changed)
    {
        if(_mouseDown)
        {
            draw(_mousePos);
        }
        emit blackboard()->itemChanged(BBIET_imageResizing,this);
    }

}

void BbItemImage::added()
{
    if(!_data->url.isEmpty() && _data->pixmap.isNull())
    {
        emit blackboard()->itemChanged(BBIET_imageHasUrl,this);
    }
}

qreal BbItemImage::z()
{
    return zValue();
}

void BbItemImage::setZ(const qreal &value)
{
    setZValue(value);
    _data->z = value;
}

void BbItemImage::absolutize()
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
        _data->width *= ratio;
        _data->height *= ratio;
        resize(_data->width,_data->height);
    }
}

BbItemData *BbItemImage::data()
{
    return _data;
}

void BbItemImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setRenderHint(QPainter::Antialiasing);
    if(!_data->pixmap.isNull())
    {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->setPen(Qt::NoPen);
        painter->drawPixmap(rect().toRect(),_data->pixmap);
    }
    else
    {
        auto bg = rect();
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0,0,0,50));
        painter->drawRect(bg);
    }
    auto progress = int(_progress * 100);
    auto showProgress = progress > 0 && progress < 99;
    if(showProgress || !_text.isEmpty())
    {
        painter->setRenderHint(QPainter::TextAntialiasing);
        QString text = _text;
        if(showProgress)
        {
            text = QString("%1 %2%").arg(_text).arg(int(_progress*100));
            auto fg = rect();
            fg.setWidth(fg.width() * _progress);
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(0,0,0,50));
            painter->drawRect(fg);
        }
        painter->setPen(QColor(255,255,255));
        painter->setBrush(QColor(255,255,255));
        painter->drawText(rect(),Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap,text);
    }
    if(isSelected())
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(Qt::white);
        painter->drawRect(rect());
        painter->setPen(Qt::DashLine);
        painter->drawRect(rect());
        if(bbScene()->editingItem() == this)
        {
            painter->setPen(QColor(0,0,0));
            painter->setBrush(QColor(255,255,255));

            QRectF rect[8];
            updateDots(rect);

            for(auto i = 0;i<8;++i)
            {
                rect[i].translate(-x(),-y());
                painter->drawRect(rect[i]);
            }
        }
    }
}

void BbItemImage::repaint()
{
    setRect(QRectF(0,0,_data->width,_data->height));
    setPos(_data->x,_data->y);
    setZValue(_data->z);
    setPixmap(_data->pixmap);
    update();
}

void BbItemImage::writeStream(QDataStream &stream)
{
    _data->x = x();
    _data->y = y();
    _data->width = rect().width();
    _data->height = rect().height();
    _data->z = zValue();
    _data->writeStream(stream);
}

void BbItemImage::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
    if(!_data->url.isEmpty() && _data->pixmap.isNull())
    {
        emit blackboard()->itemChanged(BBIET_imageHasUrl,this);
    }
}

QString BbItemImage::id() const
{
    return _data->lid;
}

void BbItemImage::setId(const QString &id)
{
    _data->lid = id;
}

BbToolType BbItemImage::toolType() const
{
    return _data->tooltype;
}

BbScene *BbItemImage::bbScene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}
