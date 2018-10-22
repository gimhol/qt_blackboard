#include "BbItemData.h"
#include "BbItemPenData.h"
#include "BbItemTextData.h"
#include "BbItemStraightData.h"
#include "BbItemRectData.h"
#include "BbItemEllipseData.h"
#include "BbItemTriangle.h"
#include "BbItemTriangleData.h"
#include "Blackboard.h"
#include "BlackboardScene.h"

#include <QResizeEvent>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QScrollBar>

Blackboard::Blackboard(QWidget *parent):
    QGraphicsView(parent),
    _scaleRatio(1)
{
    _pen = BbItemPenData::defaultPen;
    _straightPen = BbItemStraightData::defaultPen;

    _font = BbItemTextData::defalutFont;
    _textColor = BbItemTextData::defalutColor;

    _rectBrush = BbItemRectData::defaultBrush;
    _rectPen = BbItemRectData::defaultPen;

    _ellipseBrush = BbItemEllipseData::defaultBrush;
    _ellipsePen = BbItemEllipseData::defaultPen;

    _triangleBrush = BbItemTriangleData::defaultBrush;
    _trianglePen = BbItemTriangleData::defaultPen;

    setToolCursor(BBTT_Pen,Qt::CrossCursor);
    setToolCursor(BBTT_Rectangle,Qt::CrossCursor);
    setToolCursor(BBTT_Ellipse,Qt::CrossCursor);
    setToolCursor(BBTT_Triangle,Qt::CrossCursor);
    setToolCursor(BBTT_Text,Qt::IBeamCursor);
    setToolCursor(BBTT_Picker,Qt::ArrowCursor);
    setToolCursor(BBTT_Pointer,Qt::BlankCursor);

    setMouseTracking(true);
    setScene(new BlackboardScene(this));
    connect(horizontalScrollBar(),&QScrollBar::valueChanged, this, &Blackboard::onScrollXChanged);
    connect(verticalScrollBar(),&QScrollBar::valueChanged, this, &Blackboard::onScrollYChanged);
}

BlackboardScene *Blackboard::scene() const
{
    return dynamic_cast<BlackboardScene *>(QGraphicsView::scene());
}

QLabel * Blackboard::addPointer(const QString &pointerId, int x, int y)
{
    QLabel *pointer = findChild<QLabel *>(pointerId);
    if(!pointer)
    {
        pointer = new QLabel(this);
        pointer->setText("Fuck");
        pointer->setObjectName(pointerId);
        pointer->setAttribute(Qt::WA_TransparentForMouseEvents);
        pointer->setPixmap(_pointerPixmap);
    }
    if(pointer->isHidden())
    {
        pointer->raise();
        pointer->show();
    }
    pointer->move(x-pointer->width()/2,y-pointer->height()/2);
    return pointer;
}

void Blackboard::movePointer(const QString &pointerId, int x, int y)
{
    QLabel *pointer = findChild<QLabel *>(pointerId);
    if(!pointer)
    {
        pointer = new QLabel(this);
        pointer->setObjectName(pointerId);
        pointer->setAttribute(Qt::WA_TransparentForMouseEvents);
        pointer->setPixmap(_pointerPixmap);
    }
    if(pointer->isHidden())
    {
        pointer->raise();
        pointer->show();
    }
    pointer->move(x-pointer->width()/2,y-pointer->height()/2);
}

void Blackboard::hidePointer(const QString &pointerId)
{
    QLabel *pointer = findChild<QLabel *>(pointerId);
    if(pointer)
    {
        pointer->hide();
    }
}

void Blackboard::setToolCursor(const BbToolType &tool, const QCursor &cursor)
{
    _cursors[tool] = cursor;
}

void Blackboard::setPointerPixmap(const QPixmap &pixmap)
{
    _pointerPixmap = pixmap;
}

void Blackboard::setScroll(int x,int y)
{
    _scrollValue.setX(x);
    _scrollValue.setY(y);
    disconnect(this->horizontalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollXChanged);
    disconnect(this->verticalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollYChanged);
    horizontalScrollBar()->setValue(x);
    verticalScrollBar()->setValue(y);
    connect(this->horizontalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollXChanged);
    connect(this->verticalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollYChanged);
}

void Blackboard::setCanvasId(const QString &id)
{
    scene()->setObjectName(id);
}

QString Blackboard::canvasId()
{
    return scene()->objectName();
}

float Blackboard::scaleRatio()
{
    return _scaleRatio;
}

QSize Blackboard::orginalSize()
{
    return _orginalSize;
}

int Blackboard::orginalWidth()
{
    return _orginalSize.width();
}

int Blackboard::orginalHeight()
{
    return _orginalSize.height();
}

