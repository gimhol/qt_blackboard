#include "Blackboard.h"
#include "BbScene.h"
#include "BbPointer.h"

#include <QResizeEvent>
#include <QDebug>
#include <QTimer>
#include <QScrollBar>
#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLContext>

class BlackboardPrivate
{
public:
    bool resizing = false;
    qreal scaleRatio = 1;
    QSizeF canvasSize;
    QPixmap pointerPixmap;
    QPoint mousePos;
    QPointF scrollValue;
    QString cid;

    QMap<BbToolType,BbItemData*> toolSettings;
    QMap<BbToolType,QCursor> cursors;
    QMap<QString, QPoint> lazerPenPositions;
    bool tabletActive = false;
    BbFactory *factory = nullptr;
    QPointF tabletPenPos;
};

static BbFactory *_defaultFactory = BbFactory::get();

void Blackboard::setDefaultFactory(BbFactory *factory)
{
    _defaultFactory = factory;
}

BbFactory *Blackboard::defaultFactory()
{
    return _defaultFactory;
}

void Blackboard::setFactory(BbFactory *factory)
{
    dptr->factory = factory;
}

BbFactory *Blackboard::factory()
{
    return dptr->factory?dptr->factory:defaultFactory();
}

Blackboard::Blackboard(QWidget *parent):
    QGraphicsView(parent),
    dptr(new BlackboardPrivate)
{
    setScene(defaultFactory()->createScene(this));

    setMouseTracking(true);
    setTabletTracking(true);
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
    if(dptr->factory)
    {
        delete dptr->factory;
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

void Blackboard::setScroll(const qreal &x,const qreal &y)
{
    dptr->scrollValue.rx() = x;
    dptr->scrollValue.ry() = y;
    auto hsb = horizontalScrollBar();
    auto vsb = verticalScrollBar();
    disconnect(hsb,&QScrollBar::valueChanged,this,&Blackboard::onScrollXChanged);
    disconnect(vsb,&QScrollBar::valueChanged,this,&Blackboard::onScrollYChanged);
    hsb->setValue(x * width() / 100);
    vsb->setValue(y * width() / 100);
    connect(hsb,&QScrollBar::valueChanged,this,&Blackboard::onScrollXChanged);
    connect(vsb,&QScrollBar::valueChanged,this,&Blackboard::onScrollYChanged);
}

QPointF Blackboard::getScrollValue()
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

void Blackboard::resizeEvent(QResizeEvent *event)
{
#ifdef QT_DEBUG
    qInfo() << __FUNCTION__ << "begin: " << event->oldSize() << "to" << event->size();
#endif

    dptr->resizing = true;

    if(dptr->canvasSize.width() <= 0.5) {
        dptr->canvasSize = event->size();
    }
    else {
        dptr->scaleRatio = qreal(event->size().width()) / dptr->canvasSize.width();
        resetTransform();
        scale(dptr->scaleRatio,dptr->scaleRatio);
    }
    QGraphicsView::resizeEvent(event);

    dptr->resizing = false;

#ifdef QT_DEBUG
    qInfo() << __FUNCTION__ << "end: " << event->oldSize() << "to" << event->size();
#endif
}

void Blackboard::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
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
    Q_UNUSED(event)
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

void Blackboard::setCanvasSize(const QSizeF &size)
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << size;
#endif
    scene()->setSceneRect(0,0, size.width(), size.height());
    dptr->canvasSize = size;
}

void Blackboard::setCanvasSize(const qreal &width, const qreal &height)
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << width << 'x' << height;
#endif
    scene()->setSceneRect(0,0, width, height);
    dptr->canvasSize.rwidth() = width;
    dptr->canvasSize.rheight() = height;
}

void Blackboard::removeSelectedItems()
{
    auto inner = scene();
    if(inner)
    {
        inner->removeSelected();
    }
}

