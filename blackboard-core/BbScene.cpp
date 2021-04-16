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
#include "BbItemDeleter.h"
#include "BbItemInnerDataKey.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <QDateTime>
#include <QTimer>
#ifdef WIN32
#include <Windows.h>
#pragma comment(lib, "user32")
#endif

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

    _pageSplitterPen = QPen(QColor(255,255,255,100),1,Qt::DashLine);
    _pageSplitterPosition = PSP_FOREGROUND;
    _pageAspectRatio = 1.414285714285714;
    _pageSplitterTextFormat = QStringLiteral("- 牛师帮在线一对一 第%1页 -");
    _pageSpliiterTextFont = QFont("Microsoft YaHei");
}

BbScene::~BbScene()
{
    _pickerRect = nullptr;
    _curItemIndex = nullptr;
    _backgrounds.clear();
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

QList<IItemIndex*> BbScene::enumAllItems(std::function<bool (IItemIndex *)> job)
{
    QList<IItemIndex*> list;
    for(auto item: items())
    {
        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        auto index = dynamic_cast<IItemIndex *>(item);
        if(!index)
            continue;

        list << index;
    }
    if(job)
    {
        for(auto index: list)
        {
            if(job(index))
                break;
        }
    }
    return list;
}

QList<IItemIndex*> BbScene::enumSelectedItems(std::function<bool(IItemIndex *)> job)
{
    QList<IItemIndex*> list;
    for(auto item: selectedItems())
    {
        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool()) continue;

        auto index = dynamic_cast<IItemIndex *>(item);
        if(!index) continue;

        list << index;
    }
    if(job)
    {
        for(auto index: list)
        {
            if(job(index))
                break;
        }
    }

    return list;
}

