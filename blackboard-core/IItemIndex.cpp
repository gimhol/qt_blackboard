#include "BbItemDeleter.h"
#include "BbScene.h"
#include "Blackboard.h"
#include "IItemIndex.h"
#include "BbItemDeleter.h"
#include <QGraphicsItem>
#include "BbItemInnerDataKey.h"

IItemIndex::~IItemIndex(){
    BbItemDeleter::get()->remove(this);
//    if(last)
//        last->next = next;
//    if(next)
//        next->last = last;
}

QString IItemIndex::id(){
    auto d = data();
    if(d)
        return d->lid;
    return "";
}

void IItemIndex::setId(const QString &id){
    auto d = data();
    if(d)
        d->lid = id;
    auto i = item();
    if(i)
       i->setData(BBIIDK_ITEM_ID,id);
}

BbToolType IItemIndex::toolType(){
    auto d = data();
    if(d)
        return d->tooltype;
    return BBTT_None;
}

QGraphicsItem *IItemIndex::item(){
    return dynamic_cast<QGraphicsItem*>(this);
}

Blackboard *IItemIndex::blackboard(){
    auto s = bbScene();
    if(s){
        auto vs = s->views();
        for(auto v :vs){
            auto b = dynamic_cast<Blackboard*>(v);
            if(b)
                return b;
        }
    }
    return nullptr;
}

BbScene *IItemIndex::bbScene(){
    auto i = item();
    if(i)
        return dynamic_cast<BbScene*>(i->scene());
    return nullptr;
}

qreal IItemIndex::z() {
    auto i = item();
    if(i){
        return i->zValue();
    }else{
        auto d = data();
        if(d)
            return d->z;
    }
    return 0;
}

void IItemIndex::setZ(const qreal &value) {
    auto i = item();
    if(i)
        i->setZValue(value);
    auto d = data();
    if(d)
        d->z = value;
}

void IItemIndex::updatePrevZ(){
    auto d = data();
    if(d)
        d->prevZ = d->z;
}

void IItemIndex::moveByVector2(const QPointF &offset)
{
    moveToPosition(position()+offset);
}

void IItemIndex::moveByVector2(qreal x, qreal y)
{
    moveToPosition(position()+QPointF(x,y));
}

void IItemIndex::moveToPosition(const QPointF &pos){
    auto i = item();
    if(i)
        i->setPos(pos);
    auto d = data();
    if(d){
        d->x = pos.x();
        d->y = pos.y();
    }
}

void IItemIndex::moveToPosition(qreal x, qreal y){
    auto i = item();
    if(i)
        i->setPos(x,y);
    auto d = data();
    if(d){
        d->x = x;
        d->y = y;
    }
}

void IItemIndex::moveToX(qreal x){
    auto i = item();
     if(i)
         i->setX(x);
     auto d = data();
     if(d)
         d->x = x;
}

void IItemIndex::moveToY(qreal y){
    auto i = item();
     if(i)
         i->setY(y);
     auto d = data();
     if(d)
         d->y = y;
}

QPointF IItemIndex::position(){
    auto i = item();
    if(i)
        return i->pos();
    auto d = data();
    if(d)
        return QPointF(d->x,d->y);
    return QPointF(0,0);
}

qreal IItemIndex::positionX()
{
    return position().x();
}

qreal IItemIndex::positionY()
{
    return position().y();
}

void IItemIndex::updatePrevPosition(){
    auto d = data();
    if(d)
        d->updatePrevPostion();
}
