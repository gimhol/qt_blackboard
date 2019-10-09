#include "BbScene.h"
#include "Blackboard.h"
#include "BbItemPen.h"
#include "BbItemPenData.h"
#include "BbItemText.h"
#include "BbItemTextData.h"
#include "BbItemData.h"
#include "BbItemStraight.h"
#include "BbItemStraightData.h"
#include "BbItemRect.h"
#include "BbItemRectData.h"
#include "BbItemEllipse.h"
#include "BbItemEllipseData.h"
#include "BbItemTriangle.h"
#include "BbItemTriangleData.h"
#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BBItemEventType.h"
#include "BbHelper.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QDateTime>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#ifdef WIN32
#include <Windows.h>
#pragma comment(lib, "user32")
#endif

BbScene::BbScene(Blackboard *parent):
    QGraphicsScene(parent)
{
    _pickerRect = new QGraphicsRectItem();
    _pickerRect->hide();
    _pickerRect->setPen(QPen(QColor(100,100,255),
                             1,
                             Qt::DashLine,
                             Qt::RoundCap,
                             Qt::MiterJoin));
    _pickerRect->setEnabled(false);
    QGraphicsScene::addItem(_pickerRect);
}

BbScene::~BbScene()
{
    QGraphicsScene::removeItem(_pickerRect);
    delete _pickerRect;
    _pickerRect = nullptr;
    clearBackground();
}

Blackboard *BbScene::blackboard(){
    return dynamic_cast<Blackboard *>(views()[0]);
}

void BbScene::setToolType(BbToolType toolType)
{
    _mouseLeftButtonDown = false;
    if(_toolType != toolType)
    {
        auto prev = _toolType;
        _toolType = toolType; // 在下面的

        onToolChanged(prev);
        blackboard()->onToolChanged(prev);
    }
}

BbToolType BbScene::toolType()
{
    return _toolType;
}

IItemIndex *BbScene::enumAll(std::function<bool (IItemIndex *, int)> job)
{
    IItemIndex *first = nullptr;
    IItemIndex *current = nullptr;
    int i = 0;
    for(auto item: items())
    {
        IItemIndex *index = dynamic_cast<IItemIndex *>(item);
        if(index)
        {
            if(first)
            {
                index->last = current;
                current->next = index;
            }
            else
            {
                index->last = nullptr;
                first = index;
            }
            index->next = nullptr;
            if(job && job(index,i))
            {
                break;
            }
            current = index;
            i++;
        }
    }
    return first;
}

IItemIndex *BbScene::enumSelected(std::function<bool(IItemIndex *,int)> job)
{
    IItemIndex *first = nullptr;
    IItemIndex *current = nullptr;
    int i = 0;
    for(auto item: selectedItems())
    {
        IItemIndex *index = dynamic_cast<IItemIndex *>(item);
        if(index)
        {
            if(first)
            {
                index->last = current;
                current->next = index;
            }
            else
            {
                index->last = nullptr;
                first = index;
            }
            index->next = nullptr;
            if(job && job(index,i))
            {
                break;
            }
            current = index;
            i++;
        }
    }
    return first;
}

void BbScene::removeSelected()
{
    auto enumJob = [&](IItemIndex *index,int i)
    {
        Q_UNUSED(i);
        emit blackboard()->itemChanged(BBIET_itemDelete,index);
        return false;
    };
    IItemIndex *index = enumSelected(enumJob);
    emit blackboard()->multipleItemChanged(BBIET_itemDelete,index);
    while(index)
    {
        auto next = index->next;
        remove(index);
        index = next;
    }
}

void BbScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!_controlEnable)
    {
        return;
    }
    _mouseBeginPos = event->scenePos();
    _mousePos = event->scenePos();
    if(event->button() == Qt::MouseButton::LeftButton && !_mouseLeftButtonDown)
    {
        _mouseLeftButtonDown = true;
        switch(_toolType)
        {
            case BBTT_Picker:
            {
                bool interrupt = false;
                if(_editingItemIndex)
                {
                    interrupt = _editingItemIndex->mouseDown(_mousePos);
                }
                if(!interrupt)
                {
                    QGraphicsScene::mousePressEvent(event);
                    bool editing = false;
                    bool response = false;
                    for(auto item: selectedItems())
                    {
                        IItemIndex *idx = dynamic_cast<IItemIndex *>(item);
                        if(idx)
                        {
                            auto pos = event->scenePos();
                            auto data = idx->data();
                            data->prevX = data->x;
                            data->prevY = data->y;
                            if(_editingItemIndex == idx && item->isUnderMouse())
                            {
                                editing = true;
                            }
                            if(!response && item->isUnderMouse())
                            {
                                response = idx->clicked(pos);
                            }
                        }
                    }
                }
                break;
            }
            default:
            {
                if(!_curItemIndex)
                {
                    auto item = BbHelper::createItem(_toolType);
                    if(item)
                    {
                        add(item);
                        item->toolDown(_mousePos);
                    }
                }
                else
                {
                    _curItemIndex->toolDown(_mousePos);
                }
                QGraphicsScene::mousePressEvent(event);
                break;
            }
        }
    }
}

