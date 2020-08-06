#include "PixmapSaver.h"

#include <QDebug>
#include <QDateTime>

PixmapSaver::PixmapSaver(QHash<QString,QPixmap*> pms):
    QThread(),
    pixmaps(pms)
{

}

PixmapSaver::~PixmapSaver()
{
    qDebug() << "ok";
}

void PixmapSaver::run()
{
    auto b = QDateTime::currentMSecsSinceEpoch();
    auto current = 0;
    auto total = pixmaps.count();
    QTime t;
    t.start();
    for(auto itr = pixmaps.begin(); itr != pixmaps.end(); ++itr){
        auto time0 = QDateTime::currentMSecsSinceEpoch();
        itr.value()->save(itr.key());
        delete itr.value();
        ++current;
        emit progress(itr.key(), t.restart(), current, total);
    }
    pixmaps.clear();
    duration = QDateTime::currentMSecsSinceEpoch() - b;
}
