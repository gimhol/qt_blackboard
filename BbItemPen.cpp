#include "BbItemPen.h"
#include "BbScene.h"
#include "Blackboard.h"
#include "BbItemPenData.h"
#include <QDebug>
#include <QDateTime>

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
    IItemIndex(nullptr),
    _myData(new BbItemPenData())
{
    init();
}

BbItemPen::BbItemPen(BbItemData *data):
    QGraphicsRectItem(),
    IItemIndex(data),
    _myData(dynamic_cast<BbItemPenData *>(data))
{
    init();
}

void BbItemPen::init()
{
    if(!_myData)
    {
        _myData = new BbItemPenData();
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
    if(_myData){
        delete _myData;
        _myData = nullptr;
    }

    if(_path){
        delete _path;
        _path = nullptr;
    }

#ifdef SAVE_TO_PIXMAP_WHEN_DONE
    if(_pixmap){
        delete _pixmap;
        _pixmap = nullptr;
    }
#endif
}

QList<QPointF> *BbItemPen::changed(){ return &_changed ;}

bool BbItemPen::isEmpty() { return _myData->empty; }

void BbItemPen::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

    QGraphicsRectItem::paint(painter,option,widget);

    if(_path != nullptr)
    {
        if(_myData->coords.length() == 2)
        {
            qreal halfPenW = 0.5 * _myData->pen.widthF();
            painter->drawEllipse(_myData->coords[0] - halfPenW,
                                 _myData->coords[1] - halfPenW,
                                 2*halfPenW,
                                 2*halfPenW);
        }

        painter->setPen(_myData->pen);
        painter->setBrush(Qt::NoBrush);
        if(_path->length() < 2000)
        {
            painter->setRenderHint(QPainter::Antialiasing, true);
        }
        painter->drawPath(*_path);
        if(_straight && _straightLineFrom.x() > -999998)
        {
            painter->drawLine(_straightLineFrom, _straightLineTo);
        }
    }
#ifdef SAVE_TO_PIXMAP_WHEN_DONE
    if(_pixmap != nullptr){
        int width = rect().width();
        int height = rect().height();
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter->drawPixmap(0,0,width,height,*_pixmap);
    }
#endif
}

void BbItemPen::penDown(const QPointF &point){
    _editing = true;
    setPos(point);
    _myData->empty = false;
    if(_path == nullptr)
    {
        _path = new QPainterPath();
    }
    _path->moveTo(0,0);
    _mousePos = point;
    addPointToPath(point);
    if(_straight)
    {
        _straightLineFrom = QPointF(0,0);
    }
    update();
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
}

void BbItemPen::penDraw(const QPointF &point)
{
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
    setStraight(true);
    _changed.clear();
    straightLineDragging(point);
    setRect(_rect);
    update();
}

void BbItemPen::done()
{
    _editing = false;
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
#ifdef SAVE_TO_PIXMAP_WHEN_DONE

    do{
        if(_path == nullptr){
            break;
        }
        BlackboardScene *canvasScene = dynamic_cast<BlackboardScene *>(scene());
        if(!canvasScene){

            qDebug() << "not a cavnasScene!";
            break;
        }
        QList <QGraphicsView *> views = canvasScene->views();

        if(views.isEmpty()){
            break;
        }

        Blackboard *canvasView = dynamic_cast<Blackboard *>(views[0]);

        if( !canvasView ){
            qDebug() << "not a canvasView!";
            break;
        }
        float pixmapScale = canvasView->scaleRatio();

        int width = (int)(pixmapScale * rect().width());
        int height = (int)(pixmapScale * rect().height());

        _pixmap = new QPixmap(width,height);

        _pixmap->fill(Qt::transparent);

        QPainter painter(_pixmap);

        painter.scale(pixmapScale,pixmapScale);

        painter.setPen(_myData->pen);
        painter.setBrush(Qt::NoBrush);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.drawPath(*_path);

        delete _path;
        _path = nullptr;
        update();

    }while(false);
#endif
}

QColor BbItemPen::color(){
    return _myData->pen.color();
}

void BbItemPen::setColor(const QColor &color)
{
    _myData->pen.setColor(color);
}

qreal BbItemPen::penWidth()
{
    return _myData->pen.widthF();
}

qreal BbItemPen::weight()
{
    return (_myData->pen.widthF() - BbItemPenData::getMinWidth())  / (BbItemPenData::getMaxWidth() - BbItemPenData::getMinWidth());
}

