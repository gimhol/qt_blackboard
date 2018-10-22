#include "BbItemPen.h"
#include "BlackboardScene.h"
#include "Blackboard.h"
#include "BbItemPenData.h"

#if LINE_SMOOTHING
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

BbItemPen::BbItemPen():QGraphicsRectItem()
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
#if LINE_SMOOTHING
    _distances[0] = -1;
    _distances[1] = -1;
    _distances[2] = -1;
#endif
    _myData = new BbItemPenData();
}

BbItemPen::BbItemPen(BbItemPenData *penData):QGraphicsRectItem()
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
#if LINE_SMOOTHING
    _distances[0] = -1;
    _distances[1] = -1;
    _distances[2] = -1;
#endif
    _myData = penData;
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

#if SAVE_TO_PIXMAP_WHEN_DONE
    if(_pixmap){
        delete _pixmap;
        _pixmap = nullptr;
    }
#endif
}

QList<QPointF> *BbItemPen::changed(){ return &_changed ;}

bool BbItemPen::isEmpty() { return _myData->isEmpty; }

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
        if(_path->length() < 5000)
        {
            painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        }
        else if(_path->length() < 10000)
        {
            painter->setRenderHint(QPainter::Antialiasing, true);
        }
        painter->drawPath(*_path);
        if(_straight && _straightLineFrom.x() > -999998)
        {
            painter->drawLine(_straightLineFrom, _straightLineTo);
        }
    }
#if SAVE_TO_PIXMAP_WHEN_DONE
    if(_pixmap != nullptr){
        int width = rect().width();
        int height = rect().height();
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter->drawPixmap(0,0,width,height,*_pixmap);
    }
#endif
}

void BbItemPen::penDown(const QPointF &point){
    setPos(point);

    _myData->isEmpty = false;

    if(_path == nullptr){
        _path = new QPainterPath();
    }
    auto halfPenW = 0.5 * _myData->pen.widthF();

    _path->moveTo(0,0);
    _mousePos = point;
    addPointToPath(point);

    if(_straight)
    {
        _straightLineFrom = QPointF(0,0);
    }
    update();
}

void BbItemPen::penDraw(const QPointF &point)
{
    setStraight(false);

    _changed.clear();

    _mousePos = point;

#if LINE_SMOOTHING
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

void BbItemPen::done(){

#if SAVE_TO_PIXMAP_WHEN_DONE

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

void BbItemPen::setColor(const QColor &color){
    _myData->pen.setColor(color);
}

qreal BbItemPen::penWidth()
{
    return _myData->pen.widthF();
}

qreal BbItemPen::weight()
{
    return (_myData->pen.widthF() - BbItemPenData::minWidth)  / (BbItemPenData::maxWidth - BbItemPenData::minWidth);
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
    if(_myData->mode == BbItemPenData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _straightLineTo.setX(point.x() / ratio - pos().x());
        _straightLineTo.setY(point.y() / ratio - pos().y());
    }
    else
    {
        _straightLineTo.setX(point.x() - pos().x());
        _straightLineTo.setY(point.y() - pos().y());
    }
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
    _rect.setWidth(w + halfPenW);
    _rect.setHeight(h + halfPenW);

    update();

    qDebug() << _rect;
}

void BbItemPen::addPointToPath(const QPointF &point)
{
    if(_path == nullptr)
    {
        return;
    }
    _changed.append(point);

    if(_myData->mode == BbItemPenData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _myData->coords.append(point.x() / ratio);
        _myData->coords.append(point.y() / ratio);
    }
    else
    {
        _myData->coords.append(point.x());
        _myData->coords.append(point.y());
    }

    qreal halfPenW = 0.5 * _myData->pen.widthF();
    qreal oldLeft = pos().x();
    qreal oldTop = pos().y();
    qreal newLeft = std::min(point.x(), oldLeft);
    qreal newTop = std::min(point.y(), oldTop);

    _path->translate(oldLeft-newLeft,oldTop-newTop);    // 重新计算左上角位置
    _path->lineTo(point - QPointF(newLeft, newTop));

     setPos(newLeft, newTop);

     if(_myData->mode == BbItemPenData::CM_PERCENTAGE)
     {
         qreal ratio = scene()->width() / 100;
         _myData->x = newLeft / ratio;
         _myData->y = newTop / ratio;
     }
     else
     {
         _myData->x = newLeft;
         _myData->y = newTop;
     }
    _rect = _path->boundingRect();
    _rect.setX(_rect.x()-halfPenW);
    _rect.setY(_rect.y()-halfPenW);
    _rect.setWidth(_rect.width()+halfPenW);
    _rect.setHeight(_rect.height()+halfPenW);
}

#if LINE_SMOOTHING
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

        cutting = _distances[1] / LINE_SMOOTHING_UNIT;

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

void BbItemPen::repaintWithItemData()
{
    if(_path){
        delete _path;
    }
    qreal halfPenW = 0.5 * _myData->pen.widthF();
    if(_path == nullptr){
        _path = new QPainterPath();
    }

    for(int i = 0; i < _myData->coords.length(); i+=2)
    {
        qreal x = _myData->coords[i];
        qreal y = _myData->coords[i+1];
        if(_myData->mode == BbItemPenData::CM_PERCENTAGE)
        {
            qreal ratio = scene()->width() / 100;
            x *= ratio;
            y *= ratio;
        }
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
//    _path->translate(halfPenW,halfPenW);
    _rect = _path->boundingRect();
    _rect.setX(_rect.x()-halfPenW);
    _rect.setY(_rect.y()-halfPenW);
    _rect.setWidth(_rect.width()+halfPenW);
    _rect.setHeight(_rect.height()+halfPenW);

    setRect(_rect);

    done();

    if( _myData->x > -9998 && _myData->y > -9998 ){
        qreal x = _myData->x;
        qreal y = _myData->y;
        if(_myData->mode == BbItemPenData::CM_PERCENTAGE)
        {
            qreal width = scene()->width();
            qreal ratio = width / 100;
            x *= ratio;
            y *= ratio;
        }
        setPos(x,y);
    }
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
    else
    {
        if(_straightLineTo.x() > -999998)
        {
            _mousePos = _straightLineTo + pos();
            _changed.clear();
            addPointToPath(_mousePos);
        }
    }
    _straight = straight;
}

QPointF BbItemPen::straightTo()
{
    return _straightLineTo + pos();
}

void BbItemPen::writeStream(QDataStream &stream)
{
    _myData->x = x();
    _myData->y = y();
    if(_myData->mode == BbItemPenData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _myData->x /= ratio;
        _myData->y /= ratio;
    }
    _myData->writeStream(stream);
}

void BbItemPen::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    repaintWithItemData();
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

BlackboardScene *BbItemPen::scene()
{
    return dynamic_cast<BlackboardScene *>(QGraphicsRectItem::scene());
}

