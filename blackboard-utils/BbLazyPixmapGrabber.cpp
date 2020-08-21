#include "BbLazyPixmapGrabber.h"

#include <QDateTime>

BbLazyPixmapGrabber::BbLazyPixmapGrabber(QObject *parent) :
    QObject(parent),
    _timer(new QTimer(this))
{
    _timer->setInterval(1);
    _timer->setSingleShot(true);
    connect(_timer,&QTimer::timeout,this,&BbLazyPixmapGrabber::grab);
}

BbLazyPixmapGrabber::~BbLazyPixmapGrabber()
{
    _pixmaps.clear();
}

void BbLazyPixmapGrabber::setAreas(QList<QRectF> areas)
{
    _areas = areas;
}

QList<QRectF> BbLazyPixmapGrabber::areas()
{
    return _areas;
}

void BbLazyPixmapGrabber::setBlackboard(QPointer<Blackboard> blackboard)
{
    _blackboard = blackboard;
}

QPointer<Blackboard> BbLazyPixmapGrabber::blackboard()
{
    return _blackboard;
}

void BbLazyPixmapGrabber::start()
{
    (std::sort)(_areas.begin(),_areas.end(),[](const QRectF &a, const QRectF &b){
        return a.top() < b.top();
    });
    _timer->stop();
    _pixmaps.clear();

    if(!_blackboard || _areas.empty()){
        emit done(_pixmaps);
        return;
    }

    _timer->start();
}

void BbLazyPixmapGrabber::grab()
{
    if(_areas.isEmpty()){
        _timer->stop();
        emit done(_pixmaps);
    }else{
        auto rect = _areas.takeFirst();
        _pixmaps << areaToPixmap(rect);
        _timer->start();
        emit progress(_pixmaps.size(), _pixmaps.size() + _areas.size());
    }
}

QPixmap* BbLazyPixmapGrabber::areaToPixmap(QRectF srcArea)
{
    auto &areas = _areas;
    auto &blackboard = _blackboard;
    if(!blackboard)
        return nullptr;

    int srcWidth = int(srcArea.width());
    int srcHeight = int(srcArea.height());
    int dstWidth = 2 * srcWidth;
    int dstHeight = 2 * srcHeight;
    auto pm = new QPixmap(dstWidth,dstHeight);
    pm->fill(blackboard->palette().window().color());
    QPainter painter(pm);
    blackboard->scene()->render(&painter,pm->rect(),srcArea);
    return (pm);
}
