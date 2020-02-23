#include "BlackboardConnector.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>

BlackboardConnector::BlackboardConnector(Blackboard *blackboard):
    QObject(blackboard),
    _bb(blackboard),
    _me(new BlackboardClient(this))
{
    // local.
    connect(_bb,&Blackboard::scrolled,this,&BlackboardConnector::onLocalBlackboardScrolled);
    connect(_bb,&Blackboard::pointerShown,this,&BlackboardConnector::onLocalPointerShown);
    connect(_bb,&Blackboard::pointerMoving,this,&BlackboardConnector::onLocalPointerMoving);
    connect(_bb,&Blackboard::pointerMoved,this,&BlackboardConnector::onLocalPointerMoved);
    connect(_bb,&Blackboard::pointerHidden,this,&BlackboardConnector::onLocalPointerHidden);
    connect(_bb,&Blackboard::cursorShown,this,&BlackboardConnector::onLocalPointerShown);
    connect(_bb,&Blackboard::cursorMoving,this,&BlackboardConnector::onLocalPointerMoving);
    connect(_bb,&Blackboard::cursorMoved,this,&BlackboardConnector::onLocalPointerMoved);
    connect(_bb,&Blackboard::cursorHidden,this,&BlackboardConnector::onLocalPointerHidden);

    connect(_bb,&Blackboard::itemChanged,this,&BlackboardConnector::onLocalItemChanged);
    connect(_bb,&Blackboard::multipleItemChanged,this,&BlackboardConnector::onLocalMultipleItemChanged);

    // remote
    connect(_me,&BlackboardClient::msgRead,this,&BlackboardConnector::onMeMsgRead);
}

bool BlackboardConnector::isConnected()
{
    return _me->isConnected();
}

void BlackboardConnector::connectToServer(const QString &hostName, quint16 port)
{
    _me->connectToHost(hostName,port);
}

void BlackboardConnector::disconnectFromServer()
{
    _me->disconnectFromHost();
}