void Blackboard::setOrginalSize(int width, int height)
{
    _orginalSize.setWidth(width);
    _orginalSize.setHeight(height);
}

void Blackboard::setOrginalSize(const QSize &size)
{
    setOrginalSize(size.width(),size.height());
}

void Blackboard::resizeEvent(QResizeEvent *event)
{
    if(_orginalSize.width() == -1)
    {
        _orginalSize = event->size();
        scene()->setSceneRect(0,0, _canvasSize.width(), _canvasSize.height());
    }
    else
    {
        float orginalSizeRatio = static_cast<float>(_orginalSize.width()) / _orginalSize.height();

        float eventSizeRatio = static_cast<float>(event->size().width()) / event->size().height();

        if(eventSizeRatio > orginalSizeRatio)
        {  // 被拉宽
            _scaleRatio = static_cast<float>(event->size().height()) / _orginalSize.height();
        }
        else
        {  // 被拉高
            _scaleRatio = static_cast<float>(event->size().width()) / _orginalSize.width();
        }

        resetTransform();

        scale(static_cast<qreal>(_scaleRatio), static_cast<qreal>(_scaleRatio));

        scene()->setSceneRect(0,0, _canvasSize.width(), _canvasSize.height());

        setScroll(_scrollValue.x(),_scrollValue.y());

        // qDebug() << _scaleRatio;
        emit resized(_scaleRatio);
    }
    QGraphicsView::resizeEvent(event);
}

void Blackboard::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        {
            emit pointerShown(_mousePos);
            return;
        }
        default:
        {
            emit cursorShown(_mousePos);
            break;
        }
    }
}

void Blackboard::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        {
            emit pointerHidden(_mousePos);
            return;
        }
        default:
        {
            emit cursorHidden(_mousePos);
            break;
        }
    }
}

