#include "Blackboard.h"
#include "BbScene.h"
#include "BbPointer.h"

#include <QResizeEvent>
#include <QDebug>
#include <QTimer>
#include <QScrollBar>
#include <QApplication>

class BlackboardPrivate
{
public:
    qreal scaleRatio = 1;
    QSize orginalSize;
    QSize canvasSize;
    QPixmap pointerPixmap;
    QPoint mousePos;
    QPoint scrollValue;
    QString cid;

    QMap<BbToolType,BbItemData*> toolSettings;
    QMap<BbToolType,QCursor> cursors;
    QMap<QString, QPoint> lazerPenPositions;
    bool tabletActive = false;
};

Blackboard::Blackboard(QWidget *parent):
    QGraphicsView(parent),
    dptr(new BlackboardPrivate)
{
    setAttribute(Qt::WA_TabletTracking);
    setScene(new BbScene(this));
    setMouseTracking(true);

    setToolCursor(BBTT_Pen,Qt::CrossCursor);
    setToolCursor(BBTT_Rectangle,Qt::CrossCursor);
    setToolCursor(BBTT_Ellipse,Qt::CrossCursor);
    setToolCursor(BBTT_Triangle,Qt::CrossCursor);
    setToolCursor(BBTT_Text,Qt::IBeamCursor);
    setToolCursor(BBTT_Picker,Qt::ArrowCursor);
    setToolCursor(BBTT_Pointer,Qt::BlankCursor);

    connect(horizontalScrollBar(),&QScrollBar::valueChanged, this, &Blackboard::onScrollXChanged);
    connect(verticalScrollBar(),&QScrollBar::valueChanged, this, &Blackboard::onScrollYChanged);


    qApp->installEventFilter(this);
}

Blackboard::~Blackboard()
{
    for(auto settings: dptr->toolSettings)
    {
        delete settings;
    }
    delete dptr;
}

BbScene *Blackboard::scene() const
{
    return dynamic_cast<BbScene *>(QGraphicsView::scene());
}

BbPointer *Blackboard::addPointer(const QString &pointerId, int x, int y)
{
    BbPointer *pointer = findChild<BbPointer *>(pointerId);
    if(!pointer)
    {
        pointer = new BbPointer(this);
        pointer->setObjectName(pointerId);
        pointer->setPixmap(dptr->pointerPixmap);
    }
    if(pointer->isHidden())
    {
        pointer->raise();
        pointer->show();
    }
    pointer->move(x-pointer->width()/2,y-pointer->height()/2);
    return nullptr;
}

void Blackboard::movePointer(const QString &pointerId, int x, int y)
{
    addPointer(pointerId,x,y);
}

void Blackboard::hidePointer(const QString &pointerId)
{
    BbPointer *pointer = findChild<BbPointer *>(pointerId);
    if(pointer)
    {
        pointer->hide();
    }
}

void Blackboard::setToolCursor(const BbToolType &tool, const QCursor &cursor)
{
    dptr->cursors[tool] = cursor;
    if(toolType()==tool)
    {
        setCursor(cursor);
    }
}

QCursor Blackboard::toolCursor(const BbToolType &tool)
{
    if(dptr->cursors.contains(tool))
    {
        return dptr->cursors[tool];
    }
    else
    {
        return Qt::ArrowCursor;
    }
}

void Blackboard::toToolCursor(const BbToolType &tool)
{
    setCursor(toolCursor(tool));
}

void Blackboard::revertToolCursor()
{
    setCursor(toolCursor(toolType()));
}

void Blackboard::setPointerPixmap(const QPixmap &pixmap)
{
    dptr->pointerPixmap = pixmap;
    setToolCursor(BBTT_Pointer,QCursor(pixmap,pixmap.width()/2,pixmap.height()/2));
}

void Blackboard::setScroll(int x,int y)
{
    dptr->scrollValue.setX(x);
    dptr->scrollValue.setY(y);
    disconnect(this->horizontalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollXChanged);
    disconnect(this->verticalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollYChanged);
    horizontalScrollBar()->setValue(x);
    verticalScrollBar()->setValue(y);
    connect(this->horizontalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollXChanged);
    connect(this->verticalScrollBar(), &QScrollBar::valueChanged,this, &Blackboard::onScrollYChanged);
}

QPoint Blackboard::getScrollValue()
{
    return dptr->scrollValue;
}

void Blackboard::setCanvasId(const QString &id)
{
    dptr->cid = id;
}

QString Blackboard::canvasId() const
{
    return dptr->cid;
}

qreal Blackboard::scaleRatio()
{
    return dptr->scaleRatio;
}

QSize Blackboard::orginalSize()
{
    return dptr->orginalSize;
}

int Blackboard::orginalWidth()
{
    return dptr->orginalSize.width();
}

int Blackboard::orginalHeight()
{
    return dptr->orginalSize.height();
}