void BlackboardConnector::onLocalBlackboardScrolled(float x, float y)
{
    QJsonObject jobj;
    jobj["x"] = qreal(x)/_bb->canvasWidth();
    jobj["y"] = qreal(y)/_bb->canvasWidth();
    _me->send(MsgTypeBlackboardScrolled,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalPointerShown(QPoint localPoint){
    QJsonObject jobj;
    jobj["id"] = "remote_pointer";
    jobj["x"] = localPoint.x();
    jobj["y"] = localPoint.y();
    _me->send(MsgTypeBlackboardPointerShown,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalPointerMoving(QPoint localPoint){
    QJsonObject jobj;
    jobj["id"] = "remote_pointer";
    jobj["x"] = localPoint.x();
    jobj["y"] = localPoint.y();
    _me->send(MsgTypeBlackboardPointerMoving,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalPointerMoved(QPoint localPoint){
    QJsonObject jobj;
    jobj["id"] = "remote_pointer";
    jobj["x"] = localPoint.x();
    jobj["y"] = localPoint.y();
    _me->send(MsgTypeBlackboardPointerMoved,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalPointerHidden(QPoint){
    QJsonObject jobj;
    jobj["id"] = "remote_pointer";
    _me->send(MsgTypeBlackboardPointerHidden,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalMultipleItemChanged(BBItemEventType eventType, IItemIndex *index){
    switch(eventType)
    {
    case BBIET_itemMoving:
        onLocalItemsMoving(index);
        break;
    case BBIET_itemMoved:
        onLocalItemsMoved(index);
        break;
    case BBIET_itemDelete:
        onLocalItemsDelete(index);
        break;
    case BBIET_itemPaste:
        // do it better.
        break;
    default:
        break;
    }
}

void BlackboardConnector::onLocalItemsMoving(IItemIndex *index){
    QJsonArray jarr;
    while(index){
        QJsonObject jobj;
        jobj["id"] = index->id();
        jobj["x"] = index->positionX()/_bb->canvasWidth();
        jobj["y"] = index->positionY()/_bb->canvasWidth();
        jarr << jobj;
        index = index->next;
    }
    _me->send(BBIET_itemMoving,QJsonDocument(jarr).toBinaryData());
}

void BlackboardConnector::onLocalItemsMoved(IItemIndex *index)
{
    QJsonArray jarr;
    while(index){
        QJsonObject jobj;
        jobj["id"] = index->id();
        jobj["x"] = index->positionX()/_bb->canvasWidth();
        jobj["y"] = index->positionY()/_bb->canvasWidth();
        jarr << jobj;
        index = index->next;
    }
    _me->send(BBIET_itemMoved,QJsonDocument(jarr).toBinaryData());
}

void BlackboardConnector::onLocalItemsDelete(IItemIndex *index)
{
    QJsonArray jarr;
    while(index){
        jarr << index->id();
        index = index->next;
    }
    _me->send(BBIET_itemDelete,QJsonDocument(jarr).toBinaryData());
}

void BlackboardConnector::onLocalItemChanged(BBItemEventType eventType, IItemIndex *index)
{
    switch(eventType){
    case BBIET_penDown:         onLocalPenDown(index); break;
    case BBIET_penDraw:         onLocalPenDraw(index); break;
    case BBIET_penStraighting:  onLocalPenStraighting(index); break;
    case BBIET_penDone:         onLocalPenDone(index); break;
    case BBIET_textAdded:   onLocalTextAdded(index); break;
    case BBIET_textChanged: onLocalTextChanged(index); break;
    case BBIET_textDone:    onLocalTextDone(index); break;
    case BBIET_straightDown: onLocalStraightDown(index); break;
    case BBIET_straightDraw: onLocalStraightDraw(index); break;
    case BBIET_straightDone: onLocalStraightDone(index); break;
    case BBIET_rectDown: onLocalRectDown(index); break;
    case BBIET_rectDraw: onLocalRectDraw(index); break;
    case BBIET_rectDone: onLocalRectDone(index); break;
    case BBIET_ellipseDown: onLocalEllipseDown(index); break;
    case BBIET_ellipseDraw: onLocalEllipseDraw(index); break;
    case BBIET_ellipseDone: onLocalEllipseDone(index); break;
    case BBIET_triangleDown: onLocalTriangleDown(index); break;
    case BBIET_triangleDraw: onLocalTriangleDraw(index); break;
    case BBIET_triangleDone: onLocalTriangleDone(index); break;
    default:
        break;
    }
}

void BlackboardConnector::onLocalPenDown(IItemIndex *index){
    auto item = dynamic_cast<BbItemPen*>(index);
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["z"] = item->z();
    jobj["color"] = int(item->data()->pen.color().rgba());
    jobj["weight"] = item->data()->weight();
    auto dot = item->changed()->first();
    jobj["x"] = qreal(dot.x())/_bb->canvasWidth();
    jobj["y"] = qreal(dot.y())/_bb->canvasWidth();
    _me->send(BBIET_penDown,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalPenDraw(IItemIndex *index){
    auto pen = dynamic_cast<BbItemPen*>(index);
    QJsonObject jobj;
    jobj["id"] = pen->id();
    QJsonArray dots;
    for(auto p: *pen->changed()){
        dots << (p.x()/_bb->canvasWidth())
             << (p.y()/_bb->canvasWidth());
    }
    jobj["dots"] = dots;
    _me->send(BBIET_penDraw,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalPenStraighting(IItemIndex *index){
    auto pen = dynamic_cast<BbItemPen*>(index);
    QJsonObject jobj;
    jobj["id"] = pen->id();
    jobj["x"] = qreal(pen->straightTo().x())/_bb->canvasWidth();
    jobj["y"] = qreal(pen->straightTo().y())/_bb->canvasWidth();
    _me->send(BBIET_penStraighting,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalPenDone(IItemIndex *index){
    QJsonObject jobj;
    jobj["id"] = index->id();
    _me->send(BBIET_penDone,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalTextAdded(IItemIndex *index){
    auto item = dynamic_cast<BbItemText*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["z"] = item->z();
    jobj["font"] = item->font().toString();
    jobj["weight"] = item->weight();
    jobj["color"] = int(item->color().rgba());
    jobj["x"] = item->positionX()/_bb->canvasWidth();
    jobj["y"] = item->positionY()/_bb->canvasWidth();
    _me->send(BBIET_textAdded,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalTextChanged(IItemIndex *index){
    auto item = dynamic_cast<BbItemText*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["text"] = item->text();
    _me->send(BBIET_textChanged,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalTextDone(IItemIndex *index){
    auto item = dynamic_cast<BbItemText*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["text"] = item->text();
    _me->send(BBIET_textDone,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalStraightDown(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemStraight*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["z"] = item->z();
    jobj["color"] = int(item->data()->pen.color().rgba());
    jobj["weight"] = item->data()->weight();
    jobj["ax"] = item->a().x()/_bb->canvasWidth();
    jobj["ay"] = item->a().y()/_bb->canvasWidth();
    _me->send(BBIET_straightDown,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalStraightDraw(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemStraight*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["bx"] = item->b().x()/_bb->canvasWidth();
    jobj["by"] = item->b().y()/_bb->canvasWidth();
    _me->send(BBIET_straightDraw,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalStraightDone(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemStraight*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    _me->send(BBIET_straightDone,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalRectDown(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemRect*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["penColor"] = int(item->data()->pen.color().rgba());
    auto a = item->data()->brush.color();
    qInfo() << a;
    jobj["brushColor"] = int(item->data()->brush.color().rgba());
    jobj["weight"] = item->data()->weight();
    jobj["z"] = item->z();
    jobj["x"] = item->data()->x/_bb->canvasWidth();
    jobj["y"] = item->data()->y/_bb->canvasWidth();
    jobj["w"] = item->data()->size.width()/_bb->canvasWidth();
    jobj["h"] = item->data()->size.height()/_bb->canvasWidth();
    _me->send(BBIET_rectDown,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalRectDraw(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemRect*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["x"] = item->data()->x/_bb->canvasWidth();
    jobj["y"] = item->data()->y/_bb->canvasWidth();
    jobj["w"] = item->data()->size.width()/_bb->canvasWidth();
    jobj["h"] = item->data()->size.height()/_bb->canvasWidth();
    _me->send(BBIET_rectDraw,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalRectDone(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemRect*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    _me->send(BBIET_rectDone,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalEllipseDown(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemEllipse*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["penColor"] = int(item->data()->pen.color().rgba());
    jobj["brushColor"] = int(item->data()->brush.color().rgba());
    jobj["weight"] = item->data()->weight();
    jobj["z"] = item->z();
    jobj["x"] = item->data()->x/_bb->canvasWidth();
    jobj["y"] = item->data()->y/_bb->canvasWidth();
    jobj["w"] = item->data()->size.width()/_bb->canvasWidth();
    jobj["h"] = item->data()->size.height()/_bb->canvasWidth();
    _me->send(BBIET_ellipseDown,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalEllipseDraw(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemEllipse*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["x"] = item->data()->x/_bb->canvasWidth();
    jobj["y"] = item->data()->y/_bb->canvasWidth();
    jobj["w"] = item->data()->size.width()/_bb->canvasWidth();
    jobj["h"] = item->data()->size.height()/_bb->canvasWidth();
    _me->send(BBIET_ellipseDraw,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalEllipseDone(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemEllipse*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    _me->send(BBIET_ellipseDone,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalTriangleDown(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemTriangle*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["penColor"] = int(item->data()->pen.color().rgba());
    jobj["brushColor"] = int(item->data()->brush.color().rgba());
    jobj["weight"] = item->data()->weight();
    jobj["z"] = item->z();
    jobj["ax"] = item->point(0).x()/_bb->canvasWidth();
    jobj["ay"] = item->point(0).y()/_bb->canvasWidth();
    _me->send(BBIET_triangleDown,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalTriangleDraw(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemTriangle*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    jobj["bx"] = item->point(1+item->step()).x()/_bb->canvasWidth();
    jobj["by"] = item->point(1+item->step()).y()/_bb->canvasWidth();
    _me->send(BBIET_triangleDraw,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onLocalTriangleDone(IItemIndex *index)
{
    auto item = dynamic_cast<BbItemTriangle*>(index);
    if(!item)
        return;
    QJsonObject jobj;
    jobj["id"] = item->id();
    _me->send(BBIET_triangleDone,QJsonDocument(jobj).toBinaryData());
}

void BlackboardConnector::onMeMsgRead()
{
    switch(_me->msgType()){
    case MsgTypeBlackboardScrolled:
        onRemoteBlackboardScrolled();
        break;
    case MsgTypeBlackboardPointerShown:
        onRemotePointerShown();
        break;
    case MsgTypeBlackboardPointerMoving:
        onRemotePointerMoving();
        break;
    case MsgTypeBlackboardPointerMoved:
        onRemotePointerMoved();
        break;
    case MsgTypeBlackboardPointerHidden:
        onRemotePointerHidden();
        break;
    default:
        break;
    }

    switch(static_cast<BBItemEventType>(_me->msgType())){
    case BBIET_itemMoving:  onRemoteItemsMoving(); break;
    case BBIET_itemMoved:   onRemoteItemsMoved(); break;
    case BBIET_itemDelete:  onRemoteItemsDelete(); break;
    case BBIET_itemPaste:
        // do it better.
        break;
    case BBIET_penDown:         onRemotePenDown(); break;
    case BBIET_penDraw:         onRemotePenDraw(); break;
    case BBIET_penStraighting:  onRemotePenStraighting(); break;
    case BBIET_penDone:         onRemotePenDone(); break;
    case BBIET_textAdded:       onRemoteTextAdded(); break;
    case BBIET_textChanged:     onRemoteTextChanged(); break;
    case BBIET_textDone:        onRemoteTextDone(); break;
    case BBIET_straightDown: onRemoteStraightDown(); break;
    case BBIET_straightDraw: onRemoteStraightDraw(); break;
    case BBIET_straightDone: onRemoteStraightDone(); break;
    case BBIET_rectDown: onRemoteRectDown(); break;
    case BBIET_rectDraw: onRemoteRectDraw(); break;
    case BBIET_rectDone: onRemoteRectDone(); break;
    case BBIET_ellipseDown: onRemoteEllipseDown(); break;
    case BBIET_ellipseDraw: onRemoteEllipseDraw(); break;
    case BBIET_ellipseDone: onRemoteEllipseDone(); break;
    case BBIET_triangleDown: onRemoteTriangleDown(); break;
    case BBIET_triangleDraw: onRemoteTriangleDraw(); break;
    case BBIET_triangleDone: onRemoteTriangleDone(); break;
    default:
        break;
    }
}

void BlackboardConnector::onRemoteBlackboardScrolled(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    _bb->setScroll(
            jobj["x"].toDouble() * _bb->canvasWidth(),
            jobj["y"].toDouble() * _bb->canvasWidth());
}

void BlackboardConnector::onRemotePointerShown(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    _bb->movePointer(jobj["id"].toString(),jobj["x"].toInt(),jobj["y"].toInt());
}

void BlackboardConnector::onRemotePointerMoving(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    _bb->movePointer(jobj["id"].toString(),jobj["x"].toInt(),jobj["y"].toInt());
}

void BlackboardConnector::onRemotePointerMoved(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    _bb->movePointer(jobj["id"].toString(),jobj["x"].toInt(),jobj["y"].toInt());
}

void BlackboardConnector::onRemotePointerHidden(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    _bb->hidePointer(jobj["id"].toString());
}

void BlackboardConnector::onRemoteItemsMoving()
{
    auto jarr = QJsonDocument::fromBinaryData(_me->msgBody()).array();
    for(auto jval: jarr){
        auto jobj = jval.toObject();
        auto item = _bb->find<IItemIndex>(jobj["id"].toString());
        if(item){
            item->moveToPosition(
                    jobj["x"].toDouble() * _bb->canvasWidth(),
                    jobj["y"].toDouble() * _bb->canvasWidth());
        }
    }
}

void BlackboardConnector::onRemoteItemsMoved()
{
    auto jarr = QJsonDocument::fromBinaryData(_me->msgBody()).array();
    for(auto jval: jarr){
        auto jobj = jval.toObject();
        auto item = _bb->find<IItemIndex>(jobj["id"].toString());
        if(item){
            item->moveToPosition(
                    jobj["x"].toDouble() * _bb->canvasWidth(),
                    jobj["y"].toDouble() * _bb->canvasWidth());
        }
    }
}

void BlackboardConnector::onRemoteItemsDelete()
{
    auto jarr = QJsonDocument::fromBinaryData(_me->msgBody()).array();
    for(auto jval: jarr){
        _bb->remove(jval.toString());
    }
}

void BlackboardConnector::onRemotePenDown(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    BbItemPen *item = new BbItemPen();
    _bb->add(item);
    item->setId(jobj["id"].toString());
    item->setZ(jobj["z"].toDouble());
    item->data()->pen.setColor(QColor::fromRgba(QRgb(jobj["color"].toInt())));
    item->data()->setWeight(jobj["weight"].toDouble());
    QPointF dot(jobj["x"].toDouble()*_bb->canvasWidth(),
                jobj["y"].toDouble()*_bb->canvasWidth());
    item->penDown(dot);
}

void BlackboardConnector::onRemotePenDraw(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    BbItemPen *item = _bb->find<BbItemPen>(jobj["id"].toString());
    if(!item)
        return;
    auto dots = jobj["dots"].toArray();
    for(auto itr = dots.begin(); itr!=dots.end(); ++itr){

        QPointF dot;
        dot.setX(itr->toDouble()*_bb->canvasWidth());
        ++itr;
        dot.setY(itr->toDouble()*_bb->canvasWidth());
        item->penDraw(dot);
    }
}

void BlackboardConnector::onRemotePenStraighting(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    BbItemPen *item = _bb->find<BbItemPen>(jobj["id"].toString());
    _bb->add(item);
    QPointF dot(jobj["x"].toDouble()*_bb->canvasWidth(),
            jobj["y"].toDouble()*_bb->canvasWidth());
    item->penStraighting(dot);
}

void BlackboardConnector::onRemotePenDone(){
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    BbItemPen *item = _bb->find<BbItemPen>(jobj["id"].toString());
    if(item)
        item->done();
}

void BlackboardConnector::onRemoteTextAdded()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = new BbItemText();
    _bb->scene()->add(copy);
    copy->setZ(copy->z());
    QFont f;
    f.fromString(jobj["font"].toString());
    copy->setFont(f);
    copy->setWeight(jobj["weight"].toDouble());
    copy->setColor(QColor::fromRgba(QRgb(jobj["color"].toInt())));
    copy->moveToPosition(
            jobj["x"].toDouble()*_bb->canvasWidth(),
            jobj["y"].toDouble()*_bb->canvasWidth());
    copy->setId(jobj["id"].toString());
}

void BlackboardConnector::onRemoteTextChanged()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = _bb->find<BbItemText>(jobj["id"].toString());
    if(!copy)
        return;
    copy->setText(jobj["text"].toString());
}

void BlackboardConnector::onRemoteTextDone()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = _bb->find<BbItemText>(jobj["id"].toString());
    if(!copy)
        return;
    copy->setText(jobj["text"].toString());
}

void BlackboardConnector::onRemoteStraightDown()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = new BbItemStraight();
    _bb->add(copy);
    copy->setId(jobj["id"].toString());
    copy->setZ(jobj["z"].toDouble());
    copy->data()->pen.setColor(QColor::fromRgba(QRgb(jobj["color"].toInt())));
    copy->data()->setWeight(jobj["weight"].toDouble());
    QPointF dot(jobj["ax"].toDouble()*_bb->canvasWidth(),
                jobj["ay"].toDouble()*_bb->canvasWidth());
    copy->begin(dot);
}

void BlackboardConnector::onRemoteStraightDraw()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = _bb->find<BbItemStraight>(jobj["id"].toString());
    if(!copy)
        return;
    QPointF dot(jobj["bx"].toDouble()*_bb->canvasWidth(),
                jobj["by"].toDouble()*_bb->canvasWidth());
    copy->draw(dot);
}

void BlackboardConnector::onRemoteStraightDone()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = _bb->find<BbItemStraight>(jobj["id"].toString());
    if(!copy)
        return;
    copy->done();
}

void BlackboardConnector::onRemoteRectDown()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = new BbItemRect();
    _bb->add(copy);
    copy->setId(jobj["id"].toString());
    copy->setZ(jobj["z"].toDouble());
    copy->data()->setWeight(jobj["weight"].toDouble());
    copy->data()->pen.setColor(QColor::fromRgba(QRgb(jobj["penColor"].toInt())));
    copy->data()->brush.setColor(QColor::fromRgba(QRgb(jobj["brushColor"].toInt())));
    QPointF a(jobj["x"].toDouble()*_bb->canvasWidth(),
              jobj["y"].toDouble()*_bb->canvasWidth());
    copy->begin(a);
}

void BlackboardConnector::onRemoteRectDraw()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = _bb->find<BbItemRect>(jobj["id"].toString());
    if(!copy)
        return;
    QPointF a(jobj["x"].toDouble()*_bb->canvasWidth(),
              jobj["y"].toDouble()*_bb->canvasWidth());
    copy->begin(a);
    QPointF b(jobj["w"].toDouble()*_bb->canvasWidth(),
              jobj["h"].toDouble()*_bb->canvasWidth());
    copy->draw(a+b);
}

void BlackboardConnector::onRemoteRectDone()
{    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
     auto copy = _bb->find<BbItemRect>(jobj["id"].toString());
     if(!copy)
         return;
     copy->done();
}

void BlackboardConnector::onRemoteEllipseDown()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = new BbItemEllipse();
    _bb->add(copy);
    copy->setId(jobj["id"].toString());
    copy->setZ(jobj["z"].toDouble());
    copy->data()->setWeight(jobj["weight"].toDouble());
    copy->data()->pen.setColor(QColor::fromRgba(QRgb(jobj["penColor"].toInt())));
    copy->data()->brush.setColor(QColor::fromRgba(QRgb(jobj["brushColor"].toInt())));
    QPointF a(jobj["x"].toDouble()*_bb->canvasWidth(),
              jobj["y"].toDouble()*_bb->canvasWidth());
    copy->begin(a);
}

void BlackboardConnector::onRemoteEllipseDraw()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = _bb->find<BbItemEllipse>(jobj["id"].toString());
    if(!copy)
        return;
    QPointF a(jobj["x"].toDouble()*_bb->canvasWidth(),
              jobj["y"].toDouble()*_bb->canvasWidth());
    copy->begin(a);
    QPointF b(jobj["w"].toDouble()*_bb->canvasWidth(),
              jobj["h"].toDouble()*_bb->canvasWidth());
    copy->draw(a+b);
}

void BlackboardConnector::onRemoteEllipseDone()
{    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
     auto copy = _bb->find<BbItemEllipse>(jobj["id"].toString());
     if(!copy)
         return;
     copy->done();
}

void BlackboardConnector::onRemoteTriangleDown()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = new BbItemTriangle();
    _bb->add(copy);
    copy->setId(jobj["id"].toString());
    copy->setZ(jobj["z"].toDouble());
    copy->data()->setWeight(jobj["weight"].toDouble());
    copy->data()->pen.setColor(QColor::fromRgba(QRgb(jobj["penColor"].toInt())));
    copy->data()->brush.setColor(QColor::fromRgba(QRgb(jobj["brushColor"].toInt())));
    QPointF dot(jobj["ax"].toDouble()*_bb->canvasWidth(),
                jobj["ay"].toDouble()*_bb->canvasWidth());
    copy->begin(dot);
}

void BlackboardConnector::onRemoteTriangleDraw()
{
    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
    auto copy = _bb->find<BbItemTriangle>(jobj["id"].toString());
    if(!copy)
        return;
    QPointF dot(jobj["bx"].toDouble()*_bb->canvasWidth(),
                jobj["by"].toDouble()*_bb->canvasWidth());
    copy->draw(dot);
}

void BlackboardConnector::onRemoteTriangleDone()
{    auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
     auto copy = _bb->find<BbItemTriangle>(jobj["id"].toString());
     if(!copy)
         return;
     copy->done();
}
