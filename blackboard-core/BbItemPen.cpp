#include "BbItemPen.h"
#include "BbScene.h"
#include "Blackboard.h"
#include "BbItemPenData.h"
#include "BbItemInnerDataKey.h"
#include <QDebug>
#include <QMessageBox>

#ifdef NSB_PEN_DEBUG
static int deconstruct_count = 0;
static int construct_count = 0;
#endif

BbItemPen::BbItemPen():
    QGraphicsRectItem(),
    _data(new BbItemPenData())
{
#ifdef NSB_PEN_DEBUG
    construct_count++;
#endif
    init();
}

BbItemPen::BbItemPen(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemPenData *>(data))
{
#ifdef NSB_PEN_DEBUG
    construct_count++;
#endif
    init();
}

void BbItemPen::init()
{
    if(!_data)
    {
        _data = Blackboard::defaultFactory()->createItemData<BbItemPenData>(BBTT_Pen);
    }
    setId(_data->lid);
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    setData(BBIIDK_ITEM_IS_SHAPE,true);
}

BbItemPen::~BbItemPen()
{
#ifdef NSB_PEN_DEBUG
    ++deconstruct_count;
    qDebug() << __func__ << int(this) << deconstruct_count << "remain:" << (construct_count-deconstruct_count);
#endif
    if(_data){
        delete _data;
    }
}

QList<QPointF> *BbItemPen::changed(){ return &_changed ;}

bool BbItemPen::isEmpty() { return _data->empty; }

