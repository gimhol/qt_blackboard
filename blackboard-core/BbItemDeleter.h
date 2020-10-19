#ifndef BBITEMDELETER_H
#define BBITEMDELETER_H

#include <QObject>
#include <QTimer>
#include <BbHeader.h>
#include <IItemIndex.h>
#include <QGraphicsItem>

/**
 * Note:
 *      大量的delete item然后同时释放Scene会非常容易触发一个奔溃，故通过此类避免。
 *      -Gim
 */
class BbItemDeleter: public QObject {
    Q_OBJECT
protected:
    explicit BbItemDeleter(QObject *parent);
public:
    static BbItemDeleter *get();
    QTimer timer;
    QList<IItemIndex *> indices;
    QList<QGraphicsItem *> items;
    void addItem(IItemIndex *item);
    void addItem(QGraphicsItem *item);
    void doit();
};


#endif // BBITEMDELETER_H