void BbItemPen::setPenWidth(qreal width)
{
    _myData->pen.setWidthF(width);
}

void BbItemPen::setWeight(qreal weight)
{
    _myData->setWeight(weight);
}

void BbItemPen::straightLineDragging(const QPointF &point)
{
    _straightLineTo.setX(point.x() - pos().x());
    _straightLineTo.setY(point.y() - pos().y());
    if(_straightLineFrom.x() < -999998)
    {
        _straightLineFrom = _mousePos - pos();
    }

    qreal halfPenW = 0.5 * _myData->pen.widthF();
    qreal oldLeft = pos().x();
    qreal oldTop = pos().y();
    qreal newLeft = std::min(point.x(), oldLeft);
    qreal newTop = std::min(point.y(), oldTop);


    _straightLineTo += QPointF(oldLeft - newLeft,oldTop - newTop);
    _straightLineFrom += QPointF(oldLeft - newLeft,oldTop - newTop);
    _path->translate(oldLeft - newLeft,oldTop - newTop);
    setPos(newLeft, newTop);

    QRectF pathRect = _path->boundingRect();

    qreal x = std::min(pathRect.left(), _straightLineTo.x());
    qreal y = std::min(pathRect.top(), _straightLineTo.y());
    qreal w = std::max(pathRect.right(), _straightLineTo.x()) - x;
    qreal h = std::max(pathRect.bottom(), _straightLineTo.y()) - y;

    _rect.setX(x - halfPenW);
    _rect.setY(y - halfPenW);
    _rect.setWidth(w + 2 * halfPenW);
    _rect.setHeight(h + 2 * halfPenW);

    update();
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
}

void BbItemPen::addPointToPath(const QPointF &point)
{
    if(_path == nullptr)
    {
        return;
    }
    _changed.append(point);
    _myData->coords.append(point.x());
    _myData->coords.append(point.y());
    qreal halfPenW = 0.5 * _myData->pen.widthF();
    qreal oldLeft = pos().x();
    qreal oldTop = pos().y();
    qreal newLeft = std::min(point.x(), oldLeft);
    qreal newTop = std::min(point.y(), oldTop);

    _path->translate(oldLeft-newLeft,oldTop-newTop);    // 重新计算左上角位置
    _path->lineTo(point - QPointF(newLeft, newTop));

     setPos(newLeft, newTop);

    _rect = _path->boundingRect();
    _rect.setX(_rect.x()-halfPenW);
    _rect.setY(_rect.y()-halfPenW);
    _rect.setWidth(_rect.width()+halfPenW);
    _rect.setHeight(_rect.height()+halfPenW);
    _myData->updatePostion(this);
    _myData->updatePrevPostion();
}

