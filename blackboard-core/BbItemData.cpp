#include "BbItemData.h"
#include "IItemIndex.h"
#include "BbScene.h"
#include <QDebug>

static auto strokeCapStyle = [](const QJsonValue &jVal){
    switch(jVal.toInt()){
    case 1:     return Qt::FlatCap;
    case 2:     return Qt::SquareCap;
    default:    return Qt::RoundCap;
    }
};
static auto jstrokeCapStyle = [](const Qt::PenCapStyle &val){
    switch(val){
    case Qt::FlatCap:   return 1;
    case Qt::SquareCap: return 2;
    default:            return 0;
    }
};
static auto strokeStyle = [](const QJsonValue &jVal){
    switch(jVal.toInt()){
    case 1:     return Qt::SolidLine;
    case 2:     return Qt::DashLine;
    case 3:     return Qt::DotLine;
    case 4:     return Qt::DashDotLine;
    case 5:     return Qt::DashDotDotLine;
    case 6:     return Qt::CustomDashLine;
    default:    return Qt::NoPen;
    }
};
static auto jstrokeStyle = [](const Qt::PenStyle &val){
    switch(val){
    case Qt::SolidLine:         return 1;
    case Qt::DashLine:          return 2;
    case Qt::DotLine:           return 3;
    case Qt::DashDotLine:       return 4;
    case Qt::DashDotDotLine:    return 5;
    case Qt::CustomDashLine:    return 6;
    default:                    return 0;
    }
};
static auto strokeJoinStyle = [](const QJsonValue &jVal){
    switch(jVal.toInt()){
    case 1:     return Qt::MiterJoin;
    case 2:     return Qt::BevelJoin;
    default:    return Qt::RoundJoin;
    }
};
static auto jstrokeJoinStyle = [](const Qt::PenJoinStyle &val){
    switch(val){
    case Qt::MiterJoin:     return 1;
    case Qt::BevelJoin:     return 2;
    default:                return 0;
    }
};
static auto color = [](const QJsonValue &color){
    return QColor::fromRgba(color.toInt());
};
static auto jcolor = [](const QColor &color){
    return int(color.rgba());
};
static auto readStrokeFromTemplate = [](const QString &templateName, BbItemData &target){
    // TODO: -Gim
};
static auto readStrokeFromJObject = [](const QJsonObject &jstroke, BbItemData &target){
    target.pen.setWidthF(jstroke["width"].toDouble());
    target.pen.setColor(color(jstroke["color"]));
    target.pen.setJoinStyle(strokeJoinStyle(jstroke["join"]));
    target.pen.setCapStyle(strokeCapStyle(jstroke["cap"]));
    target.pen.setStyle(strokeStyle(jstroke["style"]));
};

static auto strokeToJObject = [](const QPen &pen){
    QJsonObject jpen;
    jpen["width"] = pen.widthF();
    jpen["color"] = jcolor(pen.color());
    jpen["style"] = jstrokeStyle(pen.style());
    jpen["join"] = jstrokeJoinStyle(pen.joinStyle());
    jpen["cap"] = jstrokeCapStyle(pen.capStyle());
    return jpen;
};



static auto brushStyle = [](const QJsonValue &jVal){
    switch(jVal.toInt()){
    case 1:     return Qt::SolidPattern;
    default:    return Qt::NoBrush;
    }
};
static auto jbrushStyle = [](const Qt::BrushStyle &val){
    switch(val){
    case Qt::SolidPattern: return 1;
    default:               return 0;
    }
};
static auto readBrushFromTemplate = [](const QString &templateName, BbItemData &target){
    // TODO: -Gim
};
static auto readBrushFromJObject = [](const QJsonObject &jbrush, BbItemData &target){
    target.brush.setColor(color(jbrush["color"]));
    target.brush.setStyle(brushStyle(jbrush["style"]));
};
static auto brushToJObject = [](const QBrush &brush){
    QJsonObject jbrush;
    jbrush["color"] = jcolor(brush.color());
    jbrush["style"] = jbrushStyle(brush.style());
    return jbrush;
};


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

QJsonObject BbItemData::toJsonObject()
{
    QJsonObject jobj;
    jobj["id"] = lid;
    jobj["type"] = tooltype;

    /******** write 'props' ********/
    QJsonObject jprops;
    jprops["x"] = x;
    jprops["y"] = y;
    jprops["z"] = z;
    if(needSize){
        jprops["w"] = width;
        jprops["h"] = height;
    }
    jobj["props"] = jprops;

    /******** write 'style' ********/
    if( needPen || needBrush ){
        QJsonObject jstyles;
        if(needPen)
            jstyles["stroke"] = strokeToJObject(pen);
        if(needBrush)
            jstyles["brush"] = brushToJObject(brush);

        jobj["style"] = jstyles;
    }
    if(needPrivateData)
        jobj["data"] = privateData();

    return jobj;
}

void BbItemData::fromJsonObject(const QJsonObject &jobj)
{
    if(jobj["type"] != tooltype)
        qWarning() << "unexpected type! wanted: " << tooltype << "got: " << jobj["type"];

    lid   = jobj["id"].toString();

    /******** read 'props' ********/
    auto jprops = jobj["props"].toObject();
    x       = jprops["x"].toDouble();
    y       = jprops["y"].toDouble();
    z       = jprops["z"].toDouble();
    prevX = x;
    prevY = y;
    prevZ = z;

    width   = jprops["w"].toDouble();
    height  = jprops["h"].toDouble();
    prevWidth = width;
    prevHeight = height;

    do { /******** read 'style' ********/
        auto j = jobj["style"];
        if(!j.isObject())
            break;
        auto jstyle = j.toObject();

        do { /******** read 'style.stroke' ********/
            auto j = jstyle["stroke"];
            if(j.isObject()){
                readStrokeFromJObject(j.toObject(), *this);
            } else if(j.isString()){
                readStrokeFromTemplate(j.toString(), *this);
            } else if(j.isArray()){
                for(auto i: j.toArray()){
                    if(i.isObject()){
                        readStrokeFromJObject(i.toObject(), *this);
                    } else if(i.isString()){
                        readStrokeFromTemplate(i.toString(), *this);
                    }
                }
            }
        } while(0);

        do { /******** read 'style.brush' ********/
            auto j = jstyle["brush"];
            if(j.isObject()){
                readBrushFromJObject(j.toObject(), *this);
            } else if(j.isString()){
                readBrushFromTemplate(j.toString(), *this);
            } else if(j.isArray()){
                for(auto i: j.toArray()){
                    if(i.isObject()){
                        readBrushFromJObject(i.toObject(), *this);
                    } else if(i.isString()){
                        readBrushFromTemplate(i.toString(), *this);
                    }
                }
            }
        } while(0);
    } while(0);


    /******** read 'data' ********/
    auto jdata = jobj["data"].toObject();
    readPrivateData(jdata);
}