void Blackboard::setOrginalSize(int width, int height)
{
    dptr->orginalSize.setWidth(width);
    dptr->orginalSize.setHeight(height);
}

void Blackboard::setOrginalSize(const QSize &size)
{
    setOrginalSize(size.width(),size.height());
}

void Blackboard::resizeEvent(QResizeEvent *event)
{
    if(dptr->orginalSize.width() == -1)
    {
        dptr->orginalSize = event->size();
        scene()->setSceneRect(0,0, dptr->canvasSize.width(), dptr->canvasSize.height());
    }
    else
    {
        qreal orginalSizeRatio = qreal(dptr->orginalSize.width()) / dptr->orginalSize.height();

        qreal eventSizeRatio = qreal(event->size().width()) / event->size().height();

        if(eventSizeRatio > orginalSizeRatio)
        {  // 被拉宽
            dptr->scaleRatio = qreal(event->size().height()) / dptr->orginalSize.height();
        }
        else
        {  // 被拉高
            dptr->scaleRatio = qreal(event->size().width()) / dptr->orginalSize.width();
        }

        resetTransform();

        scale(dptr->scaleRatio,dptr->scaleRatio);

        scene()->setSceneRect(0,0, dptr->canvasSize.width(), dptr->canvasSize.height());

        setScroll(dptr->scrollValue.x(),dptr->scrollValue.y());

        emit resized(float(dptr->scaleRatio));
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
            if(isEnabled())
            {
                emit pointerShown(dptr->mousePos);
            }
            return;
        }
        default:
        {
            if(isEnabled())
            {
                emit cursorShown(dptr->mousePos);
            }
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
            if(isEnabled())
            {
                emit pointerHidden(dptr->mousePos);
            }
            return;
        }
        default:
        {
            if(isEnabled())
            {
                emit cursorHidden(dptr->mousePos);
            }
            break;
        }
    }
}

void Blackboard::mousePressEvent(QMouseEvent *event)
{
    if(dptr->tabletActive)
        return;
    onMousePress(event->pos(),event->button());
    QGraphicsView::mousePressEvent(event);
}

void Blackboard::mouseMoveEvent(QMouseEvent *event)
{
    if(dptr->tabletActive)
        return;
    onMouseMove(event->pos());
    QGraphicsView::mouseMoveEvent(event);
}

void Blackboard::mouseReleaseEvent(QMouseEvent *event)
{
    if(dptr->tabletActive)
        return;
    onMouseRelease(event->pos(),event->button());
    QGraphicsView::mouseReleaseEvent(event);
}

void Blackboard::removeSelectedElement()
{
    scene()->removeSelected();
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

    dptr->canvasSize.setWidth(width);
    dptr->canvasSize.setHeight(height);
}

void Blackboard::setControlEnable(bool enable)
{
    scene()->setControlEnable(enable);

    setAttribute(Qt::WA_TransparentForMouseEvents, !enable);
}

void Blackboard::removeSelectedItems()
{
    auto inner = scene();
    if(inner)
    {
        inner->removeSelected();
    }
}

void Blackboard::remove(IItemIndex *item)
{
    auto inner = scene();
    if(inner)
    {
        inner->remove(item);
    }
}

void Blackboard::add(IItemIndex *item)
{
    auto inner = scene();
    if(inner)
    {
        inner->add(item);
    }
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
    dptr->scrollValue.setX(x);
    dptr->scrollValue.setY(y);
    emit scrolled(x, y);
}

void Blackboard::clearItems()
{
    scene()->clearItems();
}

IItemIndex *Blackboard::readItemData(BbItemData *itemData)
{
    return scene()->readItemData(itemData);
}

void Blackboard::onToolChanged(BbToolType previous)
{
    switch(previous)
    {
        case BBTT_Pointer:
        {
            emit pointerHidden(dptr->mousePos);
            break;
        }
        default:
        {
            break;
        }
    }
    BbToolType current = toolType();
    setCursor(toolCursor(current));
    switch(current)
    {
        case BBTT_Pointer:
        {
            emit pointerShown(dptr->mousePos);
            break;
        }
        default:
        {
            break;
        }
    }
    emit toolChanged(previous,current);
}

BbToolType Blackboard::toolType()
{
    return scene()->toolType();
}

BbItemData *Blackboard::toolSettings(const BbToolType &toolType)
{
    auto itr = dptr->toolSettings.find(toolType);
    if(itr != dptr->toolSettings.end())
    {
        return itr.value();
    }
    auto settings = BbHelper::createToolSettings(toolType);
    if(settings)
    {
        dptr->toolSettings.insert(toolType,settings);
    }
    return settings;
}

BbItemImage *Blackboard::addImageItem(
        const qreal &width, const qreal &height)
{
    return scene()->addImageItem(width, height);
}

BbItemImage *Blackboard::addImageItem(const QPixmap &pixmap)
{
    return scene()->addImageItem(pixmap);
}

