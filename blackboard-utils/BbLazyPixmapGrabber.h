#ifndef BBLAZYPIXMAPGRABBER_H
#define BBLAZYPIXMAPGRABBER_H

#include <BbUtilsHeader.h>
#include <Blackboard.h>
#include <QPointer>
#include <QTimer>

class NSB_BLACKBOARD_UTILS_EXPORT BbLazyPixmapGrabber : public QObject
{
    Q_OBJECT
    QPointer<QTimer> _timer;
    QList<QPixmap*> _pixmaps;
    QList<QRectF> _areas;
    QPointer<Blackboard> _blackboard;
public:
    explicit BbLazyPixmapGrabber(QObject *parent = nullptr);

    ~BbLazyPixmapGrabber();

    void setAreas(QList<QRectF> areas);

    QList<QRectF> areas();

    void setBlackboard(QPointer<Blackboard> blackboard);

    QPointer<Blackboard> blackboard();

    void start();

    void grab();

    QPixmap* areaToPixmap(QRectF srcArea);

signals:
    void progress(int current, int total);
    void done(QList<QPixmap*> pms);
};

#endif // BBLAZYPIXMAPGRABBER_H
