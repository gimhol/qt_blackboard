#ifndef BBPIXMAPSAVINGTHREAD_H
#define BBPIXMAPSAVINGTHREAD_H

#include <BbUtilsHeader.h>
#include <QHash>
#include <QPixmap>
#include <QThread>

class NSB_BLACKBOARD_UTILS_EXPORT BbPixmapSaver: public QThread{
    Q_OBJECT

    QHash<QString,QPixmap*> _pixmaps;

public:
    BbPixmapSaver(QHash<QString,QPixmap*> pms);

    ~BbPixmapSaver();

signals:
    void progress(int current,int total);

protected:
    void run();
};

#endif // BBPIXMAPSAVINGTHREAD_H
