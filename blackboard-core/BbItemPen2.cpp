#include "BbItemPen2.h"
#include "BbScene.h"
#include "Blackboard.h"
#include "BbItemPenData2.h"
#include <QDebug>
#include <QMessageBox>

BbItemPen2::BbItemPen2():
    QAbstractGraphicsShapeItem(),
    _data(new BbItemPenData2())
{
    init();
}

BbItemPen2::BbItemPen2(BbItemData *data):
    QAbstractGraphicsShapeItem(),
    _data(dynamic_cast<BbItemPenData2 *>(data))
{
    init();
}

void BbItemPen2::init()
{
    if(!_data)
    {
        _data = Blackboard::defaultFactory()->createItemData<BbItemPenData2>(BBTT_Pen);
    }
    setId(_data->lid);
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}

BbItemPen2::~BbItemPen2()
{
    if(_data){
        delete _data;
    }
}

QList<QPointF> *BbItemPen2::changed(){ return &_changed ;}

bool BbItemPen2::isEmpty() { return _data->empty; }

void BbItemPen2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
#if 0
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
#ifdef NSB_SAVE_PEN_TO_PIXMAP_WHEN_DONE
    if(!_pixmap.isNull()){  // draw a finished pixmap
        auto w = int(rect().width());
        auto h = int(rect().height());
        auto halfPenW = int(_data->pen.widthF()/2);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->drawPixmap(-halfPenW,-halfPenW,w,h,_pixmap);
    }
#endif
    QGraphicsRectItem::paint(painter,option,widget);
#else
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPath(_path);
//    QAbstractGraphicsShapeItem::paint(painter,option,widget);
#endif
}

void BbItemPen2::penDown(const QPointF &point){
#if 0
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
        addPointToPath(point);
    }
    update();
    _data->updatePostion(this);
    _data->updatePrevPostion();
#else
    _path.moveTo(point);
    _changed << point;
    _data->coords << point.x() << point.y();
    _data->updatePostion(this);
    _data->updatePrevPostion();
    update();
#endif
}

void BbItemPen2::penDraw(const QPointF &point)
{
#if 0
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__ << point;
#endif
    setStraight(false);

    _changed.clear();

    _mousePos = point;

#ifdef NSB_BLACKBOARD_PEN_ITEM_SMOOTHING
    appendPointSmoothing(point);
#else
    addPointToPath(point);
#endif
    setRect(_rect);

    update();
#else
    _path.lineTo(point);
    _changed << point;
    _data->coords << point.x() << point.y();
    _data->updatePostion(this);
    _data->updatePrevPostion();
    update();
#endif
}

void BbItemPen2::penStraighting(const QPointF &point)
{
#if 0
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__ << point;
#endif
    setStraight(true);
    _changed.clear();
    straightLineDragging(point);
    setRect(_rect);
    update();
#else
    auto ec = _path.elementCount();
    _path.setElementPositionAt(ec-1, point.x(), point.y());
    update();
#endif
}

