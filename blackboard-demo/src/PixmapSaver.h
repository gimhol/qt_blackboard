#ifndef PIXMAPSAVINGTHREAD_H
#define PIXMAPSAVINGTHREAD_H

#include <QHash>
#include <QPixmap>
#include <QThread>

class PixmapSaver: public QThread{
    Q_OBJECT
public:
    qlonglong duration;
    QHash<QString,QPixmap*> pixmaps;

    PixmapSaver(QHash<QString,QPixmap*> pms);
    ~PixmapSaver();
signals:
    void progress(QString path,int duration,int current,int total);
    // QThread interface
protected:
    void run();
};

#endif // PIXMAPSAVINGTHREAD_H
