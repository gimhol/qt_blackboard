#include "BbItemData.h"
#include "IItemIndex.h"
#include "BbScene.h"
#include <QDebug>

BbItemData::BbItemData(CoordMode mode):
    mode(mode)
{

}

BbItemData::~BbItemData()
{

}

bool BbItemData::isPositionValid()
{
    return x > -9998 && y > -9998;
}

bool BbItemData::isPrevPositionValid()
{
    return prevX > -9998 && prevY > -9998;
}

void BbItemData::updatePostion(IItemIndex *itemIdx)
{
    auto itemBase = dynamic_cast<QGraphicsItem*>(itemIdx);
    if(itemBase)
    {
        auto scene = itemIdx->bbScene();
        if(scene && CM_PERCENTAGE == mode)
        {
            qreal ratio = scene->width() / 100;
            x = itemBase->x() / ratio;
            y = itemBase->y() / ratio;
        }
        else
        {
            x = itemBase->x();
            y = itemBase->y();
        }
        if(!isPrevPositionValid())
        {
            updatePrevPostion();
        }
    }
}

void BbItemData::updatePrevPostion()
{
    prevX = x;
    prevY = y;
}

void BbItemData::updatePrevSize()
{
    prevSize = size;
}

void BbItemData::writeStream(QDataStream &stream){

    stream << QJsonDocument(toJsonObject()).toBinaryData();
}

void BbItemData::readStream(QDataStream &stream){
    QByteArray data;
    stream >> data;
    auto jobj = QJsonDocument::fromBinaryData(data).object();
    fromJsonObject(jobj);
}

QJsonObject BbItemData::toJsonObject()
{
    QJsonObject jobj;
    jobj["coord_mode"] = mode;
    jobj["type"] = tooltype;
    jobj["id"] = lid;
    jobj["x"] = x;
    jobj["y"] = y;
    jobj["z"] = z;
    jobj["prev_x"] = prevX;
    jobj["prev_y"] = prevY;
    jobj["prev_z"] = prevZ;
    if(needPen){
        QJsonObject jpen;
        jpen["width"] = pen.widthF();
        jpen["color"] = int(pen.color().rgba());
        jpen["style"] = int(pen.style());
        jpen["join_style"] = int(pen.joinStyle());
        jpen["cap_style"] = int(pen.capStyle());
        jobj["pen"] = jpen;
    }
    if(needBrush){
        QJsonObject jbrush;
        jbrush["color"] = int(brush.color().rgba());
        jbrush["style"] = int(brush.style());
        jobj["brush"] = jbrush;
    }
    if(needSize){
        jobj["width"] = size.width();
        jobj["height"] = size.height();
        jobj["prev_width"] = prevSize.width();
        jobj["prev_height"] = prevSize.height();
    }
    return jobj;
}

void BbItemData::fromJsonObject(const QJsonObject &jobj)
{
    mode  = CoordMode(jobj["coord_mode"].toInt());
    if(jobj["type"] != tooltype){
        qWarning() << "unexpected type! wanted: " << tooltype << "got: " << jobj["type"];
    }
    lid   = jobj["id"].toString();
    x     = jobj["x"].toDouble();
    y     = jobj["y"].toDouble();
    z     = jobj["z"].toDouble();
    prevX = jobj["prev_x"].toDouble();
    prevY = jobj["prev_y"].toDouble();
    prevZ = jobj["prev_z"].toDouble();

    auto jpen = jobj["pen"].toObject();
    pen.setWidthF(jpen["width"].toDouble());
    pen.setColor(QColor::fromRgba(QRgb(jpen["color"].toInt())));
    pen.setJoinStyle(Qt::PenJoinStyle(jpen["join_style"].toInt()));
    pen.setCapStyle(Qt::PenCapStyle(jpen["cap_style"].toInt()));
    pen.setStyle(Qt::PenStyle(jpen["style"].toInt()));

    auto jbrush = jobj["brush"].toObject();
    brush.setColor(QColor::fromRgba(QRgb(jbrush["color"].toInt())));
    brush.setStyle(Qt::BrushStyle(jbrush["style"].toInt()));

    size.setWidth(jobj["width"].toDouble());
    size.setHeight(jobj["height"].toDouble());
    prevSize.setWidth(jobj["prev_width"].toDouble());
    prevSize.setHeight(jobj["prev_height"].toDouble());
}
