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
    QGraphicsRectItem(){
    init();
}
BbItemImage::BbItemImage(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemImageData*>(data))
{
    init();
}

BbItemImage::~BbItemImage()
{
    if(_data)
    {
        delete _data;
        _data = nullptr;
    }
}
void BbItemImage::init()
{
    if(!_data)
        _data = new BbItemImageData();
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}

qreal BbItemImage::minWidth()
{
    auto w = _data->size.width();
    auto h = _data->size.height();
    if(h > 0 && h > w && ratioLock())
    {
        return 6 * _dotSize * w / h;
    }
    return 6 * _dotSize;
}

qreal BbItemImage::minHeight()
{
    auto w = _data->size.width();
    auto h = _data->size.height();
    if(w > 0 && w > h && ratioLock())
    {
        return 6 * _dotSize * h / w;
    }
    return 6 * _dotSize;
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
    auto w = _data->size.width();
    auto h = _data->size.height();
    if(_data->pixmap.isNull())
    {
        if(equal(0,h))
        {
            return 1;
        }
        else
        {
            return w / h;
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

BbItemImage::Direction BbItemImage::stretchDirection(const QPointF &scenePos)
{
    QRectF rects[8];
    updateDots(rects);
    for(int i=0;i<8;++i)
    {
        if(rects[i].contains(scenePos))
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
            case Invalid: break;
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

void BbItemImage::resize(QSizeF size)
{
    resize(size.width(),size.height());
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
    _data->size.setWidth(width);
    _data->size.setHeight(height);
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
    case Invalid: break;
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
    _data->size.setWidth(w);
    _data->size.setHeight(h);
    return changed;
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
    auto bb = blackboard();
    if(_data->pixmap.isNull() && bb){
        if(!_data->path.isEmpty()) // 通过信号通知外部，尝试读取本地路径
            emit bb->itemChanged(BBIET_imageHasPath,this);
        if(!_data->url.isEmpty()) // 通过信号通知外部，尝试读取URL
            emit bb->itemChanged(BBIET_imageHasUrl,this);
    }
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
        _data->size *= ratio;
        resize(_data->size);
    }
}

void BbItemImage::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverMoveEvent(event);
    if(isSelected()){
        _direction = stretchDirection(event->scenePos());
        _stretching = _direction != Invalid;
        _stretchOffset = stretchOffset(event->scenePos());
        updateCursor(_direction);
    }
}

void BbItemImage::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    blackboard()->revertToolCursor();
}

void BbItemImage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    _mouseDown = true;
    _mousePos = event->scenePos();
    if(_stretching)
        begin(_mousePos);
}

void BbItemImage::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    _mousePos = event->scenePos();
    if(_stretching){
        // resize it.
        if(draw(_mousePos)){
            emit blackboard()->itemChanged(BBIET_imageResizing,this);
            _data->updatePrevSize();
        }
    }else{
        // move it.
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void BbItemImage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if(_data->editable)
        update();

    if(_stretching){
        if(done()){
            emit blackboard()->itemChanged(BBIET_imageResized,this);
            _data->updatePrevPostion();
            _data->updatePrevSize();
        }
        _stretching = false;
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

void BbItemImage::repaint()
{
    setRect(0,0,_data->size.width(),_data->size.height());
    setPos(_data->x,_data->y);
    setZValue(_data->z);
    setPixmap(_data->pixmap);
    update();
}

void BbItemImage::writeStream(QDataStream &stream)
{
    _data->x = x();
    _data->y = y();
    _data->size = rect().size();
    _data->z = zValue();
    _data->writeStream(stream);
}

void BbItemImage::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
    if(_data->pixmap.isNull()){
        if(!_data->path.isEmpty()) // 通过信号通知外部，尝试读取本地路径
            emit blackboard()->itemChanged(BBIET_imageHasPath,this);
        if(!_data->url.isEmpty()) // 通过信号通知外部，尝试读取URL
            emit blackboard()->itemChanged(BBIET_imageHasUrl,this);
    }
}

QJsonObject BbItemImage::toJsonObject()
{
    _data->x = x();
    _data->y = y();
    _data->size = rect().size();
    _data->z = zValue();
    return _data->toJsonObject();
}

void BbItemImage::fromJsonObject(const QJsonObject &jobj)
{
    _data->fromJsonObject(jobj);
    absolutize();
    repaint();

    auto bb = blackboard();
    if(_data->pixmap.isNull() && bb){
        if(!_data->path.isEmpty()) // 通过信号通知外部，尝试读取本地路径
            emit bb->itemChanged(BBIET_imageHasPath,this);
        if(!_data->url.isEmpty()) // 通过信号通知外部，尝试读取URL
            emit bb->itemChanged(BBIET_imageHasUrl,this);
    }
}
