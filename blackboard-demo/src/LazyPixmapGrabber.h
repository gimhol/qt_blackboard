#ifndef LAZYPIXMAPGRABBER_H
#define LAZYPIXMAPGRABBER_H

#include <Blackboard.h>
#include <QObject>
#include <QPointer>
#include <QTimer>

class LazyPixmapGrabber : public QObject
{
    Q_OBJECT
    QPointer<QTimer> timer;
    QList<QPixmap*> pms;
public:
    QList<QRectF> areas;

    QPointer<Blackboard> blackboard;

    explicit LazyPixmapGrabber(QObject *parent = nullptr);

    ~LazyPixmapGrabber();

    void start();

    void grab();

    QPixmap* areaToPixmap(QRectF srcArea);
signals:
    void progress(qlonglong duration,int current, int total);
    void done(QList<QPixmap*> pms);
};

#endif // LAZYPIXMAPGRABBER_H