void BbItemPen2::done()
{
#if 0
#ifdef NSB_PEN_DEBUG
    qInfo() << __FUNCTION__;
#endif

    _editing = false;
    _data->updatePostion(this);
    _data->updatePrevPostion();

#ifdef NSB_SAVE_PEN_TO_PIXMAP_WHEN_DONE
    do{
        if(!blackboard())
            break;
        auto scale = 4; // 倍数绘制, 保证放大后没那么模糊。但在放得过大得情况下无效。
        auto width = int(scale * rect().width());
        auto height = int(scale * rect().height());
        auto halfPenW = _data->pen.widthF()/2;
        _pixmap = QPixmap(width,height);
        _pixmap.fill(Qt::transparent);
        QPainter painter(&_pixmap);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.translate(scale*halfPenW,
                          scale*halfPenW);
        painter.scale(scale,scale);

        if(_path.isEmpty()){
            painter.setPen(Qt::NoPen);
            painter.setBrush(_data->pen.color());
            QRectF rect(-halfPenW,-halfPenW,2*halfPenW,2*halfPenW);
            painter.drawEllipse(rect);
        }
        else{
            painter.setPen(_data->pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawPath(_path);
        }
        _path.clear();
        update();
    }while(false);
#endif
#else
    _editing = false;
#endif
}

void BbItemPen2::straightLineDragging(const QPointF &point)
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
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

void BbItemPen2::repaint()
{
#if 0
    _path.clear();
    qreal halfPenW = 0.5 * _data->pen.widthF();
    for(int i = 0; i < _data->coords.length(); i+=2)
    {
        bool straight = false;
        if(_data->coords.length() >= 6){
            auto itr = _data->coords.rbegin()++;
            auto prevY0 = *(++itr);
            auto prevX0 = *(++itr);
            auto prevY1 = *(++itr);
            auto prevX1 = *(++itr);
            straight = (prevY1 == prevY0 && prevX0 == prevX1);
        }

        QPointF point(_data->coords[i],_data->coords[i+1]);
        if(i == 0){
            _path = QPainterPath();
            _path.moveTo(0,0);
            setPos(point);
        }
        qreal oldLeft = pos().x();
        qreal oldTop = pos().y();
        qreal newLeft = std::min(point.x(), oldLeft);
        qreal newTop = std::min(point.y(), oldTop);
        auto dx = point.x() - newLeft;
        auto dy = point.y() - newTop;
        _path.translate(oldLeft-newLeft,oldTop-newTop);    // 重新计算左上角位置
        if(_data->cubic){
            auto elementCount = _path.elementCount();
            if(elementCount >= 3 && !straight){
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
        setRect(_rect);
    }
    if(_data->isPositionValid()){
        moveToPosition(_data->x,_data->y);
        updatePrevPosition();
    }
    setZ(_data->z);
    updatePrevZ();
    update();
#endif
}

bool BbItemPen2::straight()
{
    return _straight;
}

void BbItemPen2::setStraight(const bool & straight)
{
    if(straight == _straight)
    {
        return;
    }

    // 开/关直线模式
    if(straight)
    {
//        _straightFrom = QPointF(-999999, -999999);
//        _straightTo = QPointF(-999999, -999999);
//        _changed.append(_mousePos);
//        _changed.append(_mousePos);
//        _changed.append(_mousePos);
//        addPointToPath(_mousePos);
//        addPointToPath(_mousePos);
//        addPointToPath(_mousePos);
    }
    else if(_straightTo.x() > -999998)
    {
//        _mousePos = _straightTo + pos();
//        _changed.clear();
//        addPointToPath(_mousePos);
//        addPointToPath(_mousePos);
    }
    _straight = straight;
}

QPointF BbItemPen2::straightTo()
{
    return _straightTo + pos();
}

QRectF BbItemPen2::boundingRect() const
{
    return _path.boundingRect();
}

void BbItemPen2::writeStream(QDataStream &stream)
{
    _data->updatePostion(this);
    _data->z = zValue();
    _data->writeStream(stream);
}

void BbItemPen2::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
}

QJsonObject BbItemPen2::toJsonObject()
{
    _data->updatePostion(this);
    _data->z = zValue();
    return _data->toJsonObject();
}

void BbItemPen2::fromJsonObject(const QJsonObject &jobj)
{
    _data->fromJsonObject(jobj);
    absolutize();
    repaint();
}

BbItemData *BbItemPen2::data()
{
    return _data;
}

void BbItemPen2::toolDown(const QPointF &pos)
{
#ifdef NSB_PEN_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    setId(blackboard()->factory()->makeItemId(toolType()));
    setZ(blackboard()->factory()->makeItemZ(toolType()));
    updatePrevZ();

    setStraight(bbScene()->modifiers() & Qt::ShiftModifier);
    auto settings = blackboard()->toolSettings<BbItemPenData2>(BBTT_Pen2);
    _data->setWeight(settings->weight());
    _data->pen = settings->pen;
    setPen(_data->pen);
    penDown(pos);
    bbScene()->setCurrentItem(this);
    emit blackboard()->itemChanged(BBIET_penDown2,this);
}

void BbItemPen2::toolDraw(const QPointF &pos)
{
#ifdef NSB_PEN_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    setStraight(bbScene()->modifiers() == Qt::ShiftModifier);
    if(!straight())
    {
        /*
         * Note:
         * 当黑板按下鼠标右键弹出某个菜单，然后点击左键其他地方取消菜单。
         * 会触发一个与mouseDown事件，一个mouseMove事件。
         * 他们的鼠标坐标是相同的，在会让paint事件的逻辑会画不出任何东西。
         * 这里排除掉次坐标
         *  -Gim
         */
//        if(_data->coords.size() >= 2){
//            auto itr = _data->coords.rbegin();
//            auto y = *itr;
//            auto x = *(++itr);
//            if(pos == QPointF(x,y))
//                return;
//        }
        penDraw(pos);
        emit blackboard()->itemChanged(BBIET_penDraw2,this);
    }
    else
    {
        penStraighting(pos);
        emit blackboard()->itemChanged(BBIET_penStraighting2,this);
    }
}

void BbItemPen2::toolDone(const QPointF &pos)
{
#ifdef NSB_PEN_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    if(straight())
    {
        setStraight(false);
        emit blackboard()->itemChanged(BBIET_penDraw2,this);
    }
    done();
    emit blackboard()->itemChanged(BBIET_penDone2,this);
    bbScene()->unsetCurrentItem(this);
}

void BbItemPen2::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    if(_straight != (modifiers == Qt::ShiftModifier))
    {
        setStraight(modifiers == Qt::ShiftModifier);
        if(!_straight)
        {
            emit blackboard()->itemChanged(BBIET_penDraw2,this);
        }
    }
}

void BbItemPen2::absolutize()
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

bool BbItemPen2::isEditing()
{
    return _editing;
}

