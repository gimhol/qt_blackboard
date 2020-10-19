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

void BbItemDeleter::addItem(IItemIndex *item)
{
    indices << item;
    timer.start(500);
}

void BbItemDeleter::addItem(QGraphicsItem *item)
{
    items << item;
    timer.start(500);
}

void BbItemDeleter::doit()
{
    for(auto item: indices)
        delete item;
    for(auto item: items)
        delete item;
    indices.clear();
}
