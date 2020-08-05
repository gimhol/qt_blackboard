#include "BbItemPen.h"
#include "BbScene.h"
#include "Blackboard.h"
#include "BbItemPenData.h"
#include <QDebug>

#ifdef NSB_BLACKBOARD_PEN_ITEM_SMOOTHING
float penSqrt(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;
    x = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 ); //魔术数
    y = * ( float * ) &i;
    y = y * ( f - ( x * y * y ) ); //迭代1 1/sqrt(number)
    return number * y;
}
#endif

BbItemPen::BbItemPen():
    QGraphicsRectItem(),
    _data(new BbItemPenData())
{
    init();
}

BbItemPen::BbItemPen(BbItemData *data):
    QGraphicsRectItem(),
    _data(dynamic_cast<BbItemPenData *>(data))
{
    init();
}

void BbItemPen::init()
{
    if(!_data)
    {
        _data = new BbItemPenData();
    }
#ifdef LINE_SMOOTHING
    _distances[0] = -1;
    _distances[1] = -1;
    _distances[2] = -1;
#endif
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}
BbItemPen::~BbItemPen()
{
    if(_data){
        delete _data;
        _data = nullptr;
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
}

void BbItemPen::penDown(const QPointF &point){
#ifdef QT_DEBUG
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
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

void BbItemPen::penDraw(const QPointF &point)
{
#ifdef QT_DEBUG
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
}

void BbItemPen::penStraighting(const QPointF &point)
{
#ifdef QT_DEBUG
    qInfo() << __FUNCTION__ << point;
#endif
    setStraight(true);
    _changed.clear();
    straightLineDragging(point);
    setRect(_rect);
    update();
}

void BbItemPen::done()
{
#ifdef QT_DEBUG
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

    _rect.setX(x - halfPenW);
    _rect.setY(y - halfPenW);
    _rect.setWidth(w + 2 * halfPenW);
    _rect.setHeight(h + 2 * halfPenW);

    update();
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

void BbItemPen::addPointToPath(const QPointF &point)
{
    _changed.append(point);
    _data->coords.append(point.x());
    _data->coords.append(point.y());
    qreal halfPenW = 0.5 * _data->pen.widthF();
    qreal oldLeft = pos().x();
    qreal oldTop = pos().y();
    qreal newLeft = std::min(point.x(), oldLeft);
    qreal newTop = std::min(point.y(), oldTop);

    _path.translate(oldLeft-newLeft,oldTop-newTop);    // 重新计算左上角位置
    _path.lineTo(point - QPointF(newLeft, newTop));

     setPos(newLeft, newTop);

    _rect = _path.boundingRect();
    _rect.setX(_rect.x()-halfPenW);
    _rect.setY(_rect.y()-halfPenW);
    _rect.setWidth(_rect.width()+halfPenW);
    _rect.setHeight(_rect.height()+halfPenW);
    _data->updatePostion(this);
    _data->updatePrevPostion();
}

#ifdef NSB_BLACKBOARD_PEN_ITEM_SMOOTHING
void BbItemPen::appendPointSmoothing(const QPointF &point)
{
    qreal halfPenW = 0.5 * _data->pen.widthF();
    _temp.append(point - QPointF(halfPenW,halfPenW));
    if(_temp.length() == 4){
        // 曲线圆滑

        const QPointF *a = &_temp.at(0);
        const QPointF *b = &_temp.at(1);
        const QPointF *c = &_temp.at(2);
        const QPointF *d = &_temp.at(3);

        static int cutting = 0;
        static qreal t, tt, ttt, tt2, tt3, ttt2 = 0;

        if(_distances[1] < 0){
            _distances[0] = penSqrt(
                                    std::powf(float(b->x()-a->x()),2) +
                                    std::powf(float(b->y()-a->y()),2)
                                    );
        }
        else{
            _distances[0] = _distances[1];
        }

        if(_distances[2] < 0){
            _distances[1] = penSqrt(
                        std::powf(float(b->x()-c->x()),2) +
                        std::powf(float(b->y()-c->y()),2)
                        );
        }else{
            _distances[1] = _distances[2];
        }
        _distances[2] = penSqrt(
                    std::powf(float(d->x()-c->x()),2) +
                    std::powf(float(d->y()-c->y()),2)
                    );

        cutting = int(_distances[1] / NSB_BLACKBOARD_PEN_SMOOTHING_UNIT);
        if (cutting > 0 && _distances[0] * 2 > 0.1f * (_distances[1] + _distances[2])) {

            for (int i = 0; i < cutting; ++i) {
                t = qreal(i) / (cutting);
                tt = t*t;
                ttt = tt*t;
                tt2 = tt*2;
                tt3 = tt*3;
                ttt2 = ttt*2;
                QPointF p =
                        (*b) * (ttt2 - tt3 + 1) +
                        (*c) * (-ttt2 + tt3 ) +
                        0.5 * ((*c)-(*a)) * (ttt - tt2 + t) +
                        0.5 * ((*d)-(*b))  * (ttt - tt);

                addPointToPath(p);
            }
        }
        else {
            addPointToPath(*c);
        }
        _temp.pop_front();
    }
    else if(_temp.length() == 1)
    {
        addPointToPath(point- QPointF(halfPenW,halfPenW));
    }
}
#endif

void BbItemPen::repaint()
{
    _path.clear();
    qreal halfPenW = 0.5 * _data->pen.widthF();
    for(int i = 0; i < _data->coords.length(); i+=2)
    {
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
        _path.translate(oldLeft-newLeft,oldTop-newTop);    // 重新计算左上角位置
        _path.lineTo(point - QPointF(newLeft, newTop));
         setPos(newLeft, newTop);
        _rect = _path.boundingRect();
        _rect.setX(_rect.x()-halfPenW);
        _rect.setY(_rect.y()-halfPenW);
        _rect.setWidth(_rect.width()+halfPenW);
        _rect.setHeight(_rect.height()+halfPenW);
        setRect(_rect);
    }
    if(_data->isPositionValid()){
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
    {
        return;
    }

    // 开/关直线模式
    if(straight)
    {
        _straightFrom = QPointF(-999999, -999999);
        _straightTo = QPointF(-999999, -999999);
    }
    else if(_straightTo.x() > -999998)
    {
        _mousePos = _straightTo + pos();
        _changed.clear();
        addPointToPath(_mousePos);
    }
    _straight = straight;
}

QPointF BbItemPen::straightTo()
{
    return _straightTo + pos();
}

void BbItemPen::writeStream(QDataStream &stream)
{
    _data->updatePostion(this);
    _data->z = zValue();
    _data->writeStream(stream);
}

void BbItemPen::readStream(QDataStream &stream)
{
    _data->readStream(stream);
    absolutize();
    repaint();
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
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    setId(blackboard()->factory()->makeItemId());
    setZ(blackboard()->factory()->makeItemZ());
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
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << pos;
#endif
    setStraight(bbScene()->modifiers()==Qt::ShiftModifier);
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
        emit blackboard()->itemChanged(BBIET_penDraw,this);
    }
    else
    {
        penStraighting(pos);
        emit blackboard()->itemChanged(BBIET_penStraighting,this);
    }
}

void BbItemPen::toolDone(const QPointF &pos)
{
#ifdef QT_DEBUG
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