#ifdef NSB_BLACKBOARD_PEN_ITEM_SMOOTHING
void BbItemPen::appendPointSmoothing(const QPointF &point)
{
    qreal halfPenW = 0.5 * _myData->pen.widthF();
    _temp.append(point - QPointF(halfPenW,halfPenW));
    if(_temp.length() == 4){
        // 曲线圆滑

        const QPointF *a = &_temp.at(0);
        const QPointF *b = &_temp.at(1);
        const QPointF *c = &_temp.at(2);
        const QPointF *d = &_temp.at(3);

        static int cutting = 0;
        static float t, tt, ttt, tt2, tt3, ttt2 = 0;

        if(_distances[1] < 0){
            _distances[0] = penSqrt(
                                    std::powf(b->x()-a->x(),2) +
                                    std::powf(b->y()-a->y(),2)
                                    );
        }
        else{
            _distances[0] = _distances[1];
        }

        if(_distances[2] < 0){
            _distances[1] = penSqrt(
                        std::powf(b->x()-c->x(),2) +
                        std::powf(b->y()-c->y(),2)
                        );
        }else{
            _distances[1] = _distances[2];
        }
        _distances[2] = penSqrt(
                    std::powf(d->x()-c->x(),2) +
                    std::powf(d->y()-c->y(),2)
                    );

        cutting = _distances[1] / NSB_BLACKBOARD_PEN_SMOOTHING_UNIT;

        if (cutting > 0 && _distances[0] * 2 > 0.1f * (_distances[1] + _distances[2])) {

            for (int i = 0; i < cutting; ++i) {
                t = (float)i / (cutting);
                tt = t*t;
                ttt = tt*t;
                tt2 = tt*2;
                tt3 = tt*3;
                ttt2 = ttt*2;
                QPointF p = (*b) * (ttt2 - tt3 + 1.0f) +
                        (*c) * (-ttt2 + tt3 ) +
                        0.5f * ((*c)-(*a)) * (ttt - tt2 + t) +
                        0.5f * ((*d)-(*b))  * (ttt - tt);

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
    if(_path){
        delete _path;
    }
    _path = new QPainterPath();
    qreal halfPenW = 0.5 * _myData->pen.widthF();
    for(int i = 0; i < _myData->coords.length(); i+=2)
    {
        qreal x = _myData->coords[i];
        qreal y = _myData->coords[i+1];
        QPointF point(x,y);
        if(i==0)
        {
            setPos(point + QPointF(halfPenW,halfPenW));
        }
        qreal oldLeft = pos().x();
        qreal oldTop = pos().y();
        qreal newLeft = std::min(point.x(), oldLeft);
        qreal newTop = std::min(point.y(), oldTop);
        _path->translate(oldLeft-newLeft,oldTop-newTop);
        _path->lineTo(point-QPointF(newLeft, newTop));
         setPos(newLeft, newTop);
    }
    _rect = _path->boundingRect();
    _rect.setX(_rect.x()-halfPenW);
    _rect.setY(_rect.y()-halfPenW);
    _rect.setWidth(_rect.width()+halfPenW);
    _rect.setHeight(_rect.height()+halfPenW);
    setRect(_rect);
    if(_myData->isPositionValid())
    {
        _myData->updatePrevPostion();
        qreal x = _myData->x;
        qreal y = _myData->y;
        setPos(x,y);
    }
    setZValue(_myData->z);
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
        _straightLineFrom = QPointF(-999999, -999999);
        _straightLineTo = QPointF(-999999, -999999);
    }
    else if(_straightLineTo.x() > -999998)
    {
        _mousePos = _straightLineTo + pos();
        _changed.clear();
        addPointToPath(_mousePos);
    }
    _straight = straight;
}

QPointF BbItemPen::straightTo()
{
    return _straightLineTo + pos();
}

void BbItemPen::writeStream(QDataStream &stream)
{
    _myData->updatePostion(this);
    _myData->z = zValue();
    _myData->writeStream(stream);
}

void BbItemPen::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    toAbsoluteCoords();
    repaint();
}

QString BbItemPen::id() const
{
    return _myData->lid;
}

void BbItemPen::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemPen::toolType() const
{
    return _myData->tooltype;
}

BbScene *BbItemPen::scene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemPen::data()
{
    return _myData;
}

Blackboard *BbItemPen::blackboard()
{
    return scene()->blackboard();
}

void BbItemPen::toolDown(const QPointF &pos)
{
    if(scene()->modifiers() & Qt::ShiftModifier)
    {
        setStraight(true);
    }
    setZValue(QDateTime::currentMSecsSinceEpoch());
    auto settings = blackboard()->toolSettings<BbItemPenData>(BBTT_Pen);
    setWeight(settings->weight());
    setColor(settings->pen.color());
    penDown(pos);
    setId(scene()->generatItemId());
    setStraight(scene()->onlyShiftDown());
    scene()->setCurrentItem(this);
    emit blackboard()->itemChanged(BBIET_penDown,this);
}

void BbItemPen::toolDraw(const QPointF &pos)
{
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

void BbItemPen::toolDone(const QPointF &pos)
{
    Q_UNUSED(pos);
    if(straight())
    {
        setStraight(false);
        emit blackboard()->itemChanged(BBIET_penDraw,this);
    }
    done();
    emit blackboard()->itemChanged(BBIET_penDone,this);
    scene()->unsetCurrentItem(this);
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

qreal BbItemPen::z()
{
    return zValue();
}

void BbItemPen::setZ(const qreal &value)
{
    setZValue(value);
    _myData->z = value;
}

void BbItemPen::toAbsoluteCoords()
{
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        _myData->mode = BbItemData::CM_ABSOLUTE;
        qreal ratio = scene()->width() / 100;
        if(_myData->isPositionValid())
        {
            _myData->x *= ratio;
            _myData->y *= ratio;
        }
        if(_myData->isPrevPositionValid())
        {
            _myData->prevX *= ratio;
            _myData->prevY *= ratio;
        }
        for(int i=0;i<_myData->coords.length();++i)
        {
            _myData->coords[i] = _myData->coords[i] * ratio;
        }
    }
}

bool BbItemPen::isEditing()
{
    return _editing;
}