void BbScene::emitItemMovingSignals()
{
    auto enumJob = [&](IItemIndex *index,int i){
        Q_UNUSED(i);
        index->data()->updatePostion(index);
        emit blackboard()->itemChanged(BBIET_itemMoving,index);
        return false;
    };
    auto index = enumSelected(enumJob);
    emit blackboard()->multipleItemChanged(BBIET_itemMoving,index);
}

void BbScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!_controlEnable)
    {
        return;
    }
    _mousePos = event->scenePos();
    switch(_toolType)
    {
        case BBTT_Picker:
        {
            auto interrupt = false;
            if(_editingItemIndex)
            {
                interrupt = _editingItemIndex->mouseMove(_mousePos);
            }
            if(!interrupt)
            {
                QGraphicsScene::mouseMoveEvent(event); // 调用函数才能拖动。
                if(_mouseLeftButtonDown)
                {
                    /*
                     * NOTE: 拖动item会使得此event被Accepted，故在此发出移动信号。
                     *      但如果有其他被Accepted的情况就不应该这么做。但现在还没发现。
                     */
                    if(event->isAccepted())
                    {
                        emitItemMovingSignals();
                    }
                    else // 没拖动任何东西，在这里进行框选item的工作。
                    {
                        pickingItems(_mousePos);
                    }
                }
            }
            break;
        }
        default:
        {
            if(_curItemIndex && _mouseLeftButtonDown)
            {
                _curItemIndex->toolDraw(_mousePos);
            }
            break;
        }
    }
}

void BbScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(!_controlEnable){
        return;
    }
    _mousePos = event->scenePos();
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        _mouseLeftButtonDown = false;
        switch(_toolType)
        {
            case BBTT_Picker:
            {
                auto interrupt = false;
                if(_editingItemIndex)
                {
                    interrupt = _editingItemIndex->mouseRelease(_mousePos);
                }
                if(!interrupt)
                {
                    QGraphicsScene::mouseReleaseEvent(event);
                    emitItemMovedSignals();
                }
                _pickerRect->hide();
                break;
            }
            default:
            {
                QGraphicsScene::mouseReleaseEvent(event);
                if(_curItemIndex)
                {
                    _curItemIndex->toolDone(_mousePos);
                }
                break;
            }
        }
    }
}

void BbScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!_controlEnable)
    {
        return;
    }
    switch(_toolType)
    {
        case BBTT_Picker:
        {
            auto pos = event->scenePos();
            for(auto item : items())
            {
                IItemIndex *index = dynamic_cast<IItemIndex *>(item);
                if(index && item->isUnderMouse() && index->doubleClicked(pos))
                {
                    break;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
    QGraphicsScene::mouseDoubleClickEvent(event);
}

void BbScene::keyPressEvent(QKeyEvent *e)
{
    if(_modifiers != e->modifiers())
    {
        if(_editingItemIndex)
        {
            _editingItemIndex->modifiersChanged(e->modifiers());
        }
        if(_curItemIndex)
        {
            _curItemIndex->modifiersChanged(e->modifiers());
        }
        _modifiers = e->modifiers();
    }
    _onlyShiftDown = e->modifiers() ==  Qt::ShiftModifier;
    _onlyAltDown = e->modifiers() ==  Qt::AltModifier;
    _onlyCtrlDown = e->modifiers() ==  Qt::ControlModifier;
    QGraphicsScene::keyPressEvent(e);
}

void BbScene::keyReleaseEvent(QKeyEvent *e)
{
    if(_modifiers != e->modifiers())
    {
        if(_editingItemIndex)
        {
            _editingItemIndex->modifiersChanged(e->modifiers());
        }
        if(_curItemIndex)
        {
            _curItemIndex->modifiersChanged(e->modifiers());
        }
        _modifiers = e->modifiers();
    }
    switch(e->key())
    {
        case Qt::Key_Shift:
        {
            _onlyShiftDown = false;
            break;
        }
        case Qt::Key_Control:
        {
            _onlyCtrlDown = false;
            break;
        }
        case Qt::Key_Alt:
        {
            _onlyAltDown = false;
            break;
        }
        default:
        {
            break;
        }
    }
    QGraphicsScene::keyReleaseEvent(e);
}

void BbScene::copyItems()
{
    QClipboard *clipboard = QApplication::clipboard();

    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);
    for(auto item: selectedItems())
    {
        IItemIndex *index = dynamic_cast<IItemIndex *>(item);
        IStreamW *streamWriter = dynamic_cast<IStreamW *>(item);
        if(!index || !streamWriter)
        {
            continue;
        }
        dataStream << static_cast<int>(index->toolType());
        streamWriter->writeStream(dataStream);
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setData("nsb/blackboard-items",data);
    clipboard->setMimeData(mimeData);
}

void BbScene::pasteItems()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();


    for(auto item: selectedItems())
    {
        item->setSelected(false);
    }
    if(mimeData && mimeData->hasFormat("nsb/blackboard-items"))
    {
        auto data = mimeData->data("nsb/blackboard-items");
        QDataStream dataStream(&data, QIODevice::ReadOnly);
        IItemIndex *first = nullptr;
        IItemIndex *current = nullptr;
        while(!dataStream.atEnd())
        {
            auto index = copyItemFromStream(dataStream);
            if(index)
            {
                if(first)
                {
                    current->next = index;
                    index->last = current;
                }
                else
                {
                    index->last = nullptr;
                    first = index;
                }
                current = index;
                index->next = nullptr;
                auto item = dynamic_cast<QGraphicsItem*>(index);
                if(item)
                {
                    item->setSelected(true);
                }
            }
        }
        if(first)
        {
            emit blackboard()->multipleItemChanged(BBIET_itemPaste,first);
            current = first;
            while(current)
            {
                auto next = current->next;
                emit blackboard()->itemChanged(BBIET_itemPaste,current);
                current = next;
            }
        }
    }
}

QSizeF BbScene::backgroundSize() const
{
    return QSizeF(width(),_backgroundHeight);
}

bool BbScene::hasBackground() const
{
    return !_backgrounds.empty();
}

void BbScene::setBackground(const QPixmap &pixmap)
{
    clearBackground();
    addBackground(pixmap);
}

void BbScene::addBackground(const QPixmap &pixmap)
{
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem();
    pixmapItem->setZValue(INT_MIN);
    qreal ratio = width() / pixmap.width();
    pixmapItem->setPixmap(pixmap);
    pixmapItem->setScale(ratio);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    pixmapItem->setPos(0,_backgroundHeight);
    addItem(pixmapItem);

    _backgroundHeight += pixmap.height()*ratio;
    _backgrounds.push_back(pixmapItem);
}

void BbScene::clearBackground()
{
    if(hasBackground())
    {
        for(auto background: _backgrounds)
        {
            QGraphicsScene::removeItem(background);
            delete background;
        }
    }
    _backgroundHeight = 0;
}

void BbScene::pickingItems(const QPointF &mousePos)
{
    _pickerRect->setRect(
                (std::min)(mousePos.x(),_mouseBeginPos.x()),
                (std::min)(mousePos.y(),_mouseBeginPos.y()),
                (std::abs)(mousePos.x()-_mouseBeginPos.x()),
                (std::abs)(mousePos.y()-_mouseBeginPos.y())
                );
    _pickerRect->show();
    for(auto item: items())
    {
        IItemIndex* itemIndex = dynamic_cast<IItemIndex*>(item);
        if(itemIndex)
        {
            bool collided = item->collidesWithItem(_pickerRect);
            if(collided != item->isSelected())
            {
                item->setSelected(collided);
                emit blackboard()->itemSelected(itemIndex,collided);
            }
        }
    }
}

QString BbScene::generatItemId() const
{
    if(_itemIdGenerator != nullptr)
    {
        return _itemIdGenerator();
    }
    return QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
}

IItemIndex *BbScene::currentItem()
{
    return _curItemIndex;
}

void BbScene::setCurrentItem(IItemIndex *item)
{
    _curItemIndex = item;
}

void BbScene::unsetCurrentItem(IItemIndex *item)
{
    if(_curItemIndex == item)
    {
        _curItemIndex = nullptr;
    }
}

IItemIndex *BbScene::editingItem()
{
    return _editingItemIndex;
}

void BbScene::setEditingItem(IItemIndex *item)
{
    _editingItemIndex = item;
}

void BbScene::unsetEditingItem(IItemIndex *item)
{
    if(_editingItemIndex == item)
    {
        _editingItemIndex = nullptr;
    }
}

bool BbScene::onlyShiftDown()
{
    return _onlyShiftDown;
}

Qt::KeyboardModifiers BbScene::modifiers()
{
#ifdef WIN32
    /*
    Note: 未获得焦点时，收不到keyPressEvent与keyReleaseEvent。
    导致首次绘图，图形响应不了功能键。
    所以这里直接使用win32的api来获取功能键状态。
    - Gim
    */
    if(GetKeyState(VK_SHIFT) < 0 ||
            GetKeyState(VK_LSHIFT) < 0 ||
            GetKeyState(VK_RSHIFT) < 0)
    {
        _modifiers |= Qt::ShiftModifier;
    }
    if(GetKeyState(VK_CONTROL) < 0 ||
            GetKeyState(VK_LCONTROL) < 0 ||
            GetKeyState(VK_RCONTROL) < 0)
    {
        _modifiers |= Qt::ControlModifier;
    }
    if(GetKeyState(VK_MENU) < 0 ||
            GetKeyState(VK_LMENU) < 0 ||
            GetKeyState(VK_RMENU) < 0)
    {
        _modifiers |= Qt::AltModifier;
    }
#endif
    return _modifiers;

}

void BbScene::setItemPicking(bool picking)
{
    for(auto item: items())
    {
        if(!isPrivateItem(item))
        {
            auto idx = dynamic_cast<IItemIndex*>(item);
            if(idx && !idx->isEditing())
            {
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,picking);
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,picking);
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,picking);
            }
        }
    }
}

