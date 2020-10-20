#include "BbItemDeleter.h"
#include <QApplication>

BbItemDeleter::BbItemDeleter(QObject *parent)
{
    timer.setSingleShot(true);
    connect(&timer,&QTimer::timeout,this,&BbItemDeleter::doit);
}

BbItemDeleter *BbItemDeleter::get()
{
    static BbItemDeleter *ret = nullptr;
    if(!ret)
        ret = new BbItemDeleter(qApp);
    return ret;
}

void BbItemDeleter::remove(IItemIndex *item)
{
    auto itr = indices.find(item);
    if(itr != indices.end())
        indices.erase(itr);
}

void BbItemDeleter::remove(QGraphicsItem *item)
{
    auto itr = items.find(item);
    if(itr != items.end())
        items.erase(itr);
}

void BbItemDeleter::addItem(IItemIndex *item)
{
    auto itr = indices.find(item);
    if(itr != indices.end())
        return;

    indices[item] = false;
    timer.start(500);
}

void BbItemDeleter::addItem(QGraphicsItem *item)
{
    auto itr = items.find(item);
    if(itr != items.end())
        return;

    items[item] = false;
    timer.start(500);
}

void BbItemDeleter::doit()
{
    for(auto item: indices)
        delete item;
    for(auto item: items)
        delete item;

    indices.clear();
    items.clear();
}