void Blackboard::remove(QString lid)
{
    auto inner = scene();
    if(inner)
        inner->remove(lid);
}

void Blackboard::remove(IItemIndex *item)
{
    auto inner = scene();
    if(inner)
        inner->remove(item);
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
#ifdef QT_DEBUG
    qInfo() << __FUNCTION__
            << "begin."
            << "x:" << x
            << "y:" << y
            << "resizing:" << (dptr->resizing?"true":"false");
#endif
    dptr->scrollValue.rx() = 100.0 * x / width();
    dptr->scrollValue.ry() = 100.0 * y / width();
    emit scrolled(dptr->scrollValue.rx(),
                  dptr->scrollValue.ry());
#ifdef QT_DEBUG
    qInfo() << __FUNCTION__ << "end.";
#endif
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
    auto settings = defaultFactory()->createToolSettings(toolType);
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

void Blackboard::cutItems()
{
    scene()->cutItems();
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

void Blackboard::addBackground(QString id, const QPixmap &pixmap)
{
    scene()->addBackground(id,pixmap);
}

void Blackboard::addBackground(QString id, QGraphicsItem *graphicsItem)
{
    scene()->addBackground(id,graphicsItem);
}

QString Blackboard::addBackground(const QPixmap &pixmap)
{
    return scene()->addBackground(pixmap);
}

QString Blackboard::addBackground(QGraphicsItem *graphicsItem)
{
    return scene()->addBackground(graphicsItem);
}

void Blackboard::clearBackground()
{
    scene()->clearBackground();
}

int Blackboard::backgroundCount()
{
    return scene()->backgroundCount();
}

void Blackboard::removeBackground(int index)
{
    scene()->removeBackground(index);
}

void Blackboard::removeBackground(QString id)
{
    scene()->removeBackground(id);
}

QGraphicsItem *Blackboard::background(int index)
{
    return scene()->background(index);
}

QGraphicsItem *Blackboard::background(QString id)
{
    return scene()->background(id);
}

void Blackboard::layBackgrounds()
{
    return scene()->layBackgrounds();
}

QRectF Blackboard::backgroundRect()
{
    return scene()->backgroundRect();
}

QSizeF Blackboard::backgroundSize()
{
    return backgroundRect().size();
}

QList<QPair<QString, QGraphicsItem *> > Blackboard::backgrounds()
{
    return scene()->backgrounds();
}

void Blackboard::groupUp()
{
    return scene()->groupUp();
}

void Blackboard::dismiss()
{
    return scene()->dismiss();
}

QPointF Blackboard::tabletPenPos()
{
    return dptr->tabletPenPos;
}

bool Blackboard::tabletActive()
{
    return dptr->tabletActive;
}

void Blackboard::writeStream(QDataStream &stream)
{
    scene()->writeStream(stream);
}

void Blackboard::readStream(QDataStream &stream)
{
    scene()->readStream(stream);
}

QJsonObject Blackboard::toJsonObject()
{
    return scene()->toJsonObject();
}

void Blackboard::fromJsonObject(const QJsonObject &jobj)
{
    scene()->fromJsonObject(jobj);
}

QSizeF Blackboard::canvasSize(){return dptr->canvasSize;}

qreal Blackboard::canvasWidth(){return dptr->canvasSize.rwidth();}

qreal Blackboard::canvasHeight(){return dptr->canvasSize.rheight();}

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
    auto updateTabletPenPos = [&](){
        auto p = event->posF();
        p.rx() += horizontalScrollBar()->value();
        p.ry() += verticalScrollBar()->value();
        dptr->tabletPenPos = transform().inverted().map(p);
    };
    switch(event->type())
    {
    case QEvent::TabletPress:
    {
        onMousePress(event->pos(),event->button());
        updateTabletPenPos();
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
        static QPointF p;
        if(p == event->posF())
            break;
        p = event->posF();
        updateTabletPenPos();
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
        updateTabletPenPos();
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