void BbScene::onToolChanged(BbToolType previous)
{
    switch(previous)
    {
        case BBTT_Picker:
        {
            _editingItemIndex = nullptr;
            setItemPicking(false);
            break;
        }
        default:
        {
            break;
        }
    }
    if(_curItemIndex)
    {
        _curItemIndex->toolDone(_mousePos);
    }
    switch(_toolType)
    {
        case BBTT_Picker:
        {
            setItemPicking(true);
            break;
        }
        default:
        {
            break;
        }
    }
}

IItemIndex *BbScene::readItemFromStream(QDataStream &stream)
{
    int type;
    stream >> type;
    auto index = BbHelper::createItem(static_cast<BbToolType>(type));
    if(index)
    {
        add(index);
        auto streamReader = dynamic_cast<IStreamR*>(index);
        if(streamReader)
        {
            streamReader->readStream(stream);
        }
    }
    return index;
}

void BbScene::selectedAll()
{
    if(_toolType == BBTT_Picker)
    {
        for(auto item: items())
        {
            IItemIndex* itemIndex = dynamic_cast<IItemIndex*>(item);
            if(!itemIndex)
            {
                continue;
            }
            item->setSelected(true);
        }
    }
}

void BbScene::deselectAll()
{
    for(auto item: selectedItems())
    {
        IItemIndex* itemIndex = dynamic_cast<IItemIndex*>(item);
        if(!itemIndex)
        {
            continue;
        }
        item->setSelected(false);
    }
}

void BbScene::remove(IItemIndex *index)
{
    if(index)
    {
        index->removed();
        if(_curItemIndex == index)
        {
            _curItemIndex = nullptr;
        }
        if(_editingItemIndex == index)
        {
            _editingItemIndex= nullptr;
        }
        auto item = dynamic_cast<QGraphicsItem*>(index);
        if(item)
        {
            QGraphicsScene::removeItem(item);
        }
        else
        {
            qWarning() << "[BlackboardScene::remove] item is not a 'QGraphicsItem'! what happen?!";
        }
        delete index;
    }
    else
    {
        qWarning() << "[BlackboardScene::remove] can not remove item, pointer is nullptr!";
    }
}

void BbScene::add(IItemIndex *index)
{
    if(index)
    {
        QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(index);
        if(item)
        {
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,_toolType == BBTT_Picker);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,_toolType == BBTT_Picker);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,_toolType == BBTT_Picker);
            QGraphicsScene::addItem(item);
            index->absolutize();
        }
        else
        {
            qWarning() << "[BlackboardScene::add] item is not a 'QGraphicsItem'! what happen?!";
        }
        index->added();
    }
    else
    {
        qWarning() << "[BlackboardScene::add] fail to add item, pointer is nullptr!";
    }
}

void BbScene::setControlEnable(bool enable)
{
    _controlEnable = enable;
}

