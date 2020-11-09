#include "BbItemDeleter.h"
#include <QApplication>

Q_GLOBAL_STATIC(BbItemDeleter,bbItemDeleterInstance);

BbItemDeleter::BbItemDeleter(QObject *parent)
{
    timer.setSingleShot(true);
    connect(&timer,&QTimer::timeout,this,&BbItemDeleter::doit);
}

BbItemDeleter *BbItemDeleter::get()
{
    return bbItemDeleterInstance();
}

void BbItemDeleter::remove(IItemIndex *item)
{
    if(itemDeleting){
        deadIndices << int(item);
        return;
    }
    auto itr = indices.find(int(item));
    if(itr != indices.end())
        indices.erase(itr);
}

void BbItemDeleter::remove(QGraphicsItem *item)
{
    if(itemDeleting){
        deadItems << int(item);
        return;
    }
    auto itr = items.find(int(item));
    if(itr != items.end())
        items.erase(itr);
}

void BbItemDeleter::addItem(IItemIndex *item)
{
    auto itr = indices.find(int(item));
    if(itr != indices.end())
        return;

    indices[int(item)] = item;
    timer.start(500);
}

void BbItemDeleter::addItem(QGraphicsItem *item)
{
    auto itr = items.find(int(item));
    if(itr != items.end())
        return;

    items[int(item)] = item;
    timer.start(500);
}

void BbItemDeleter::doit()
{
    itemDeleting = true;
    QList<int> deadIndices;
    QList<int> deadItems;
    for(auto item: deadIndices)
        indices.remove(item);
    for(auto item: deadItems)
        items.remove(item);
    deadIndices.clear();
    deadItems.clear();
    for(auto item: indices)
        delete item;
    for(auto item: items)
        delete item;
    indices.clear();
    items.clear();
    itemDeleting = false;
}