void Blackboard::mousePressEvent(QMouseEvent *event)
{
    _mousePos = event->pos();
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        {
            event->accept();
            return;
        }
        case BBTT_Picker:
        {
            break;
        }
        default:
        {
            if(event->button() == Qt::MouseButton::LeftButton)
            {
                emit cursorHidden(_mousePos);
            }
            break;
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void Blackboard::mouseMoveEvent(QMouseEvent *event)
{
    _mousePos = event->pos();
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        {
            emit pointerMoved(_mousePos);
            return;
        }
        case BBTT_Picker:
        {
            emit cursorMoved(_mousePos);
            break;
        }
        default:
        {
            if(!scene()->isMouseLeftButtonDown())
            {
                emit cursorMoved(_mousePos);
            }
            break;
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void Blackboard::mouseReleaseEvent(QMouseEvent *event)
{
    _mousePos = event->pos();
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        {
            return;
        }
        case BBTT_Picker:
        {
            break;
        }
        default:
        {
            if(event->button() == Qt::MouseButton::LeftButton)
            {
                emit cursorShown(_mousePos);
            }
            break;
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void Blackboard::removeSelectedElement()
{
    scene()->removeSelectedItems();
}

void Blackboard::setToolType(BbToolType toolType)
{
    scene()->setToolType(toolType);
}

void Blackboard::setCanvasSize(const QSize &size)
{
    setCanvasSize(size.width(), size.height());
}

void Blackboard::setCanvasSize(int width, int height)
{
    scene()->setSceneRect(0,0, width, height);

    _canvasSize.setWidth(width);
    _canvasSize.setHeight(height);
}

void Blackboard::setControlEnable(bool enable)
{
    scene()->setControlEnable(enable);

    setAttribute(Qt::WA_TransparentForMouseEvents, !enable);
}

void Blackboard::onScrollXChanged(int x)
{
    onScrollChanged( x, verticalScrollBar()->value() );
}

void Blackboard::onScrollYChanged(int y)
{
    onScrollChanged( horizontalScrollBar()->value(), y );
}

void Blackboard::onScrollChanged(int x, int y)
{
    _scrollValue.setX(x);
    _scrollValue.setY(y);
    emit scrolled(x, y);
}

void Blackboard::clearItems()
{
    scene()->clearItems();
}

void Blackboard::readItemData(BbItemData *itemData)
{
    scene()->readItemData(itemData);
}

void Blackboard::onToolChanged(BbToolType previous, BbToolType current)
{
    switch(previous)
    {
        case BBTT_Pointer:
        {
            emit pointerHidden(_mousePos);
            break;
        }
        default:
        {
            break;
        }
    }
    auto _cursorItr = _cursors.find(current);
    if(_cursorItr != _cursors.end())
    {
        setCursor(*_cursorItr);
    }
    switch(current)
    {
        case BBTT_Pointer:
        {
            setCursor(QCursor(_pointerPixmap,_pointerPixmap.width()/2,_pointerPixmap.height()/2));
            emit pointerShown(_mousePos);
            break;
        }
        default:
        {
            break;
        }
    }
    emit toolChanged(previous,current);
}

void Blackboard::setDefaultPen(const QPen &pen)
{
    BbItemPenData::defaultPen = pen;
}

const QPen &Blackboard::defaultPen()
{
    return BbItemPenData::defaultPen;
}

void Blackboard::setDefaultFont(const QFont &font)
{
    BbItemTextData::defalutFont = font;
}

const QFont &Blackboard::defaultFont()
{
    return BbItemTextData::defalutFont;
}

void Blackboard::setDefaultTextColor(const QColor &color)
{
    BbItemTextData::defalutColor = color;
}

const QColor &Blackboard::defaultTextColor()
{
    return BbItemTextData::defalutColor;
}

void Blackboard::setStraightPen(const QPen &pen)
{
    _straightPen = pen;
}

const QPen &Blackboard::straightPen()
{
    return _straightPen;
}

void Blackboard::setPen(const QPen &pen)
{
    _pen = pen;
}

const QPen &Blackboard::pen()
{
    return _pen;
}

void Blackboard::setFont(const QFont &font)
{
    _font = font;
}

const QFont &Blackboard::font()
{
    return _font;
}

void Blackboard::setTextColor(const QColor &color)
{
    _textColor = color;
}

const QColor &Blackboard::textColor()
{
    return _textColor;
}

void Blackboard::setPenWeight(const qreal &weight)
{
    _penWeight = weight;
}

void Blackboard::setStraightPenWeight(const qreal &weight)
{
    _straightPenWeight = weight;
}

void Blackboard::setTextPointWeight(const qreal &weight)
{
    _textPointWeight = weight;
}

qreal Blackboard::textPointWeight()
{
    return _textPointWeight;
}

qreal Blackboard::penWeight()
{
    return _penWeight;
}

qreal Blackboard::straightPenWeight()
{
    return _straightPenWeight;
}

QColor Blackboard::penColor()
{
    return _pen.color();
}

void Blackboard::setPenColor(const QColor &color)
{
    _pen.setColor(color);
}

QColor Blackboard::straightPenColor()
{
    return _straightPen.color();
}

void Blackboard::setStraightPenColor(const QColor &color)
{
    _straightPen.setColor(color);
}

QColor Blackboard::rectPenColor()
{
    return _rectPen.color();
}

QColor Blackboard::rectBrushColor()
{
    return _rectBrush.color();
}

qreal Blackboard::rectWeight()
{
    return _rectWeight;
}

void Blackboard::setRectPenColor(const QColor &color)
{
    _rectPen.setColor(color);
}

void Blackboard::setRectBrushColor(const QColor &color)
{
    _rectBrush.setColor(color);
}

void Blackboard::setRectWeight(const qreal &weight)
{
    _rectWeight = weight;
}

QColor Blackboard::ellipsePenColor()
{
    return _ellipsePen.color();
}

QColor Blackboard::ellipseBrushColor()
{
    return _ellipseBrush.color();
}

qreal Blackboard::ellipseWeight()
{
    return _ellipseWeight;
}

void Blackboard::setEllipsePenColor(const QColor &color)
{
    _ellipsePen.setColor(color);
}

void Blackboard::setEllipseBrushColor(const QColor &color)
{
    _ellipseBrush.setColor(color);
}

void Blackboard::setEllipseWeight(const qreal &weight)
{
    _ellipseWeight = weight;
}

QColor Blackboard::trianglePenColor()
{
    return _trianglePen.color();
}

QColor Blackboard::triangleBrushColor()
{
    return _triangleBrush.color();
}

qreal Blackboard::triangleWeight()
{
    return _triangleWeight;
}

void Blackboard::setTrianglePenColor(const QColor &color)
{
    _trianglePen.setColor(color);
}

void Blackboard::setTriangleBrushColor(const QColor &color)
{
    _triangleBrush.setColor(color);
}

void Blackboard::setTriangleWeight(const qreal &weight)
{
    _triangleWeight = weight;
}

void Blackboard::addPixmapItem(const QPixmap &pixmap)
{
    scene()->addImageItem(pixmap);
}

void Blackboard::writeStream(QDataStream &stream)
{
    scene()->writeStream(stream);
}

void Blackboard::readStream(QDataStream &stream)
{
    scene()->readStream(stream);
}

float Blackboard::orginalRatio(){return static_cast<float>(_orginalSize.width())/_orginalSize.height();  }

QSize Blackboard::canvasSize(){return _canvasSize;}

int Blackboard::canvasWidth(){return _canvasSize.width();}

int Blackboard::canvasHeight(){return _canvasSize.height();}