void BbScene::clearItems()
{
    auto index = enumAll(nullptr);
    while(index)
    {
        auto next = index->next;
        remove(index);
        index = next;
    }
}

bool BbScene::isPrivateItem(QGraphicsItem *item)
{
    return item == _pickerRect || _backgrounds.contains(item);
}

IItemIndex *BbScene::readItemData(BbItemData *itemData)
{
    auto item = BbHelper::createItem(itemData);
    if(item)
    {
        add(item);
        item->repaint();
    }
    return item;
}

void BbScene::setItemIdGenerator(ItemIDGenerator itemIdGenerator)
{
    _itemIdGenerator = itemIdGenerator;
}

void BbScene::writeStream(QDataStream &stream)
{
    for(auto item: items())
    {
        IItemIndex *idx = dynamic_cast<IItemIndex *>(item);
        IStreamWR *wr = dynamic_cast<IStreamWR *>(item);
        if(idx && wr)
        {
            stream << static_cast<int>(idx->toolType());
            wr->writeStream(stream);
        }
    }
}

void BbScene::readStream(QDataStream &stream)
{
    while(!stream.atEnd())
    {
        readItemFromStream(stream);
    }
}

bool BbScene::isMouseLeftButtonDown()
{
    return _mouseLeftButtonDown;
}

BbItemImage *BbScene::addImageItem(const qreal &width, const qreal &height)
{
    auto item = new BbItemImage();
    add(item);
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItem(const QPixmap &pixmap)
{
    auto item = new BbItemImage();
    add(item);
    item->resize(pixmap.width(),pixmap.height());
    item->updatePrevPosition();
    item->setZ(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    item->setPixmap(pixmap);
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItem(const qreal &width, const qreal &height, const QPixmap &pixmap)
{
    auto item = new BbItemImage();
    add(item);
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    item->setPixmap(pixmap);
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItemWithPath(const QString &path)
{
    QPixmap pixmap(path);
    auto item = new BbItemImage();
    add(item);
    auto data = dynamic_cast<BbItemImageData*>(item->data());
    data->path = path;
    item->resize(pixmap.width(),pixmap.height());
    item->updatePrevPosition();
    item->setZ(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    item->setPixmap(pixmap);
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItemWithPath(const qreal &width, const qreal &height, const QString &path)
{
    QPixmap pixmap(path);
    auto item = new BbItemImage();
    add(item);
    auto data = dynamic_cast<BbItemImageData*>(item->data());
    data->path = path;
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    item->setPixmap(pixmap);
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItemWithUrl(const qreal &width, const qreal &height, const QString &url)
{
    auto item = new BbItemImage();
    add(item);
    auto data = dynamic_cast<BbItemImageData*>(item->data());
    data->url = url;
    item->resize(width,height);

    item->updatePrevPosition();
    item->setZ(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

IItemIndex *BbScene::copyItemFromStream(QDataStream &stream)
{
    static int time = 0;
    auto index = readItemFromStream(stream);
    if(index)
    {
        // maybe we need a better rule to make the 'copy' id.
        index->setId(QString("%1_%2").arg(index->id()).arg(time));
        auto item = dynamic_cast<QGraphicsItem*>(index);
        if(item)
        {
            item->setZValue(QDateTime::currentMSecsSinceEpoch());
            item->setPos(item->pos() + QPointF(10,10));
            index->data()->updatePostion(index);
            index->data()->prevX = index->data()->x;
            index->data()->prevY = index->data()->y;
        }
    }
    ++time;
    return index;
}

void BbScene::emitItemMovedSignals()
{
    bool moved = false;
    auto enumJob = [&](IItemIndex *index,int i)
    {
        if(i == 0)
        {
            auto item = dynamic_cast<QGraphicsItem *>(index);
            auto dx = std::abs(index->data()->prevX - item->x());
            auto dy = std::abs(index->data()->prevY - item->y());
            moved = dx >= 1 || dy >= 1;
            if(!moved)
            {
                return true;
            }
        }
        return false;
    };
    auto index = enumSelected(enumJob);
    emit blackboard()->multipleItemChanged(BBIET_itemMoving,index);
    if(moved)
    {
        emit blackboard()->multipleItemChanged(BBIET_itemMoved,index);
        while(index)
        {
            auto next = index->next;
            auto data = index->data();
            data->updatePostion(index);
            emit blackboard()->itemChanged(BBIET_itemMoved,index);
            data->prevX = data->x;
            data->prevY = data->y;
            index = next;
        }
    }
}