void BbItemPen::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(!_path.isEmpty()){
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(_data->pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(_path);
    }
    else if(!_straight || (_straight && _straightFrom == _straightTo)){ // draw a dot.
        auto halfPenW = 0.5 * _data->pen.widthF();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(_data->pen.color());
        QRectF rect(-halfPenW,-halfPenW,2*halfPenW,2*halfPenW);
        painter->drawEllipse(rect);
    }
    if(_straight && _straightFrom != _straightTo){ // draw a straight line
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(_data->pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(_straightFrom, _straightTo);
    }
    QGraphicsRectItem::paint(painter,option,widget);
}

void BbItemPen::penDown(const QPointF &point){
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__ << point;
#endif
    _editing = true;
    setPos(point);
    _data->empty = false;
    _path.moveTo(0,0);
    _mousePos = point;
    addPointToPath(point);
    setRect(_rect);
    if(_straight){
        _straightFrom = _mousePos - point;
        _straightTo = _straightFrom;
    }
    update();
    _data->fixPostion(this);
}

void BbItemPen::penDraw(const QPointF &point)
{
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__ << point;
#endif
    _changed.clear();

    _mousePos = point;

    addPointToPath(point);
    setRect(_rect);

    update();
}

void BbItemPen::penStraighting(const QPointF &point)
{
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__ << point;
#endif
    _changed.clear();
    straightLineDragging(point);
    setRect(_rect);
    update();
}

void BbItemPen::done()
{
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__;
#endif

    _editing = false;
    _data->fixPostion(this);
}

void BbItemPen::straightLineDragging(const QPointF &point)
{
    _straightTo.setX(point.x() - pos().x());
    _straightTo.setY(point.y() - pos().y());
    if(_straightFrom.x() < -999998)
    {
        _straightFrom = _mousePos - pos();
    }

    qreal halfPenW = 0.5 * _data->pen.widthF();
    qreal oldLeft = pos().x();
    qreal oldTop = pos().y();
    qreal newLeft = std::min(point.x(), oldLeft);
    qreal newTop = std::min(point.y(), oldTop);


    _straightTo += QPointF(oldLeft - newLeft,oldTop - newTop);
    _straightFrom += QPointF(oldLeft - newLeft,oldTop - newTop);
    _path.translate(oldLeft - newLeft,oldTop - newTop);
    setPos(newLeft, newTop);

    QRectF pathRect = _path.boundingRect();

    qreal x = std::min(pathRect.left(), _straightTo.x());
    qreal y = std::min(pathRect.top(), _straightTo.y());
    qreal w = std::max(pathRect.right(), _straightTo.x()) - x;
    qreal h = std::max(pathRect.bottom(), _straightTo.y()) - y;

    _rect.moveLeft(x - halfPenW);
    _rect.moveTop(y - halfPenW);
    _rect.setWidth(w + 2 * halfPenW);
    _rect.setHeight(h + 2 * halfPenW);

    update();
    _data->fixPostion(this);
}

void BbItemPen::addPointToPath(const QPointF &point)
{
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__ << (_data->coords.size()/2) << ", point: " <<point;
#endif

    _changed.append(point);

    auto isStraightEnd = false;
    auto dontAddToPath = false;
    if(_data->coords.size() >= 2){
        auto itr = _data->coords.rbegin();
        const auto prevY = *itr;
        const auto prevX = *(++itr);
        dontAddToPath = point.x() == prevX && point.y() == prevY;
    }
    if(_data->coords.size() >= 4){
        auto itr = _data->coords.rbegin();
        const auto prevY0 = *itr;
        const auto prevX0 = *(++itr);
        const auto prevY1 = *(++itr);
        const auto prevX1 = *(++itr);
        isStraightEnd = prevX1 == prevX0 && prevY1 == prevY0;
    }

    _data->coords.append(point.x());
    _data->coords.append(point.y());

    if(dontAddToPath)
        return;

    qreal halfPenW = 0.5 * _data->pen.widthF();
    qreal oldLeft = pos().x();
    qreal oldTop = pos().y();
    qreal newLeft = qMin(point.x(), oldLeft);
    qreal newTop = qMin(point.y(), oldTop);

    _path.translate(oldLeft-newLeft,oldTop-newTop);    // 重新计算左上角位置

    auto dx = point.x() - newLeft;
    auto dy = point.y() - newTop;

    if(_data->cubic){
        auto elementCount = _path.elementCount();
        if(elementCount > 1 && !isStraightEnd){
            auto a = _path.elementAt(elementCount-3);
            _path.setElementPositionAt(elementCount-1,(dx+a.x)/2,(dy+a.y)/2);
            _path.setElementPositionAt(elementCount-2,a.x,a.y);
            _path.setElementPositionAt(elementCount-3,a.x,a.y);
        }
        _path.cubicTo(dx,dy,dx,dy,dx,dy);
    }else{
        _path.lineTo(dx,dy);
    }
    setPos(newLeft, newTop);

    _rect = _path.boundingRect();
    _rect.moveLeft(_rect.x()-halfPenW);
    _rect.moveTop(_rect.y()-halfPenW);
    _rect.setWidth(_rect.width()+2*halfPenW);
    _rect.setHeight(_rect.height()+2*halfPenW);
    _data->fixPostion(this);
}

void BbItemPen::repaint()
{
    _path.clear();
    auto coords = _data->coords;
    _data->coords = QList<qreal>();
    auto finalX = _data->x;
    auto finalY = _data->y;
    for(int i = 0; i < coords.length(); i+=2)
    {
        QPointF point(coords[i], coords[i+1]);
        if(i == 0)
            penDown(point);
        else
            penDraw(point);
        if(i == coords.length() - 2)
            done();
    }
    _changed.clear();
    _data->x = finalX;
    _data->y = finalY;
    if(_data->isPositionValid())
    {
        moveToPosition(_data->x,_data->y);
        updatePrevPosition();
    }
    setZ(_data->z);
    updatePrevZ();
    update();
}

bool BbItemPen::straight()
{
    return _straight;
}

void BbItemPen::setStraight(const bool & straight)
{
    if(straight == _straight)
        return;

    // 开/关直线模式
    if(straight)
    {
        _straightFrom = QPointF(-999999, -999999);
        _straightTo = QPointF(-999999, -999999);
        if(_data->coords.size() >= 4){
            auto itr = _data->coords.rbegin();
            auto y0 = *itr;
            auto x0 = *(++itr);
            auto y1 = *(++itr);
            auto x1 = *(++itr);
            if(x1 != x0 || y1 != y0){
                addPointToPath(QPointF(x0,y0));
            }
        } else if(_data->coords.size() >= 2){
            auto itr = _data->coords.rbegin();
            auto y0 = *itr;
            auto x0 = *(++itr);
            addPointToPath(QPointF(x0,y0));
        }
    }
    else if(_straightTo.x() > -999998)
    {
        _changed.clear();
        _mousePos = straightTo();
        addPointToPath(_mousePos);
        addPointToPath(_mousePos);
    }
    _straight = straight;
}

QPointF BbItemPen::straightTo()
{
    return _straightTo + pos();
}

QJsonObject BbItemPen::toJsonObject()
{
    _data->updatePostion(this);
    _data->z = zValue();
    return _data->toJsonObject();
}

void BbItemPen::fromJsonObject(const QJsonObject &jobj)
{
    _data->fromJsonObject(jobj);
    absolutize();
    repaint();
}

BbItemData *BbItemPen::data()
{
    return _data;
}

void BbItemPen::toolDown(const QPointF &pos)
{
#ifdef NSB_PEN_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    setId(blackboard()->factory()->makeItemId(toolType()));
    setZ(blackboard()->factory()->makeItemZ(toolType()));
    updatePrevZ();

    setStraight(bbScene()->modifiers() & Qt::ShiftModifier);
    auto settings = blackboard()->toolSettings<BbItemPenData>(BBTT_Pen);
    _data->setWeight(settings->weight());
    _data->pen = settings->pen;
    penDown(pos);
    bbScene()->setCurrentItem(this);
    emit blackboard()->itemChanged(BBIET_penDown,this);
}

void BbItemPen::toolDraw(const QPointF &pos)
{
#ifdef NSB_PEN_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    if(_mousePos == pos){
#ifdef NSB_PEN_DEBUG
        qDebug() << __FUNCTION__ << "got same mouse pos!";
#endif
        return;
    }
    setStraight(bbScene()->modifiers() == Qt::ShiftModifier);
    if(!straight())
    {
        penDraw(pos);
        emit blackboard()->itemChanged(BBIET_penDraw,this);
    }
    else
    {
        penStraighting(pos);
        emit blackboard()->itemChanged(BBIET_penStraighting,this);
    }
}

void BbItemPen::toolUp(const QPointF &pos)
{
#ifdef NSB_PEN_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    if(straight())
    {
        setStraight(false);
        emit blackboard()->itemChanged(BBIET_penDraw,this);
    }
    done();
    emit blackboard()->itemChanged(BBIET_penDone,this);
    bbScene()->unsetCurrentItem(this);
}

void BbItemPen::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    if(_straight != (modifiers == Qt::ShiftModifier))
    {
        setStraight(modifiers == Qt::ShiftModifier);
        if(!_straight)
        {
            emit blackboard()->itemChanged(BBIET_penDraw,this);
        }
    }
}

void BbItemPen::absolutize()
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
        for(int i=0;i<_data->coords.length();++i)
        {
            _data->coords[i] = _data->coords[i] * ratio;
        }
    }
}

bool BbItemPen::isEditing()
{
    return _editing;
}

