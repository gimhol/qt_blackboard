#include "BbScene.h"
#include "Blackboard.h"
#include "IItemIndex.h"

#include <QGraphicsItem>

IItemIndex::~IItemIndex(){
    if(last)
        last->next = next;

    if(next)
        next->last = last;
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
        if(d){
            return d->z;
        }
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

void IItemIndex::updatePrevZ()
{
    auto d = data();
    if(d){
        d->prevZ = d->z;
    }
}

void IItemIndex::updatePrevPosition(){
    auto d = data();
    if(d)
        d->updatePrevPostion();
}