void BbScene::removeSelected()
{
    auto enumJob = [&](IItemIndex *index){
        emit blackboard()->itemChanged(BBIET_itemDelete,index);
        return false;
    };
    auto indexes = enumSelectedItems(enumJob);
    emit blackboard()->multipleItemChanged(BBIET_itemDelete,indexes);
    for(auto index: indexes)
    {
        remove(index);
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
                _curItemIndex->toolUp(_mousePos);
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
             *          然后toolUp，使_curItemIndex=nullptr，
             *          以实现点击其他地方立刻创建一个text。
             *      -Gim
             */
            if(_curItemIndex && _curItemIndex->toolType() == BBTT_Text)
                QGraphicsScene::mousePressEvent(event);

            if(!_curItemIndex){
                auto item = factory()->createItemWhenToolDown(_toolType);
                if(item){
                    add(item);
                    item->toolDown(_mousePos);
                }
            }
        }
    }
    else if(event->button() == Qt::RightButton){
        if(_curItemIndex){
            _curItemIndex->toolUp(_mousePos);
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
            _curItemIndex->toolUp(_mousePos);
        }
        /*
         * NOTE:
         * 在拖动item时移除item。会导致后面新出现的item拖动动时发生跳至其他位置的BUG，故在这里特殊处理。
         * 鼠标是按下状态时，先加入待移除的列。在鼠标抬起时再移除。
         *      -Gim
         */
        for(auto idx: _deletingItems)
        {
            auto item = idx->item();
            if(!item) // ??
            {
                continue;
            }
            removeItem(item);
            BbItemDeleter::get()->addItem(idx);
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
    auto modifiers = e->modifiers();
    if(_modifiers != modifiers){
        if(_curItemIndex)
            _curItemIndex->modifiersChanged(modifiers);
        _modifiers = modifiers;
    }
}

void BbScene::keyReleaseEvent(QKeyEvent *e)
{
    QGraphicsScene::keyReleaseEvent(e);
    auto modifiers = e->modifiers();
    if(_modifiers != modifiers){
        if(_curItemIndex)
            _curItemIndex->modifiersChanged(modifiers);
        _modifiers = modifiers;
    }
}

QList<IItemIndex *> BbScene::selectedItems2Clipboard()
{
    QList<IItemIndex *> ret;
    auto items = selectedItems();

    if(items.isEmpty())
        return ret;

    auto sortOp = [](QGraphicsItem *a, QGraphicsItem *b){
        return a->zValue() < b->zValue();
    };
    std::sort(items.begin(),items.end(),sortOp);

    QJsonArray jArr;
    auto bounding_left = qreal(INT_MAX);
    auto bounding_right = qreal(INT_MIN);
    auto bounding_top = qreal(INT_MAX);
    auto bounding_bottom = qreal(INT_MIN);

    for(auto item: items)
    {
        // 忽略正在被移除的item。
        if(!item->isVisible() || item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        auto curr = dynamic_cast<IItemIndex *>(item);
        auto writer = dynamic_cast<IJsonW *>(item);
        if(!writer)
            continue;
        auto boundingRect = item->sceneBoundingRect();
        bounding_left = (std::min)(boundingRect.left(),bounding_left);
        bounding_right = (std::max)(boundingRect.right(),bounding_right);
        bounding_top = (std::min)(boundingRect.top(),bounding_top);
        bounding_bottom = (std::max)(boundingRect.bottom(),bounding_bottom);
        jArr << writer->toJsonObject();
        ret << curr;
    }

    if(jArr.isEmpty())
        return ret;

    QJsonObject jObj;
    jObj["items"] = jArr;
    jObj["bounding_left"] = bounding_left;
    jObj["bounding_right"] = bounding_right;
    jObj["bounding_top"] = bounding_top;
    jObj["bounding_bottom"] = bounding_bottom;

    jObj["address"] = int(this);
    auto data = QJsonDocument(jObj).toBinaryData();
    auto clipboard = QApplication::clipboard();
    auto mimeData = new QMimeData();
    mimeData->setData("nsb/bb-items",data);
    clipboard->setMimeData(mimeData);

    return ret;
}

void BbScene::cutItems()
{
    auto indexes = selectedItems2Clipboard();
    if(indexes.empty())
        return;

    emit blackboard()->multipleItemChanged(BBIET_itemCut,indexes);

    for(auto index : indexes){
        remove(index);
    }
}

void BbScene::copyItems()
{
    auto indexes = selectedItems2Clipboard();
    if(indexes.empty())
        return;
    emit blackboard()->multipleItemChanged(BBIET_itemCopy,indexes);
}

void BbScene::pasteItems()
{
    auto clipboard = QApplication::clipboard();
    auto mimeData = clipboard->mimeData();

    auto a = mimeData->formats();

    if(!mimeData)
        return;

    if(mimeData->hasFormat("text/plain"))
    {
        auto data = mimeData->data("text/plain");
        if(data.size() == 0)
            return;
        auto item = factory()->createItemWhenToolDown(BBTT_Text);
        if(!item){ // ???
            qWarning() << __FUNCTION__ << "failed to paste text! cant create Item by tool type BBTT_Text!";
            return;
        }
        auto text = dynamic_cast<BbItemText*>(item);
        if(!text){ // ???
            qWarning() << __FUNCTION__ << "failed to paste text! cant cast Item to Text.";
            return;
        }
        add(text);
        auto pos = blackboard()->mapToScene(10,10);
        text->toolDown(pos);
        text->setPlainText(QString::fromUtf8(data));
        text->updateContent();
        text->setupTextBlockFormat();
        text->toolUp(pos);
        return;
    }

    if(!mimeData->hasFormat("nsb/bb-items"))
        return;

    auto data = mimeData->data("nsb/bb-items");

    Q_ASSERT(data.size() != 0);

    if(data.size() == 0)
        return;

    auto topZ = 0.0;
    for(auto item : this->items(Qt::DescendingOrder)){
        auto index = dynamic_cast<IItemIndex*>(item);
        if(!index)
            continue;
        topZ = item->zValue();
        break;
    }
    deselectAll();
    auto jDoc = QJsonDocument::fromBinaryData(data);

    Q_ASSERT(jDoc.isObject());

    if(!jDoc.isObject())
        return;

    auto jObj = jDoc.object();
    auto jItems = jObj["items"].toArray();
    auto formSelf = jObj["address"].toInt() == int(this);
    auto boundingLeft = jObj["bounding_left"].toDouble();
    auto boundingTop = jObj["bounding_top"].toDouble();
    QRectF boundingRect(
                boundingLeft,
                boundingTop,
                jObj["bounding_right"].toDouble() - boundingLeft,
                jObj["bounding_bottom"].toDouble() - boundingTop);

    auto visionLeftTop = blackboard()->mapToScene(0,0);
    auto visionRightBottom = blackboard()->mapToScene(
                blackboard()->width(),
                blackboard()->height());
    QRectF visionRect(
                visionLeftTop.x(),
                visionLeftTop.y(),
                visionRightBottom.x()-visionLeftTop.x(),
                visionRightBottom.y()-visionLeftTop.y());

    auto outOfVision = !formSelf || !boundingRect.intersects(visionRect);
    auto offsetX = visionRect.left() + 0.5f * (visionRect.width() - boundingRect.width()) - boundingRect.left();
    auto offsetY = visionRect.top() + 0.5f * (visionRect.height() - boundingRect.height()) - boundingRect.top();

    QList<IItemIndex*> indexes;
    for(auto jVal : jItems)
    {
        auto curr = factory()->createItem(jVal.toObject());
        if(!curr)
        {
            qWarning() << __FUNCTION__ << "failed to createItem by JSON object!" << jVal;
            continue;
        }
        auto item = curr->item();
        if(!item)
        {
            qWarning() << __FUNCTION__ << "failed to cast ItemIndex to QGraphicsItem!";
            continue;
        }
        curr->setId(factory()->makeItemId(curr->toolType()));
        curr->setZ(factory()->makeItemZ(curr->toolType()));
        add(curr);
        item->setSelected(true);
        if(outOfVision){
            auto x = curr->positionX() + offsetX;
            auto y = curr->positionY() + offsetY;
            curr->moveToPosition(x,y);
            curr->updatePrevPosition();
        }else{
            curr->moveByVector2(20,20);
            curr->updatePrevPosition();
        }
        indexes << curr;
    }
    emit blackboard()->multipleItemChanged(BBIET_itemPaste,indexes);
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
    auto backgroundId = factory()->makeBackgroundId();
    addBackground(backgroundId,pixmap);
    return backgroundId;
}

QString BbScene::addBackground(QGraphicsItem *graphicsItem)
{
    auto backgroundId = factory()->makeBackgroundId();
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
            removeItem(pair.second);
            BbItemDeleter::get()->addItem(pair.second);
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
        auto pair = _backgrounds.takeAt(index);
        removeItem(pair.second);
        BbItemDeleter::get()->addItem(pair.second);
    }
}

void BbScene::removeBackground(QString id)
{
    for(auto pair : _backgrounds)
    {
        if(id == pair.first)
        {
            _backgrounds.removeOne(pair);
            removeItem(pair.second);
            BbItemDeleter::get()->addItem(pair.second);
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
        auto item = factory()->createItem(jVal.toObject());
        add(item);
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

QHash<int, QRectF> BbScene::getNotEmptyPageAreas()
{
    QHash<int, QRectF> ret;
    qreal maxH = height();
    qreal w = width();
    qreal h = w * pageAspectRatio();
    auto items = this->items();
    if(items.empty())
        return ret;
    while(!items.empty()){
        auto item = items.takeFirst();
        if(!item->isVisible())
            continue;
        item->setSelected(false);
        auto itemRect = item->sceneBoundingRect();
        if(itemRect.right() < 0 ||
                itemRect.bottom() < 0 ||
                itemRect.left() > w ||
                itemRect.bottom() > maxH)
            continue;

        auto beginPage = int(itemRect.top()/h);
        auto endPage = int(itemRect.bottom()/h);
        for(auto page = beginPage; page <= endPage; ++page)
            ret[page] = QRectF(0,h * page, w, h);
    }
    return ret;
}

void BbScene::groupUp(QList<IItemIndex*> indexes, const QString &id){
    if(indexes.isEmpty())
        return;
    QList<QGraphicsItem*> items;
    for(auto index : indexes){
        auto item = index->item();
        if(!item)
            continue;
        item->setFlag(QGraphicsItem::ItemIsMovable,false);
        item->setFlag(QGraphicsItem::ItemIsSelectable,false);
        item->setFlag(QGraphicsItem::ItemIsFocusable,false);
        items << item;
    }
    auto group = QGraphicsScene::createItemGroup(items);
    group->setFlag(QGraphicsItem::ItemIsMovable,true);
    group->setFlag(QGraphicsItem::ItemIsSelectable,true);
    group->setFlag(QGraphicsItem::ItemIsFocusable,false);
    group->setData(BBIIDK_ITEM_IS_GROUP,true);
    group->setData(BBIIDK_ITEM_ID,id.isEmpty()?factory()->makeGroupId():id);
    group->setSelected(true);
    _groups << group;
    if(id.isEmpty())
        emit blackboard()->groupUped( { group } );
}

void BbScene::groupUp(QList<QString> itemIds, const QString &id)
{
    QList<IItemIndex *> items;
    enumAllItems([&items,&id](IItemIndex *item){
        if(item->id() == id)
            items << item;
        return false;
    });
    groupUp(items);
}

void BbScene::groupUp(const QString &id)
{
    auto indexes = QList<IItemIndex*>();
    auto items = QList<QGraphicsItem*>();
    auto oldGroups = QList<QGraphicsItemGroup*>();
    for(auto item : selectedItems()){
        if(true == item->data(BBIIDK_ITEM_IS_GROUP)){
            auto group = static_cast<QGraphicsItemGroup*>(item);
            for(auto item : group->childItems()){
                items << item;
                auto index = dynamic_cast<IItemIndex*>(item);
                if(index)
                    indexes << index;
            }
            oldGroups << group;
        }
        else if(true == item->data(BBIIDK_ITEM_IS_SHAPE).toBool()){
            items << item;
            auto index = dynamic_cast<IItemIndex*>(item);
            if(index)
                indexes << index;
        }
    }
    emit blackboard()->dismissed(oldGroups);
    for(auto group : oldGroups){
        _groups.removeAll(group);
        destroyItemGroup(group);
    }
    groupUp(indexes,id);
}

void BbScene::dismiss(QList<QString> ids)
{
    auto groups = QList<QGraphicsItemGroup*>();

    if(ids.empty()){
        for(auto item : selectedItems()){
            if(false == item->data(BBIIDK_ITEM_IS_GROUP).toBool())
                continue;
            groups << static_cast<QGraphicsItemGroup*>(item);
        }
    }else{
        for(auto item : items()){
            if(false == item->data(BBIIDK_ITEM_IS_GROUP).toBool() || !ids.contains(item->data(BBIIDK_ITEM_ID).toString()))
                continue;
            groups << static_cast<QGraphicsItemGroup*>(item);
        }
    }
    emit blackboard()->dismissed(groups);
    for(auto group: groups){
        auto items = group->childItems();
        _groups.removeAll(group);
        destroyItemGroup(group);
        for(auto item: items){
            if(true == item->data(BBIIDK_ITEM_IS_SHAPE).toBool()){
                item->setFlag(QGraphicsItem::ItemIsFocusable);
                item->setFlag(QGraphicsItem::ItemIsMovable);
                item->setFlag(QGraphicsItem::ItemIsSelectable);
                item->setSelected(true);
            }
            else{
                // group->removeFromGroup(item);
                // NOTE: delete? -Gim
            }
        }
    }
}

void BbScene::onToolChanged(BbToolType previous)
{
//    if(BBTT_Picker == previous)
//        setItemPicking(false);
    if(_curItemIndex)
        _curItemIndex->toolUp(_mousePos);
//    if(BBTT_Picker == _toolType)
    //        setItemPicking(true);
}

void BbScene::drawPageSplitterWhenNeeded(QPainter *painter, const QRectF &rect)
{
    painter->setPen(_pageSplitterPen);
    painter->setFont(_pageSpliiterTextFont);
    auto pageHeight = _pageAspectRatio * width();
    auto i = int(rect.bottom() / pageHeight);
    auto y = i * pageHeight;
    while(y > rect.top()){
        painter->drawLine(0,y,width(),y);
        painter->drawText(2,y-2,_pageSplitterTextFormat.arg(i));
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
    auto index = factory()->createItem(static_cast<BbToolType>(type));
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

BbFactory *BbScene::factory()
{
    return blackboard()->factory();
}

void BbScene::selectedAll()
{
    for(auto item: items()){

        // 忽略正在被移除的item。
        if(item->data(GRAPHICS_ITEM_DATA_KEY_DELETING).toBool())
            continue;

        IItemIndex* itemIndex = dynamic_cast<IItemIndex*>(item);
        if(!itemIndex)
            continue;

        item->setSelected(true);
    }
}

void BbScene::deselectAll()
{
    QPainterPath path;
    setSelectionArea(path);
}

void BbScene::remove(IItemIndex *index)
{
    if(!index){
        qWarning() << __FUNCTION__ << "failed! index == nullptr!";
        return;
    }

    index->removed();

    if(_curItemIndex == index)
        _curItemIndex = nullptr;

    auto item = index->item();
    if(item)
    {
        item->clearFocus();
        item->setSelected(false);
        item->setOpacity(0);
        item->setScale(0);
        item->setData(GRAPHICS_ITEM_DATA_KEY_DELETING,true);
    }
    else
    {
        qWarning() << __FUNCTION__ << "item is not a 'QGraphicsItem'! what happen?!";
    }
    /*
     * NOTE:
     * 在拖动item时移除item。会导致后面新出现的item拖动动时发生跳至其他位置的BUG，故在这里特殊处理。
     * 鼠标是按下状态时，先加入待移除的列。在鼠标抬起时再移除。
     *      -Gim
     */
    if(_mouseButtons != Qt::NoButton){
        _deletingItems << index;
    }
    else
    {
        BbItemDeleter::get()->addItem(index);
        if(item)
            removeItem(item);
    }
}

void BbScene::add(IItemIndex *index)
{
    if(!index){
        qWarning() << "[BlackboardScene::add] fail to add item, pointer is nullptr!";
        return;
    }
    auto item = index->item();
    if(item)
    {
        item->setFlag(QGraphicsItem::ItemIsMovable,true);
        item->setFlag(QGraphicsItem::ItemIsSelectable,true);
        item->setFlag(QGraphicsItem::ItemIsFocusable,true);
        QGraphicsScene::addItem(item);
    }
    else
    {
        qWarning() << "[BlackboardScene::add] item is not a 'QGraphicsItem'! what happen?!";
    }
    index->absolutize();
    index->added();
}

void BbScene::clearItems()
{
    auto indexes = enumAllItems();
    for(auto index : indexes)
    {
        remove(index);
    }
}

IItemIndex *BbScene::readItemData(BbItemData *itemData)
{
    auto item = factory()->createItem(itemData);
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
static void moveItemToLeftTop(IItemIndex *item,BbScene *scene){
    auto blackboard = scene->blackboard();
    auto pos = blackboard->mapToScene(0,0);
    item->moveToPosition(pos);
}

BbItemImage *BbScene::addImageItem(const qreal &width, const qreal &height)
{
    auto item = new BbItemImage();
    add(item);
    moveItemToLeftTop(item,this);
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(factory()->makeItemZ(item->toolType()));
    item->setId(factory()->makeItemId(item->toolType()));
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItem(const QPixmap &pixmap)
{
    auto item = new BbItemImage();
    add(item);
    moveItemToLeftTop(item,this);
    item->resize(pixmap.width(),pixmap.height());
    item->updatePrevPosition();
    item->setZ(factory()->makeItemZ(item->toolType()));
    item->setId(factory()->makeItemId(item->toolType()));
    item->setPixmap(pixmap);
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

BbItemImage *BbScene::addImageItem(const qreal &width, const qreal &height, const QPixmap &pixmap)
{
    auto item = new BbItemImage();
    add(item);
    moveItemToLeftTop(item,this);
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(factory()->makeItemZ(item->toolType()));
    item->setId(factory()->makeItemId(item->toolType()));
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

    moveItemToLeftTop(item,this);

    item->resize(pixmap.width(),pixmap.height());
    item->updatePrevPosition();
    item->setZ(factory()->makeItemZ(item->toolType()));
    item->setId(factory()->makeItemId(item->toolType()));
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
    moveItemToLeftTop(item,this);
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(factory()->makeItemZ(item->toolType()));
    item->setId(factory()->makeItemId(item->toolType()));
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
    moveItemToLeftTop(item,this);
    item->resize(width,height);
    item->updatePrevPosition();
    item->setZ(factory()->makeItemZ(item->toolType()));
    item->setId(factory()->makeItemId(item->toolType()));
    emit blackboard()->itemChanged(BBIET_imageAdded,item);
    return item;
}

IItemIndex *BbScene::copyItemFromStream(QDataStream &stream)
{
    static int time = 0;
    auto index = readItemFromStream(stream);
    if(index)
    {
        index->setId(factory()->makeItemId(index->toolType()));
        index->setZ(factory()->makeItemZ(index->toolType()));
        index->updatePrevZ();
        index->moveByVector2(10,10);
        index->updatePrevPosition();
    }
    ++time;
    return index;
}

void BbScene::emitItemMovingSignals()
{
    auto enumJob = [&](IItemIndex *index){
        index->data()->updatePostion(index);
        emit blackboard()->itemChanged(BBIET_itemMoving,index);
        return false;
    };
    auto indexes = enumSelectedItems(enumJob);
    emit blackboard()->multipleItemChanged(BBIET_itemMoving,indexes);
}

void BbScene::emitItemMovedSignals()
{
    bool moved = false;
    auto enumJob = [&](IItemIndex *index){
        /*
        NOTE:
            是一堆item被拖动的，目前还不存在不能被拖动的item。
            若第一item位置沒有变化，就可以判定为未拖动。此时return true打断。
                - Gim
        */
        auto item = index->item();
        auto d = std::abs(index->data()->prevX - item->x()) +
                 std::abs(index->data()->prevY - item->y());
        moved = d > 0;
        return true;
    };
    auto indexes = enumSelectedItems(enumJob);
    if(!moved)
        return;

    emit blackboard()->multipleItemChanged(BBIET_itemMoved,indexes);
    for(auto index :indexes){
        auto data = index->data();
        data->updatePostion(index);
        emit blackboard()->itemChanged(BBIET_itemMoved,index);
        data->updatePrevPostion();
    }
}
