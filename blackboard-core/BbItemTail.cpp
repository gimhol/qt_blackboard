#include "BbItemTail.h"
#include "BbScene.h"
#include "Blackboard.h"
#include "BbItemTailData.h"
#include "BbItemInnerDataKey.h"
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsItemAnimation>

BbItemTail::BbItemTail():
    QGraphicsRectItem(),
    _data(new BbItemTailData())
{
    init();
}

BbItemTail::BbItemTail(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemTailData *>(data))
{
    init();
}

void BbItemTail::init()
{
    if(!_data)
    {
        _data = Blackboard::defaultFactory()->createItemData<BbItemTailData>(BBTT_Tail);
    }
    setId(_data->lid);
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}

BbItemTail::~BbItemTail()
{
    if(_data){
        delete _data;
    }
}

QList<QPointF> *BbItemTail::changed(){ return &_changed ;}

bool BbItemTail::isEmpty() { return _data->empty; }

void BbItemTail::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
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

void BbItemTail::penDown(const QPointF &point){
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

void BbItemTail::penDraw(const QPointF &point)
{
    _changed.clear();
    _mousePos = point;
    addPointToPath(point);
    setRect(_rect);
    update();
}

void BbItemTail::penStraighting(const QPointF &point)
{
    _changed.clear();
    straightLineDragging(point);
    setRect(_rect);
    update();
}

void BbItemTail::done()
{
    _data->fixPostion(this);

    auto fadeOutTimer = new QTimer(this);
    auto interval = 1000/30;
    auto delay = _data->fadeOutDelay;
    fadeOutTimer->setInterval(interval);
    connect(fadeOutTimer,&QTimer::timeout,this,[&,delay,interval,fadeOutTimer](){
        _data->fadeOutDuration -= interval;
        if(_data->fadeOutDuration <= 0){
            setOpacity(0);
            fadeOutTimer->stop();
            deleteLater();
        }else{
            setOpacity(1.0 * _data->fadeOutDuration / delay);
        }
    });
    QTimer::singleShot(_data->fadeOutDelay,fadeOutTimer,static_cast<void(QTimer::*)()>(&QTimer::start));
}

void BbItemTail::straightLineDragging(const QPointF &point)
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

void BbItemTail::addPointToPath(const QPointF &point)
{
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

void BbItemTail::repaint()
{
    _path.clear();
    auto coords = _data->coords;
    _data->coords = QList<qreal>();
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
    if(_data->isPositionValid())
    {
        moveToPosition(_data->x,_data->y);
        updatePrevPosition();
    }
    setZ(_data->z);
    updatePrevZ();
    update();
}

bool BbItemTail::straight()
{
    return _straight;
}

void BbItemTail::setStraight(const bool & straight)
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

QPointF BbItemTail::straightTo()
{
    return _straightTo + pos();
}

BbItemData *BbItemTail::data()
{
    return _data;
}

void BbItemTail::toolDown(const QPointF &pos)
{
    setId(blackboard()->factory()->makeItemId(toolType()));
    setZ(blackboard()->factory()->makeItemZ(toolType()));
    updatePrevZ();

    setStraight(bbScene()->modifiers() & Qt::ShiftModifier);
    auto settings = blackboard()->toolSettings<BbItemTailData>(BBTT_Tail);
    _data->setWeight(settings->weight());
    _data->pen = settings->pen;
    penDown(pos);
    bbScene()->setCurrentItem(this);
    emit blackboard()->itemChanged(BBIET_tailDown,this);
}

void BbItemTail::toolDraw(const QPointF &pos)
{
    if(_mousePos == pos){
        return;
    }
    setStraight(bbScene()->modifiers() == Qt::ShiftModifier);
    if(!straight())
    {
        penDraw(pos);
        emit blackboard()->itemChanged(BBIET_tailDraw,this);
    }
    else
    {
        penStraighting(pos);
        emit blackboard()->itemChanged(BBIET_tailStraighting,this);
    }
}

void BbItemTail::toolUp(const QPointF &pos)
{
    if(straight())
    {
        setStraight(false);
        emit blackboard()->itemChanged(BBIET_tailDraw,this);
    }
    done();
    emit blackboard()->itemChanged(BBIET_tailDone,this);
    bbScene()->unsetCurrentItem(this);
}

void BbItemTail::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    if(_straight != (modifiers == Qt::ShiftModifier))
    {
        setStraight(modifiers == Qt::ShiftModifier);
        if(!_straight)
        {
            emit blackboard()->itemChanged(BBIET_tailDraw,this);
        }
    }
}

void BbItemTail::absolutize()
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

void BbItemTail::added()
{
    setFlag(QGraphicsItem::ItemIsMovable,false);
    setFlag(QGraphicsItem::ItemIsSelectable,false);
    setFlag(QGraphicsItem::ItemIsFocusable,false);
}
