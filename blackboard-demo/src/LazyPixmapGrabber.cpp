#include "LazyPixmapGrabber.h"

#include <QDateTime>

LazyPixmapGrabber::LazyPixmapGrabber(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    timer->setInterval(1);
    timer->setSingleShot(true);
    connect(timer,&QTimer::timeout,this,&LazyPixmapGrabber::grab);
}

LazyPixmapGrabber::~LazyPixmapGrabber()
{
    pms.clear();
}

void LazyPixmapGrabber::start()
{
    (std::sort)(areas.begin(),areas.end(),[](const QRectF &a, const QRectF &b){
        return a.top() < b.top();
    });
    timer->stop();
    pms.clear();

    if(!blackboard || areas.empty()){
        emit done(pms);
        return;
    }

    timer->start();
}

void LazyPixmapGrabber::grab()
{
    if(areas.isEmpty()){
        timer->stop();
        emit done(pms);
    }else{
        auto a = QDateTime::currentMSecsSinceEpoch();
        auto rect = areas.takeFirst();
        pms << areaToPixmap(rect);
        timer->start();
        auto duration = QDateTime::currentMSecsSinceEpoch() - a;
        emit progress(duration, pms.size(), pms.size() + areas.size());
    }
}

QPixmap* LazyPixmapGrabber::areaToPixmap(QRectF srcArea)
{
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
