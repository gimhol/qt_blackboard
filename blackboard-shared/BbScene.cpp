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
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#ifdef WIN32
#include <QDateTime>
#include <QTimer>
#include <Windows.h>
#pragma comment(lib, "user32")
#endif

#define GRAPHICS_ITEM_DATA_KEY_DELETING 1000

BbScene::BbScene(Blackboard *parent):
    QGraphicsScene(parent),
    _backgroundRect(0,0,0,0),
    _pickerTimer(new QTimer(this))
{
    _pickerTimer->setInterval(1000/30);
    connect(_pickerTimer,&QTimer::timeout,this,&BbScene::onPicking);
    _pickerRect = new QGraphicsRectItem();
    _pickerRect->hide();
    _pickerRect->setPen(QPen(QColor(100,100,255),
                             1,
                             Qt::DashLine,
                             Qt::RoundCap,
                             Qt::MiterJoin));
    _pickerRect->setEnabled(false);
    _pickerRect->setZValue(DBL_MAX);
    QGraphicsScene::addItem(_pickerRect);
}

BbScene::~BbScene()
{
    _pickerRect = nullptr;
    _curItemIndex = nullptr;
    _backgrounds.clear();
    /*
     * Note: 这里不需要delete 因为会item 会在clear中被delete。
     * 这里的delete可能会导致不可预知的问题。(?)
     * 但还没有找到重现的方法？？？
     * -Gim
     */
    _deletingItems.clear();
}

Blackboard *BbScene::blackboard(){
    return dynamic_cast<Blackboard *>(views()[0]);
}

void BbScene::setToolType(BbToolType toolType)
{
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
        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        IItemIndex *index = dynamic_cast<IItemIndex *>(item);
        if(index){
            if(first){
                index->last = current;
                current->next = index;
            }
            else{
                index->last = nullptr;
                first = index;
            }
            index->next = nullptr;
            if(job && job(index,i)){
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
    for(auto item: selectedItems()){
        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        IItemIndex *index = dynamic_cast<IItemIndex *>(item);
        if(!index)
            continue;

        if(first){
            index->last = current;
            current->next = index;
        }
        else{
            index->last = nullptr;
            first = index;
        }
        index->next = nullptr;
        if(job && job(index,i)){
            break;
        }
        current = index;
        i++;
    }
    return first;
}

void BbScene::removeSelected()
{
    auto enumJob = [&](IItemIndex *index,int ){
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

void BbScene::remove(QString lid)
{
    auto index = find<IItemIndex>(lid);
    if(index)
        remove(index);
}
void BbScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _mouseButtons |= event->button();
    _mousePos = blackboard()->tabletActive()?
                blackboard()->tabletPenPos():
                event->scenePos();
    _pickerBeginPos = event->scenePos();
    if(event->button() == Qt::LeftButton){
        if(_toolType == BBTT_Picker){
            QGraphicsScene::mousePressEvent(event);
            if(!event->isAccepted()) // 点中一个item，就不框选了。
                startPicking();
            if(_curItemIndex)
                _curItemIndex->toolDone(_mousePos);
        }else{
            stopPicking();
            deselectAll();
            if(_curItemIndex)
                _curItemIndex->toolDown(_mousePos);

            /*
             * Note:
             *      比如BbItemText，某些_curItemIndex的toolDown，
             *      会使得 _curItemIndex == nullptr。
             *      调用mousePressEvent：
             *          焦点text的文字才能被鼠标框选。
             *          如果点击的不是焦点text，焦点text将会失去焦点，
             *          然后toolDone，使_curItemIndex=nullptr，
             *          以实现点击其他地方立刻创建一个text。
             *      -Gim
             */
            if(_curItemIndex && _curItemIndex->toolType() == BBTT_Text)
                QGraphicsScene::mousePressEvent(event);

            if(!_curItemIndex){
                auto item = blackboard()->factory()->createItemWhenToolDown(_toolType);
                if(item){
                    add(item);
                    item->toolDown(_mousePos);
                }
            }
        }
    }
    else if(event->button() == Qt::RightButton){
        if(_curItemIndex){
            _curItemIndex->toolDone(_mousePos);
        }

        /*
         * NOTE:
         * 情况1：右键没点中任何item时，开始框选模式。
         * 情况2：右键没点中没选择的item时，此item.selected=true,其他item.selected=false
         *  -Gim
         */
        auto item = itemAt(_mousePos,QTransform());
        auto idx = dynamic_cast<IItemIndex*>(item);
        auto situation1 = idx == nullptr;
        auto situation2 = idx && !item->isSelected();
        if(situation1){
            startPicking();
        }
        else if(situation2){
            deselectAll();
            item->setSelected(true);
        }
    }
}

void BbScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    _mousePos = blackboard()->tabletActive()?
                blackboard()->tabletPenPos():
                event->scenePos();
    if(_mouseButtons & Qt::LeftButton){
        if(BBTT_Picker == _toolType){
            QGraphicsScene::mouseMoveEvent(event);
            /*
             * NOTE:
             *      拖动item会使得此event被Accepted，故在此发出移动信号。
             *      但如果有其他被Accepted的情况就不应该这么做。但现在还没发现。
             *      当没被Accepted时，进行进行框选item的工作。
             */
            if(event->isAccepted())
                emitItemMovingSignals();
            else
                updatePickerRect();
        }
        else if(_curItemIndex){
            _curItemIndex->toolDraw(_mousePos);

            /*
             * Note:
             *      就怕日后某个item的toolDraw会让_curItemIndex==nullptr；
             *      调用mouseMoveEvent，文字的内容才能被鼠标框选。
             *      -Gim
             */
            if(_curItemIndex && _curItemIndex->toolType() == BBTT_Text)
                QGraphicsScene::mouseMoveEvent(event);
        }
    }else if(_mouseButtons & Qt::RightButton){
        updatePickerRect();
    }
}

void BbScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _mouseButtons ^= event->button();
    _mousePos = blackboard()->tabletActive()?
                blackboard()->tabletPenPos():
                event->scenePos();
    QGraphicsScene::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton){
        if(BBTT_Picker == _toolType){
            if(isPicking())
                stopPicking();
            emitItemMovedSignals();
        }
        else if(_curItemIndex){
            _curItemIndex->toolDone(_mousePos);
        }
        /*
         * NOTE:
         * 在拖动item时移除item。会导致后面新出现的item拖动动时发生跳至其他位置的BUG，故在这里特殊处理。
         * 鼠标是按下状态时，先加入待移除的列。在鼠标抬起时再移除。
         *      -Gim
         */
        for(auto idx: _deletingItems){
            if(idx->toolType() == BBTT_Text){
                auto text = dynamic_cast<BbItemText*>(idx);
                text->deleteLater();
            }
            else{
                delete idx;
            }
        }
        _deletingItems.clear();
    }else if(event->button() == Qt::RightButton){
        if(isPicking())
            stopPicking();
    }
}

void BbScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Note: 在非选择器状态下，不要响应，否则快速画画的时候可能导致框选到新画的笔迹！-Gim
    if(toolType() == BBTT_Picker) // Picker双击文本实现文本再次编辑。
        QGraphicsScene::mouseDoubleClickEvent(event);

}

void BbScene::keyPressEvent(QKeyEvent *e)
{
    QGraphicsScene::keyPressEvent(e);
    if(_modifiers != e->modifiers()){
        if(_curItemIndex)
            _curItemIndex->modifiersChanged(e->modifiers());
        _modifiers = e->modifiers();
    }
}

void BbScene::keyReleaseEvent(QKeyEvent *e)
{
    QGraphicsScene::keyReleaseEvent(e);
    if(_modifiers != e->modifiers()){
        if(_curItemIndex)
            _curItemIndex->modifiersChanged(e->modifiers());

        _modifiers = e->modifiers();
    }
}

void BbScene::copyItems()
{
    QClipboard *clipboard = QApplication::clipboard();

    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);
    auto items = selectedItems();
    std::sort(items.begin(),items.end(),[](QGraphicsItem *a,QGraphicsItem*b){
        return a->zValue() < b->zValue();
    });
    for(auto item: items){
        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

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


    for(auto item: selectedItems()){

        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

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
    return _backgroundRect.size();
}

bool BbScene::hasBackground() const
{
    return !_backgrounds.empty();
}

void BbScene::setBackground(const QPixmap &pixmap)
{
    clearBackground();
    addBackground(pixmap);
    layBackgrounds();
}

QString BbScene::addBackground(const QPixmap &pixmap)
{
    auto backgroundId = blackboard()->factory()->makeBackgroundId();
    addBackground(backgroundId,pixmap);
    return backgroundId;
}

QString BbScene::addBackground(QGraphicsItem *graphicsItem)
{
    auto backgroundId = blackboard()->factory()->makeBackgroundId();
    addBackground(backgroundId,graphicsItem);
    return backgroundId;
}

void BbScene::addBackground(QString backgroundId, const QPixmap &pixmap)
{
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem();
    pixmapItem->setZValue(INT_MIN);
    qreal ratio = width() / pixmap.width();
    pixmapItem->setPixmap(pixmap);
    pixmapItem->setScale(ratio);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    addBackground(backgroundId, pixmapItem);
}

void BbScene::addBackground(QString backgroundId, QGraphicsItem *graphicsItem)
{
    Q_ASSERT(nullptr != graphicsItem);
    Q_ASSERT(this != graphicsItem->scene());
    if(graphicsItem && graphicsItem->scene() != this)
    {
        addItem(graphicsItem);
        QPair<QString,QGraphicsItem*> pair(backgroundId,graphicsItem);
        _backgrounds.push_back(pair);
    }
}

void BbScene::clearBackground()
{
    if(!_backgrounds.isEmpty())
    {
        for(auto pair: _backgrounds)
        {
            QGraphicsScene::removeItem(pair.second);
            delete pair.second;
        }
        _backgrounds.clear();
    }
    _backgroundRect.setRect(0,0,0,0);
}

int BbScene::backgroundCount()
{
    return _backgrounds.count();
}

void BbScene::removeBackground(int index)
{
    if(index < _backgrounds.count())
    {
        delete _backgrounds.takeAt(index).second;
    }
}

void BbScene::removeBackground(QString id)
{
    for(auto pair : _backgrounds)
    {
        if(id == pair.first)
        {
            _backgrounds.removeOne(pair);
            delete pair.second;
            break;
        }
    }
}

QGraphicsItem *BbScene::background(int index)
{
    if(index < _backgrounds.count())
    {
        return _backgrounds.at(index).second;
    }
    return nullptr;
}

QGraphicsItem *BbScene::background(QString id)
{
    for(auto pair : _backgrounds)
    {
        if(id == pair.first)
        {
            return pair.second;
        }
    }
    return nullptr;
}

void BbScene::layBackgrounds()
{
    auto width = 0.0;
    auto height = 0.0;
    for(auto pair: _backgrounds)
    {
        auto rect = pair.second->sceneBoundingRect();
        width = (std::max)(_backgroundRect.width(),rect.width());
        pair.second->setPos(0,height);
        height += rect.height();
    }
    _backgroundRect.setWidth(width);
    _backgroundRect.setHeight(height);
}

QRectF BbScene::backgroundRect()
{
    for(auto itr = _backgrounds.begin(); itr != _backgrounds.end(); ++itr)
    {
        if(itr == _backgrounds.begin())
        {
            _backgroundRect = itr->second->sceneBoundingRect();
        }
        else
        {
            auto itemRect = itr->second->sceneBoundingRect();
            auto left = ((std::min)(_backgroundRect.left(),itemRect.left()));
            auto right = ((std::max)(_backgroundRect.right(),itemRect.right()));
            auto top = ((std::min)(_backgroundRect.top(),itemRect.top()));
            auto bottom = ((std::max)(_backgroundRect.bottom(),itemRect.bottom()));
            _backgroundRect.setRect(left,top,right-left,bottom-top);
        }
    }
    return _backgroundRect;
}

QList<QPair<QString, QGraphicsItem *> > BbScene::backgrounds()
{
    return _backgrounds;
}

void BbScene::updatePickerRect()
{
    _pickerRect->setRect(
                (std::min)(_mousePos.x(),_pickerBeginPos.x()),
                (std::min)(_mousePos.y(),_pickerBeginPos.y()),
                (std::abs)(_mousePos.x()-_pickerBeginPos.x()),
                (std::abs)(_mousePos.y()-_pickerBeginPos.y())
                );
}

QJsonObject BbScene::toJsonObject()
{
    QJsonObject ret;
    auto rect = sceneRect();
    ret["x"] = rect.x();
    ret["y"] = rect.y();
    ret["width"] = rect.width();
    ret["height"] = rect.height();

    QJsonArray jItems;
    for(auto item: items()){

        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        auto idx = dynamic_cast<IItemIndex *>(item);
        auto wr = dynamic_cast<IJsonWR *>(item);
        if(!idx || !wr)
            continue;

        jItems << wr->toJsonObject();
    }
    ret["items"] = jItems;
    return ret;
}

void BbScene::fromJsonObject(const QJsonObject &jobj)
{
    auto rect = QRectF(
            jobj["x"].toDouble(),
            jobj["y"].toDouble(),
            jobj["width"].toDouble(),
            jobj["height"].toDouble());
    setSceneRect(rect);
    auto jItems = jobj["items"].toArray();

    for(auto jVal: jItems){
        readItemFromJsonObject(jVal.toObject());
    }
}

void BbScene::checkItemsPicking()
{
    QPainterPath path;
    path.addRect(_pickerRect->sceneBoundingRect());
    setSelectionArea(path);
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

Qt::KeyboardModifiers BbScene::modifiers()
{
#ifdef WIN32
    /*
    Note: 未获得焦点时，收不到keyPressEvent与keyReleaseEvent。
    导致首次绘图，图形响应不了功能键。
    所以这里直接使用win32的api来获取功能键状态。
    - Gim
    */
    _modifiers = Qt::NoModifier;
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

bool BbScene::isPicking()
{
    return _pickerTimer->isActive();
}

void BbScene::startPicking()
{
    updatePickerRect();
    checkItemsPicking();
    _pickerTimer->start();
    _pickerRect->show();
}

void BbScene::onPicking()
{
    checkItemsPicking();
}

void BbScene::stopPicking()
{
    updatePickerRect();
    checkItemsPicking();
    _pickerTimer->stop();
    _pickerRect->hide();
}

void BbScene::onToolChanged(BbToolType previous)
{
//    if(BBTT_Picker == previous)
//        setItemPicking(false);
    if(_curItemIndex)
        _curItemIndex->toolDone(_mousePos);
//    if(BBTT_Picker == _toolType)
    //        setItemPicking(true);
}

void BbScene::drawPageSplitterWhenNeeded(QPainter *painter, const QRectF &rect)
{
    painter->setPen(_pageSplitterPen);
    auto pageHeight = _pageAspectRatio * width();
    auto i = int(rect.bottom() / pageHeight);
    auto y = i * pageHeight;
    while(y > rect.top()){
        painter->drawLine(0,y,width(),y);
        painter->drawText(2,y-2,QString("- %1 -").arg(i));
        --i;
        y = i * pageHeight;
    }
}

void BbScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if(_pageSplitterPosition == PSP_BACKGROUND)
        drawPageSplitterWhenNeeded(painter,rect);
}

void BbScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    if(_pageSplitterPosition == PSP_FOREGROUND)
        drawPageSplitterWhenNeeded(painter,rect);
}

IItemIndex *BbScene::readItemFromStream(QDataStream &stream)
{
    int type;
    stream >> type;
    auto index = blackboard()->factory()->createItem(static_cast<BbToolType>(type));
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

IItemIndex *BbScene::readItemFromJsonObject(const QJsonObject &jobj)
{
    auto type = jobj["type"].toInt();
    auto index = blackboard()->factory()->createItem(static_cast<BbToolType>(type));
    if(index){
        add(index);
        auto r = dynamic_cast<IJsonWR*>(index);
        if(r)
            r->fromJsonObject(jobj);
    }
    return index;
}

void BbScene::selectedAll()
{
//    if(_toolType == BBTT_Picker){
    for(auto item: items()){

        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        IItemIndex* itemIndex = dynamic_cast<IItemIndex*>(item);
        if(!itemIndex)
            continue;

        item->setSelected(true);
    }
//    }
}

void BbScene::deselectAll()
{
    for(auto item: selectedItems())
    {
        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        IItemIndex* itemIndex = dynamic_cast<IItemIndex*>(item);
        if(!itemIndex)
            continue;

        item->setSelected(false);
    }
}

void BbScene::remove(IItemIndex *index)
{
    if(index){
        index->removed();
        if(_curItemIndex == index)
            _curItemIndex = nullptr;

        auto item = dynamic_cast<QGraphicsItem*>(index);
        if(item)
        {
            item->clearFocus();
            item->setSelected(false);
//            item->setFlag(QGraphicsItem::ItemIsMovable,false);
//            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
//            item->setFlag(QGraphicsItem::ItemIsFocusable,false);
            item->setOpacity(0);
            item->setScale(0);
            item->setData(GRAPHICS_ITEM_DATA_KEY_DELETING,true);
        }
        else
        {
            qWarning() << "[BlackboardScene::remove] item is not a 'QGraphicsItem'! what happen?!";
        }

        /*
         * NOTE:
         * 在拖动item时移除item。会导致后面新出现的item拖动动时发生跳至其他位置的BUG，故在这里特殊处理。
         * 鼠标是按下状态时，先加入待移除的列。在鼠标抬起时再移除。
         *      -Gim
         */
        if(_mouseButtons != Qt::NoButton){
            _deletingItems << index;
        }else{
            if(index->toolType() == BBTT_Text){
                auto text = dynamic_cast<BbItemText*>(index);
                text->deleteLater();
            }
            else{
                delete index;
            }
        }
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
            item->setFlag(QGraphicsItem::ItemIsMovable,true);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsFocusable,true);
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

IItemIndex *BbScene::readItemData(BbItemData *itemData)
{
    auto item = blackboard()->factory()->createItem(itemData);
    if(item)
    {
        add(item);
        item->repaint();
    }
    return item;
}

void BbScene::writeStream(QDataStream &stream)
{
    for(auto item: items()){

        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        IItemIndex *idx = dynamic_cast<IItemIndex *>(item);
        IStreamWR *wr = dynamic_cast<IStreamWR *>(item);
        if(idx && wr){
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
    return _mouseButtons & Qt::LeftButton;
}

BbItemImage *BbScene::addImageItem(const qreal &width, const qreal &height)
{
    auto item = new BbItemImage();
    add(item);
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(blackboard()->factory()->makeItemZ());
    item->setId(blackboard()->factory()->makeItemId());
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItem(const QPixmap &pixmap)
{
    auto item = new BbItemImage();
    add(item);
    item->resize(pixmap.width(),pixmap.height());
    item->updatePrevPosition();
    item->setZ(blackboard()->factory()->makeItemZ());
    item->setId(blackboard()->factory()->makeItemId());
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
    item->setZ(blackboard()->factory()->makeItemZ());
    item->setId(blackboard()->factory()->makeItemId());
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
    item->setZ(blackboard()->factory()->makeItemZ());
    item->setId(blackboard()->factory()->makeItemId());
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
    item->setZ(blackboard()->factory()->makeItemZ());
    item->setId(blackboard()->factory()->makeItemId());
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
    item->setZ(blackboard()->factory()->makeItemZ());
    item->setId(blackboard()->factory()->makeItemId());
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

IItemIndex *BbScene::copyItemFromStream(QDataStream &stream)
{
    static int time = 0;
    auto index = readItemFromStream(stream);
    if(index)
    {
        index->setId(blackboard()->factory()->makeItemId());
        index->setZ(blackboard()->factory()->makeItemZ());
        index->updatePrevZ();
        index->moveByVector2(10,10);
        index->updatePrevPosition();
    }
    ++time;
    return index;
}

void BbScene::emitItemMovingSignals()
{
    auto enumJob = [&](IItemIndex *index,int ){
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
    auto enumJob = [&](IItemIndex *index,int n){
        if(n == 0){
            /*
            NOTE:
                是一堆item被拖动的，目前还不存在不能被拖动的item。
                若第一item位置沒有变化，就可以判定为未拖动。此时return true打断。
                    - Gim
            */
            auto item = dynamic_cast<QGraphicsItem *>(index);
            auto d = std::abs(index->data()->prevX - item->x()) +
                     std::abs(index->data()->prevY - item->y());
            moved = d > 0;
            if(!moved)
                return true;
        }
        return false;
    };
    auto index = enumSelected(enumJob);
    if(moved){
        emit blackboard()->multipleItemChanged(BBIET_itemMoved,index);
        while(index){
            auto next = index->next;
            auto data = index->data();
            data->updatePostion(index);
            emit blackboard()->itemChanged(BBIET_itemMoved,index);
            data->updatePrevPostion();
            index = next;
        }
    }
}
