#include "BbPixmapSaver.h"

#include <QDebug>

BbPixmapSaver::BbPixmapSaver(QHash<QString,QPixmap*> pms):
    QThread(),
    _pixmaps(pms)
{

}

BbPixmapSaver::~BbPixmapSaver()
{
    qDebug() << "ok";
}

void BbPixmapSaver::run()
{
    auto current = 0;
    auto total = _pixmaps.count();
    for(auto itr = _pixmaps.begin(); itr != _pixmaps.end(); ++itr){
        itr.value()->save(itr.key());
        delete itr.value();
        ++current;
        emit progress(current, total);
    }
    _pixmaps.clear();
}
