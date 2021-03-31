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

bool BbItemData::isPositionValid() const
{
    return x > -9998 && y > -9998;
}

bool BbItemData::isPrevPositionValid() const
{
    return prevX > -9998 && prevY > -9998;
}

bool BbItemData::isSizeValid() const
{
    return width >=0 && height >=0;
}

bool BbItemData::isPrevSizeValid() const
{
    return prevWidth >=0 && prevHeight >=0;
}

bool BbItemData::isSizeNull() const { return width == 0 && height == 0; }

bool BbItemData::isSizeEmpty() const { return width < 1 || height < 1; }

bool BbItemData::isPrevSizeNull() const { return prevWidth == 0 && prevHeight == 0; }

bool BbItemData::isPrevSizeEmpty() const { return prevWidth < 1 || prevHeight < 1; }

void BbItemData::fixPostion(const qreal &x, const qreal &y)
{
    this->x = x;
    this->y = y;
    prevX = x;
    prevY = y;
}

void BbItemData::updatePostion(const qreal &x, const qreal &y)
{
    this->x = x;
    this->y = y;
    if(!isPrevPositionValid())
    {
        updatePrevPostion();
    }
}

void BbItemData::fixPostion(IItemIndex *index)
{
    auto item = index->item();
    if(!item)
        return;
    auto scene = index->bbScene();
    if(scene && CM_PERCENTAGE == mode)
    {
        qreal ratio = scene->width() / 100;
        x = item->x() / ratio;
        y = item->y() / ratio;
    }
    else
    {
        x = item->x();
        y = item->y();
    }
    prevX = x;
    prevY = y;
}

void BbItemData::updatePostion(IItemIndex *index)
{
    auto item = index->item();
    if(!item)
        return;
    auto scene = index->bbScene();
    if(scene && CM_PERCENTAGE == mode)
    {
        qreal ratio = scene->width() / 100;
        x = item->x() / ratio;
        y = item->y() / ratio;
    }
    else
    {
        x = item->x();
        y = item->y();
    }
    if(!isPrevPositionValid())
    {
        updatePrevPostion();
    }
}

void BbItemData::updatePrevPostion()
{
    prevX = x;
    prevY = y;
}

void BbItemData::updatePrevSize()
{
    prevWidth = width;
    prevHeight = height;
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
        jobj["width"] = width;
        jobj["height"] = height;
        jobj["prev_width"] = prevWidth;
        jobj["prev_height"] = prevHeight;
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

    width = jobj["width"].toDouble();
    height = jobj["height"].toDouble();
    prevWidth = jobj["prev_width"].toDouble();
    prevHeight = jobj["prev_height"].toDouble();
}
