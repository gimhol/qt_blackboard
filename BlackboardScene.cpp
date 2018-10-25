#include "BlackboardScene.h"
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
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QDateTime>
#include <QKeyEvent>

BlackboardScene::BlackboardScene(Blackboard *parent):
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
    add(_pickerRect);
}

BlackboardScene::~BlackboardScene()
{
    QGraphicsScene::removeItem(_pickerRect);
    delete _pickerRect;
}

Blackboard *BlackboardScene::blackboard(){
    return dynamic_cast<Blackboard *>(views()[0]);
}

void BlackboardScene::setToolType(BbToolType toolType)
{
    _mouseLeftButtonDown = false;
    if(_toolType != toolType)
    {
        onToolChanged(_toolType, toolType);
        blackboard()->onToolChanged(_toolType, toolType);
        _toolType = toolType;
    }
}

BbToolType BlackboardScene::toolType()
{
    return _toolType;
}

void BlackboardScene::removeSelectedItems()
{
    for(auto item: selectedItems())
    {
        IItemIndex *idx = dynamic_cast<IItemIndex *>(item);
        if(!idx)
        {
            continue;
        }

#define REMOVE_ITEM_INDEX(_TYPE_,_CLASS_,_SIGNAL_) \
    case _TYPE_: \
        emit blackboard()->_SIGNAL_(dynamic_cast<_CLASS_ *>(item)); \
        remove(item); \
        break

        switch(idx->toolType())
        {
            REMOVE_ITEM_INDEX(BBTT_Rectangle,BbItemRect,rectDelete);
            REMOVE_ITEM_INDEX(BBTT_Straight,BbItemStraight,straightDelete);
            REMOVE_ITEM_INDEX(BBTT_Text,BbItemText,textDelete);
            REMOVE_ITEM_INDEX(BBTT_Pen,BbItemPen,penDelete);
            REMOVE_ITEM_INDEX(BBTT_Ellipse,BbItemEllipse,ellipseDelete);
            REMOVE_ITEM_INDEX(BBTT_Triangle,BbItemTriangle,triangleDelete);
            REMOVE_ITEM_INDEX(BBTT_Image,BbItemImage,imageDelete);
            default:
            {
                break;
            }
        }
    }
}


void BlackboardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!_controlEnable)
    {
        return;
    }
    _mouseBeginPos = event->scenePos();
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        _mouseLeftButtonDown = true;

        switch(_toolType)
        {
            case BBTT_Triangle:
            {
                localTriangleBegin(_mouseBeginPos);
                return;
            }
            case BBTT_Ellipse:
            {
                localEllipseBegin(_mouseBeginPos);
                return;
            }
            case BBTT_Rectangle:
            {
                localRectBegin(_mouseBeginPos);
                return;
            }
            case BBTT_Pen:
            {
                localPenDown(_mouseBeginPos);
                return;
            }
            case BBTT_Text:
            {
                localTextAdded(_mouseBeginPos);
                return;
            }
            case BBTT_Straight:
            {
                localStraightBegin(_mouseBeginPos);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void BlackboardScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!_controlEnable)
    {
        return;
    }

    QPointF mousePos = event->scenePos();
    switch(_toolType)
    {
        case BBTT_Triangle:
        {
            if(_mouseLeftButtonDown)
            {
                localTriangleDrag(mousePos);
            }
            break;
        }
        case BBTT_Ellipse:
        {
            if(_mouseLeftButtonDown)
            {
                localEllipseDrag(mousePos);
            }
            break;
        }
        case BBTT_Rectangle:
        {
            if(_mouseLeftButtonDown)
            {
                localRectDrag(mousePos);
            }
            break;
        }
        case BBTT_Pen:
        {
            if(_mouseLeftButtonDown)
            {
                loaclPenDraw(mousePos);
            }
            break;
        }
        case BBTT_Straight:
        {
            if(_mouseLeftButtonDown)
            {
                localStraightDrag(mousePos);
            }
            break;
        }
        case BBTT_Picker:
        {
            if(_mouseLeftButtonDown)
            {
                QGraphicsScene::mouseMoveEvent(event);
                if(event->isAccepted())
                {
                    for(auto item: selectedItems())
                    {
                        IItemIndex *idx = dynamic_cast<IItemIndex *>(item);
                        if(!idx)
                        {
                            continue;
                        }
                        switch(idx->toolType())
                        {
                            case BBTT_Triangle:
                            {
                                emit blackboard()->triangleMoved(dynamic_cast<BbItemTriangle *>(item));
                                break;
                            }
                            case BBTT_Ellipse:
                            {
                                emit blackboard()->ellipseMoved(dynamic_cast<BbItemEllipse *>(item));
                                break;
                            }
                            case BBTT_Rectangle:
                            {
                                emit blackboard()->rectMoved(dynamic_cast<BbItemRect *>(item));
                                break;
                            }
                            case BBTT_Text:
                            {
                                emit blackboard()->textMoved(dynamic_cast<BbItemText *>(item));
                                break;
                            }
                            case BBTT_Pen:
                            {
                                emit blackboard()->penMoved(dynamic_cast<BbItemPen *>(item));
                                break;
                            }
                            case BBTT_Straight:
                            {
                                emit blackboard()->straightMoved(dynamic_cast<BbItemStraight *>(item));
                                break;
                            }
                            case BBTT_Image:
                            {
                                emit blackboard()->imageMoved(dynamic_cast<BbItemImage *>(item));
                                break;
                            }
                            default:
                            {
                                qDebug() << "Try to move unkowning element!" << endl;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    pickingItems(mousePos);
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

void BlackboardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(!_controlEnable){
        return;
    }
    QGraphicsScene::mouseReleaseEvent(event);
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        _mouseLeftButtonDown = false;
        switch(_toolType)
        {
            case BBTT_Triangle:
            {
                localTriangleDone(false);
                break;
            }
            case BBTT_Ellipse:
            {
                localEllipseDone();
                break;
            }
            case BBTT_Rectangle:
            {
                localRectDone();
                break;
            }
            case BBTT_Straight:
            {
                localStraightDone();
                break;
            }
            case BBTT_Pen:
            {
                localPenDone();
                break;
            }
            case BBTT_Picker:
            {
                _pickerRect->hide();
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void BlackboardScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
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

void BlackboardScene::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_Escape:
        {
            localTextDone();
            break;
        }
        case Qt::Key_Shift:
        {
            if(!(e->modifiers() & Qt::ControlModifier) && !(e->modifiers() & Qt::AltModifier))
            {
                _onlyShiftDown = true;
            }
            BbItemPen * pen = dynamic_cast<BbItemPen *>(_curElement);
            if(pen)
            {
                pen->setStraight(true);
                break;
            }
            BbItemStraight * straight = dynamic_cast<BbItemStraight *>(_curElement);
            if(straight)
            {
                straight->setFortyFive(true);
                emit blackboard()->straightDragged(straight);
                break;
            }
            BbItemRect * rect = dynamic_cast<BbItemRect *>(_curElement);
            if(rect)
            {
                rect->setSquare(true);
                emit blackboard()->rectDragged(rect);
                break;
            }
            BbItemEllipse * ellipse = dynamic_cast<BbItemEllipse *>(_curElement);
            if(ellipse)
            {
                ellipse->setCircle(true);
                emit blackboard()->ellipseDragged(ellipse);
                break;
            }
            break;
        }
        case Qt::Key_Alt:
        {
            if(!(e->modifiers() & Qt::ControlModifier) && !(e->modifiers() & Qt::ShiftModifier))
            {
                _onlyAltDown = true;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    QGraphicsScene::keyPressEvent(e);
}

void BlackboardScene::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_Shift:
        {
            if(_onlyShiftDown)
            {
                _onlyShiftDown = false;
            }
            BbItemPen * pen = dynamic_cast<BbItemPen *>(_curElement);
            if(pen)
            {
                pen->setStraight(false);
                emit blackboard()->penDraw(pen);
                break;
            }
            BbItemStraight * straight = dynamic_cast<BbItemStraight *>(_curElement);
            if(straight)
            {
                straight->setFortyFive(false);
                emit blackboard()->straightDragged(straight);
                break;
            }
            BbItemRect * rect = dynamic_cast<BbItemRect *>(_curElement);
            if(rect)
            {
                rect->setSquare(false);
                emit blackboard()->rectDragged(rect);
                break;
            }
            BbItemEllipse * ellipse = dynamic_cast<BbItemEllipse *>(_curElement);
            if(ellipse)
            {
                ellipse->setCircle(false);
                emit blackboard()->ellipseDragged(ellipse);
                break;
            }
            break;
        }
        case Qt::Key_Alt:
        {
            if(_onlyAltDown)
            {
                _onlyAltDown = false;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    QGraphicsScene::keyReleaseEvent(e);
}

void BlackboardScene::localRectBegin(const QPointF &pos)
{
    BbItemRect * rect = new BbItemRect();
    rect->setZValue(QDateTime::currentMSecsSinceEpoch());
    rect->setPenColor(blackboard()->rectPenColor());
    rect->setWeight(blackboard()->rectWeight());
    rect->setBrushColor(blackboard()->rectBrushColor());
    add(rect);

    rect->begin(pos);
    rect->setId(generatItemId());
    rect->setSquare(_onlyShiftDown);
    _curElement = rect;
    emit blackboard()->rectBegun(rect);
}

void BlackboardScene::localRectDrag(const QPointF &pos)
{
    BbItemRect *rect = dynamic_cast<BbItemRect *>(_curElement);
    if(rect)
    {
        rect->drag(pos);
        emit blackboard()->rectDragged(rect);
    }
}

void BlackboardScene::localRectDone()
{
    BbItemRect *rect = dynamic_cast<BbItemRect *>(_curElement);
    if(rect)
    {
        rect->done();
        emit blackboard()->rectDone(rect);
    }
    _curElement = nullptr;
}

void BlackboardScene::localEllipseBegin(const QPointF &pos)
{
    BbItemEllipse * ellipse = new BbItemEllipse();
    ellipse->setZValue(QDateTime::currentMSecsSinceEpoch());
    ellipse->setPenColor(blackboard()->ellipsePenColor());
    ellipse->setBrushColor(blackboard()->ellipseBrushColor());
    ellipse->setWeight(blackboard()->ellipseWeight());
    add(ellipse);

    ellipse->begin(pos);
    ellipse->setId(generatItemId());
    ellipse->setCircle(_onlyShiftDown);
    _curElement = ellipse;
    emit blackboard()->ellipseBegun(ellipse);
}

void BlackboardScene::localEllipseDrag(const QPointF &pos)
{
    BbItemEllipse *ellipse = dynamic_cast<BbItemEllipse *>(_curElement);
    if(ellipse)
    {
        ellipse->drag(pos);
        emit blackboard()->ellipseDragged(ellipse);
    }
}

void BlackboardScene::localEllipseDone()
{
    BbItemEllipse *ellipse = dynamic_cast<BbItemEllipse *>(_curElement);
    if(ellipse)
    {
        ellipse->done();
        emit blackboard()->ellipseDone(ellipse);
    }
    _curElement = nullptr;
}

void BlackboardScene::localTriangleBegin(const QPointF &pos)
{
    BbItemTriangle *triangle = dynamic_cast<BbItemTriangle *>(_curElement);
    if(triangle)
    {
        return;
    }

    triangle = new BbItemTriangle();
    triangle->setZValue(QDateTime::currentMSecsSinceEpoch());
    triangle->setPenColor(blackboard()->trianglePenColor());
    triangle->setBrushColor(blackboard()->triangleBrushColor());
    triangle->setWeight(blackboard()->triangleWeight());
    add(triangle);

    triangle->begin(pos);
    triangle->setId(generatItemId());
    _curElement = triangle;
    emit blackboard()->triangleBegun(triangle);
}

void BlackboardScene::localTriangleDrag(const QPointF &pos)
{
    BbItemTriangle *triangle = dynamic_cast<BbItemTriangle *>(_curElement);
    if(triangle)
    {
        triangle->drag(pos);
        emit blackboard()->triangleDragged(triangle);
    }
}

void BlackboardScene::localTriangleDone(bool force)
{
    BbItemTriangle *triangle = dynamic_cast<BbItemTriangle *>(_curElement);
    if(triangle)
    {
        triangle->done();

        emit blackboard()->triangleDone(triangle);

        if(triangle->step() > 1 || force)
        {
            _curElement = nullptr;
        }
    }
    else
    {
        _curElement = nullptr;
    }

}

void BlackboardScene::localStraightBegin(const QPointF &pos)
{
    BbItemStraight * straight = new BbItemStraight();
    straight->setZValue(QDateTime::currentMSecsSinceEpoch());
    straight->setColor(blackboard()->straightPenColor());
    straight->setWeight(blackboard()->straightPenWeight());
    add(straight);

    straight->begin(pos);
    straight->setId(generatItemId());
    straight->setFortyFive(_onlyShiftDown);
    _curElement = straight;
    emit blackboard()->straightBegun(straight);
}

void BlackboardScene::localStraightDrag(const QPointF &pos)
{
    BbItemStraight *straight = dynamic_cast<BbItemStraight *>(_curElement);
    if(straight)
    {
        straight->drag(pos);
        emit blackboard()->straightDragged(straight);
    }
}

void BlackboardScene::localStraightDone()
{
    BbItemStraight *straight = dynamic_cast<BbItemStraight *>(_curElement);
    if(straight)
    {
        straight->done();
        emit blackboard()->straightDone(straight);
    }
    _curElement = nullptr;
}

void BlackboardScene::localPenDown(const QPointF & mousePos)
{
    BbItemPen *pen = new BbItemPen();
    pen->setZValue(QDateTime::currentMSecsSinceEpoch());
    pen->setWeight(blackboard()->penWeight());
    pen->setColor(blackboard()->penColor());
    add(pen);

    pen->penDown(mousePos);
    pen->setId(generatItemId());
    pen->setStraight(_onlyShiftDown);
    _curElement = pen;

    emit blackboard()->penDown(pen);
}

void BlackboardScene::loaclPenDraw(const QPointF &mousePos)
{
    BbItemPen *pen = dynamic_cast<BbItemPen *>(_curElement);
    if(pen)
    {
        if(!pen->straight())
        {
            pen->penDraw(mousePos);
            emit blackboard()->penDraw(pen);
        }
        else
        {
            pen->penStraighting(mousePos);
            emit blackboard()->penStraighting(pen);
        }
    }
}

void BlackboardScene::localPenDone()
{
    BbItemPen *pen = dynamic_cast<BbItemPen *>(_curElement);
    if(pen)
    {
        if(pen->straight())
        {
            pen->setStraight(false);
            emit blackboard()->penDraw(pen);
        }
        pen->done();
        emit blackboard()->penDone(pen);
    }
    _curElement = nullptr;
}

void BlackboardScene::localTextAdded(const QPointF &pos)
{
    BbItemText *text = new BbItemText();
    text->setZValue(QDateTime::currentMSecsSinceEpoch());
    text->setFont(blackboard()->font());
    text->setWeight(blackboard()->textPointWeight());
    text->setColor(blackboard()->textColor());
    add(text);
    text->setTextInteractionFlags(Qt::TextEditorInteraction);
    text->setFocus();
    text->setPos(pos.x(), pos.y() - 0.5 * text->boundingRect().height());
    text->setId(generatItemId());
    _curElement = text;
    emit blackboard()->textAdded(text);
}

void BlackboardScene::localTextDone()
{
    BbItemText * text = dynamic_cast<BbItemText *>(_curElement);
    if(text)
    {
        text->clearFocus();
        emit blackboard()->textDone(text);
    }
    _curElement = nullptr;
}

void BlackboardScene::pickingItems(const QPointF &mousePos)
{
    _pickerRect->setRect(
                std::min(mousePos.x(),_mouseBeginPos.x()),
                std::min(mousePos.y(),_mouseBeginPos.y()),
                std::abs(mousePos.x()-_mouseBeginPos.x()),
                std::abs(mousePos.y()-_mouseBeginPos.y())
                );
    _pickerRect->show();
    for(auto item: items()){
        IItemIndex* itemIndex = dynamic_cast<IItemIndex*>(item);
        if(!itemIndex){
            continue;
        }
        if(item->collidesWithItem(_pickerRect)){
            if(!item->isSelected()){
                item->setSelected(true);
            }
        }
        else{
            if(item->isSelected()){
                item->setSelected(false);
                item->update();
            }
        }
    }
}

QString BlackboardScene::generatItemId() const
{
    if(_itemIdGenerator != nullptr)
    {
        return _itemIdGenerator();
    }
    return QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
}

void BlackboardScene::onToolChanged(BbToolType previous, BbToolType current)
{
    switch(previous)
    {
        case BBTT_Picker:
        {
            for(auto item: items())
            {
                if(item == _pickerRect)
                {
                    continue;
                }
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,false);
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,false);
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,false);
            }
            break;
        }
        case BBTT_Triangle:
        {
            localTriangleDone(true);
            break;
        }
        case BBTT_Text:
        {
            localTextDone();
            break;
        }
        case BBTT_Pen:
        {
            localPenDone();
            break;
        }
        case BBTT_Straight:
        {
            localStraightDone();
            break;
        }
        case BBTT_Rectangle:
        {
            localRectDone();
            break;
        }
        case BBTT_Ellipse:
        {
            localEllipseDone();
            break;
        }
        default:
        {
            break;
        }
    }

    switch(current)
    {
        case BBTT_Picker:
        {
            for(auto item: items())
            {
                if(item == _pickerRect)
                {
                    continue;
                }
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
                item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void BlackboardScene::readPenData(BbItemPenData *penData)
{
    if(!penData)
    {
        qDebug() << "[BlackboardScene::readPenData] data can't be nullptr";
        return;
    }

    BbItemPen *pen = new BbItemPen(penData);
    add(pen);
    pen->repaintWithItemData();
}

void BlackboardScene::readTextData(BbItemTextData *textData)
{
    if(!textData)
    {
        qDebug() << "[BlackboardScene::readTextData] data can't be nullptr";
        return;
    }

    BbItemText *text = new BbItemText(textData);
    add(text);
    text->repaintWithItemData();
}

void BlackboardScene::readStraightData(BbItemStraightData *straightData)
{
    if(!straightData)
    {
        qDebug() << "[BlackboardScene::readStraightData] data can't be nullptr";
        return;
    }

    BbItemStraight *straight = new BbItemStraight(straightData);
    add(straight);
    straight->repaintWithItemData();
}

void BlackboardScene::readRectData(BbItemRectData *rectData)
{
    if(!rectData)
    {
        qDebug() << "[BlackboardScene::readRectData] data can't be nullptr";
        return;
    }
    BbItemRect *rect = new BbItemRect(rectData);
    add(rect);
    rect->repaintWithItemData();
}

void BlackboardScene::readEllipseData(BbItemEllipseData *ellipseData)
{
    if(!ellipseData)
    {
        qDebug() << "[BlackboardScene::readEllipseData] data can't be nullptr";
        return;
    }
    BbItemEllipse *ellipse = new BbItemEllipse(ellipseData);
    add(ellipse);
    ellipse->repaintWithItemData();
}

void BlackboardScene::readTriangleData(BbItemTriangleData *triangleData)
{
    if(!triangleData)
    {
        qDebug() << "[BlackboardScene::readTriangleData] data can't be nullptr";
        return;
    }
    BbItemTriangle *triangle = new BbItemTriangle(triangleData);
    add(triangle);
    triangle->repaintWithItemData();
}

void BlackboardScene::readImageData(BbItemImageData *data)
{
    if(!data)
    {
        qDebug() << "[BlackboardScene::readImageData] data can't be nullptr";
        return;
    }
    BbItemImage *item = new BbItemImage(data);
    add(item);
    item->repaintWithItemData();
}

void BlackboardScene::remove(QGraphicsItem *item)
{
    BbItemText * text = dynamic_cast<BbItemText *>(item);
    if(text)
    {
        text->setContentChangedCallback(nullptr);
        text->setOnFoucsOutCallback(nullptr);
    }
    if(_curElement == item)
    {
        _curElement = nullptr;
    }
    QGraphicsScene::removeItem(item);
    delete item;
}

void BlackboardScene::add(QGraphicsItem *item)
{
    BbItemText *text = dynamic_cast<BbItemText *>(item);

    if(text)
    {
        text->setOnFoucsOutCallback([this](BbItemText *text){
            text->setActive(false);
            text->setSelected(false);
            emit blackboard()->textDone(text);
        });
        text->setContentChangedCallback([this](BbItemText *text){
            emit blackboard()->textChanged(text);
        });
    }
    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,_toolType == BBTT_Picker);
    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,_toolType == BBTT_Picker);
    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,_toolType == BBTT_Picker);
    QGraphicsScene::addItem(item);
}

void BlackboardScene::setControlEnable(bool enable)
{
    _controlEnable = enable;
}

void BlackboardScene::clearItems()
{
    for(auto item: items())
    {
        if(item == _pickerRect)
        {
            continue;
        }
        remove(item);
    }
}

void BlackboardScene::readItemData(BbItemData *itemData)
{
    switch(itemData->tooltype)
    {
        case BBTT_Pen:
        {
            readPenData(dynamic_cast<BbItemPenData *>(itemData));
            return;
        }
        case BBTT_Text:
        {
            readTextData(dynamic_cast<BbItemTextData *>(itemData));
            return;
        }
        case BBTT_Straight:
        {
            readStraightData(dynamic_cast<BbItemStraightData *>(itemData));
            return;
        }
        case BBTT_Rectangle:
        {
            readRectData(dynamic_cast<BbItemRectData *>(itemData));
            return;
        }
        case BBTT_Ellipse:
        {
            readEllipseData(dynamic_cast<BbItemEllipseData *>(itemData));
            return;
        }
        case BBTT_Triangle:
        {
            readTriangleData(dynamic_cast<BbItemTriangleData *>(itemData));
            return;
        }
        case BBTT_Image:
        {
            readImageData(dynamic_cast<BbItemImageData *>(itemData));
            return;
        }
        default:
        {
            break;
        }
    }
}

void BlackboardScene::setItemIdGenerator(ItemIDGenerator3 itemIdGenerator)
{
    _itemIdGenerator = itemIdGenerator;
}

void BlackboardScene::writeStream(QDataStream &stream)
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

void BlackboardScene::readStream(QDataStream &stream)
{
#define CREATE_ITEM_AND_READ_DATA(_TOOL_TYPE_,_ITEM_CLASS_) \
    case _TOOL_TYPE_:{ \
    _ITEM_CLASS_ *item = new _ITEM_CLASS_();\
        addItem(item);\
        item->readStream(stream);\
        item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable,_toolType == BBTT_Picker);\
        item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable,_toolType == BBTT_Picker);\
        item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable,_toolType == BBTT_Picker);\
        break;\
    }

    while(!stream.atEnd())
    {
        int type;
        stream >> type;
        switch(static_cast<BbToolType>(type))
        {
            CREATE_ITEM_AND_READ_DATA(BBTT_Pen,BbItemPen);
            CREATE_ITEM_AND_READ_DATA(BBTT_Text,BbItemText);
            CREATE_ITEM_AND_READ_DATA(BBTT_Straight,BbItemStraight);
            CREATE_ITEM_AND_READ_DATA(BBTT_Rectangle,BbItemRect);
            CREATE_ITEM_AND_READ_DATA(BBTT_Ellipse,BbItemEllipse);
            CREATE_ITEM_AND_READ_DATA(BBTT_Triangle,BbItemTriangle);
            CREATE_ITEM_AND_READ_DATA(BBTT_Image,BbItemImage);
            default:
            {
                break;
            }
        }
    }
}

bool BlackboardScene::isMouseLeftButtonDown()
{
    return _mouseLeftButtonDown;
}

void BlackboardScene::addImageItem(const QPixmap &pixmap)
{
    auto item = new BbItemImage();
    item->setPixmap(pixmap);
    add(item);
    item->setZValue(QDateTime::currentMSecsSinceEpoch());
    item->setId(generatItemId());
    emit blackboard()->imageAdded(item);
}
