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
        onToolChanged(_toolType, toolType);
        blackboard()->onToolChanged(_toolType, toolType);
        _toolType = toolType;
    }
}

BbToolType BbScene::toolType()
{
    return _toolType;
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
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        _mouseLeftButtonDown = true;

        switch(_toolType)
        {
            case BBTT_Picker:
            {
                for(auto item: selectedItems())
                {
                    IItemIndex *idx = dynamic_cast<IItemIndex *>(item);
                    if(idx)
                    {
                        auto data = idx->data();
                        data->prevX = data->x;
                        data->prevY = data->y;
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
                break;
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
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
            if(_mouseLeftButtonDown)
            {
                QGraphicsScene::mouseMoveEvent(event);
                if(event->isAccepted())
                {
                    emitItemMovingSignals();
                }
                else
                {
                    pickingItems(_mousePos);
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
    QGraphicsScene::mouseReleaseEvent(event);
    _mousePos = event->scenePos();
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        _mouseLeftButtonDown = false;
        switch(_toolType)
        {
            case BBTT_Picker:
            {
                _pickerRect->hide();
                emitItemMovedSignals();
                break;
            }
            default:
            {
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
    QGraphicsScene::mouseDoubleClickEvent(event);
    switch(_toolType)
    {
        case BBTT_Picker:
        {
            for(auto item : items()){
                BbItemText *text = dynamic_cast<BbItemText *>(item);
                if(text && text->boundingRect().contains(event->scenePos()-item->pos())){
                    text->setTextInteractionFlags(Qt::TextEditorInteraction);
                    text->setFocus();
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void BbScene::keyPressEvent(QKeyEvent *e)
{
    if(_lastModifiers != e->modifiers())
    {
        if(_curItemIndex)
        {
            _curItemIndex->modifiersChanged(e->modifiers());
        }
        _lastModifiers = e->modifiers();
    }
    _onlyShiftDown = e->modifiers() ==  Qt::ShiftModifier;
    _onlyAltDown = e->modifiers() ==  Qt::AltModifier;
    _onlyCtrlDown = e->modifiers() ==  Qt::ControlModifier;
    QGraphicsScene::keyPressEvent(e);
}

void BbScene::keyReleaseEvent(QKeyEvent *e)
{
    if(_lastModifiers != e->modifiers())
    {
        if(_curItemIndex)
        {
            _curItemIndex->modifiersChanged(e->modifiers());
        }
        _lastModifiers = e->modifiers();
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
    return _backgroundSize;
}

bool BbScene::hasBackground() const
{
    return _backgroundItem != nullptr;
}

void BbScene::setBackground(const QPixmap &pixmap)
{
    clearBackground();

    QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem();
    pixmapItem->setZValue(INT_MIN);
    qreal ratio = width() / pixmap.width();
    pixmapItem->setPixmap(pixmap);
    pixmapItem->setScale(ratio);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    addItem(pixmapItem);

    _backgroundSize = pixmap.size();
    _backgroundItem = pixmapItem;
}

void BbScene::clearBackground()
{
    if(_backgroundItem)
    {
        QGraphicsScene::removeItem(_backgroundItem);
        delete _backgroundItem;
        _backgroundItem = nullptr;
        _backgroundSize.setWidth(-1);
        _backgroundSize.setHeight(-1);
    }
}

void BbScene::pickingItems(const QPointF &mousePos)
{
    _pickerRect->setRect(
                std::min(mousePos.x(),_mouseBeginPos.x()),
                std::min(mousePos.y(),_mouseBeginPos.y()),
                std::abs(mousePos.x()-_mouseBeginPos.x()),
                std::abs(mousePos.y()-_mouseBeginPos.y())
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

bool BbScene::onlyShiftDown()
{
    return _onlyShiftDown;
}

void BbScene::onToolChanged(BbToolType previous, BbToolType current)
{
    switch(previous)
    {
        case BBTT_Picker:
        {
            for(auto item: items())
            {
                if(!isPrivateItem(item))
                {
                    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,false);
                    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,false);
                    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,false);
                }
            }
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
    switch(current)
    {
        case BBTT_Picker:
        {
            for(auto item: items())
            {
                if(!isPrivateItem(item))
                {
                    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
                    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
                    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable);
                }
            }
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
        auto streamReader = dynamic_cast<IStreamR*>(index);
        if(streamReader)
        {
            streamReader->readStream(stream);
        }
        auto item = dynamic_cast<QGraphicsItem*>(index);
        if(item)
        {
            addItem(item);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,_toolType == BBTT_Picker);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,_toolType == BBTT_Picker);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,_toolType == BBTT_Picker);
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
        index->added();
        QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(index);
        if(item)
        {
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,_toolType == BBTT_Picker);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,_toolType == BBTT_Picker);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,_toolType == BBTT_Picker);
            QGraphicsScene::addItem(item);
        }
        else
        {
            qWarning() << "[BlackboardScene::add] item is not a 'QGraphicsItem'! what happen?!";
        }
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
    auto index = enumSelected(nullptr);
    while(index)
    {
        auto next = index->next;
        remove(index);
        index = next;
    }
}

bool BbScene::isPrivateItem(QGraphicsItem *item)
{
    return item == _pickerRect || item == _backgroundItem;
}

void BbScene::readItemData(BbItemData *itemData)
{
    auto item = BbHelper::createItem(itemData);
    if(item)
    {
        add(item);
        item->repaint();
    }
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

void BbScene::addImageItem(const QPixmap &pixmap)
{
    auto item = new BbItemImage();
    item->setPixmap(pixmap);
    add(item);
    item->setZValue(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
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