BbItemImage *Blackboard::addImageItem(
        const qreal &width,
        const qreal &height,
        const QPixmap &pixmap)
{
    return scene()->addImageItem(
                width, height, pixmap);
}

BbItemImage *Blackboard::addImageItemWithPath(
        const QString &path)
{
    return scene()->addImageItemWithPath(path);
}

BbItemImage *Blackboard::addImageItemWithPath(
        const qreal &width,
        const qreal &height,
        const QString &path)
{
    return scene()->addImageItemWithPath(width,height,path);
}

BbItemImage *Blackboard::addImageItemWithUrl(
        const qreal &width,
        const qreal &height,
        const QString &url)
{
    return scene()->addImageItemWithUrl(width,height,url);
}

void Blackboard::selectedAll()
{
    scene()->selectedAll();
}

void Blackboard::deselectAll()
{
    scene()->deselectAll();
}

void Blackboard::copyItems()
{
    scene()->copyItems();
}

void Blackboard::pasteItems()
{
    scene()->pasteItems();
}

QSizeF Blackboard::backgroundSize() const
{
    return scene()->backgroundSize();
}

bool Blackboard::hasBackground() const
{
    return scene()->hasBackground();
}

void Blackboard::setBackground(const QPixmap &pixmap)
{
    scene()->setBackground(pixmap);
}

void Blackboard::clearBackground()
{
    scene()->clearBackground();
}

void Blackboard::writeStream(QDataStream &stream)
{
    scene()->writeStream(stream);
}

void Blackboard::readStream(QDataStream &stream)
{
    scene()->readStream(stream);
}

qreal Blackboard::orginalRatio(){return static_cast<qreal>(dptr->orginalSize.width())/dptr->orginalSize.height();  }

QSize Blackboard::canvasSize(){return dptr->canvasSize;}

int Blackboard::canvasWidth(){return dptr->canvasSize.width();}

int Blackboard::canvasHeight(){return dptr->canvasSize.height();}

void Blackboard::onMousePress(const QPoint &pos, const Qt::MouseButton &button)
{
    dptr->mousePos = pos;
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        case BBTT_Picker:
        {
            break;
        }
        default:
        {
            if(button == Qt::LeftButton)
            {
                emit cursorHidden(dptr->mousePos);
            }
            break;
        }
    }
}


void Blackboard::onMouseMove(const QPoint &pos)
{
    dptr->mousePos = pos;
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        {
            emit pointerMoving(dptr->mousePos);
            return;
        }
        case BBTT_Picker:
        {
            emit cursorMoving(dptr->mousePos);
            break;
        }
        default:
        {
            if(!scene()->isMouseLeftButtonDown())
            {
                emit cursorMoving(dptr->mousePos);
            }
            break;
        }
    }
}

void Blackboard::onMouseRelease(const QPoint &pos, const Qt::MouseButton &button)
{
    dptr->mousePos = pos;
    switch(scene()->toolType())
    {
        case BBTT_Pointer:
        case BBTT_Picker:
            break;
        default:
        {
            if(button == Qt::LeftButton)
            {
                emit cursorShown(dptr->mousePos);
            }
            break;
        }
    }
}


bool Blackboard::eventFilter(QObject *object, QEvent *event)
{
    if(object == qApp){
        switch(event->type()){
        case QEvent::TabletEnterProximity:
            dptr->tabletActive = true;
            break;
        case QEvent::TabletLeaveProximity:
            dptr->tabletActive = false;
            break;
        default:
            break;
        }
    }
    return false;
}

void Blackboard::tabletEvent(QTabletEvent *event)
{
    switch(event->type())
    {
    case QEvent::TabletPress:
    {
        onMousePress(event->pos(),event->button());
        QMouseEvent mouseEvent(QEvent::MouseMove,
                    event->pos(),
                    event->button(),
                    event->buttons(),
                    event->modifiers());
        QGraphicsView::mousePressEvent(&mouseEvent);
        break;
    }
    case QEvent::TabletMove:
    {
        onMouseMove(event->pos());
        QMouseEvent mouseEvent(QEvent::MouseMove,
                    event->pos(),
                    event->button(),
                    event->buttons(),
                    event->modifiers());
        QGraphicsView::mouseMoveEvent(&mouseEvent);
        break;
    }
    case QEvent::TabletRelease:
    {
        onMouseRelease(event->pos(),event->button());
        QMouseEvent mouseEvent(QEvent::MouseButtonRelease,
                    event->pos(),
                    event->button(),
                    event->buttons(),
                    event->modifiers());
        QGraphicsView::mouseReleaseEvent(&mouseEvent);
        break;
    }
    default:
        break;
    }
}

void Blackboard::moveEvent(QMoveEvent *event)
{
    QGraphicsView::moveEvent(event);
    emit moved();
}
