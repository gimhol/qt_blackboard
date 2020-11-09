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
public:
    explicit BbItemDeleter(QObject *parent = nullptr);
    static BbItemDeleter *get();
    void remove(IItemIndex *item);
    void remove(QGraphicsItem *item);
    void addItem(IItemIndex *item);
    void addItem(QGraphicsItem *item);
    void doit();
private:
    QTimer timer;
    QMap<int, IItemIndex *> indices;
    QMap<int, QGraphicsItem *> items;
    QList<int> deadIndices;
    QList<int> deadItems;
    bool itemDeleting = false;
};


#endif // BBITEMDELETER_H
