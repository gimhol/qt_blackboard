#include "BbItemTriangleData.h"


static QColor defaultPenColor = QColor(100,100,180);

static qreal minPenWidth = 1.1;

static qreal maxPenWidth = 30;

static QPen defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

static QColor defaultBrushColor = Qt::transparent;

static QBrush defaultBrush = defaultBrushColor;

const QColor &BbItemTriangleData::getDefaultPenColor()
{
    return defaultPenColor;
}

const qreal &BbItemTriangleData::getMinPenWidth()
{
    return minPenWidth;
}

const qreal &BbItemTriangleData::getMaxPenWidth()
{
    return maxPenWidth;
}

const QPen &BbItemTriangleData::getDefaultPen()
{
    return defaultPen;
}

const QColor &BbItemTriangleData::getDefaultBrushColor()
{
    return defaultBrushColor;
}

const QBrush &BbItemTriangleData::getDefaultBrush()
{
    return defaultBrush;
}

void BbItemTriangleData::setDefaultPenColor(const QColor &value)
{
    defaultPenColor = value;
}

void BbItemTriangleData::setMinPenWidth(const qreal &value)
{
    minPenWidth = value;
}

void BbItemTriangleData::setMaxPenWidth(const qreal &value)
{
    maxPenWidth = value;
}

void BbItemTriangleData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

void BbItemTriangleData::setDefaultBrushColor(const QColor &value)
{
    defaultBrushColor = value;
}

void BbItemTriangleData::setDefaultBrush(const QBrush &value)
{
    defaultBrush = value;
}

BbItemTriangleData::BbItemTriangleData(CoordMode mode):
    BbItemData(mode),
    pen(defaultPen),
    brush(defaultBrush)
{
    tooltype = BBTT_Triangle;
}

qreal BbItemTriangleData::weight()
{
    return (pen.widthF() - minPenWidth)/(maxPenWidth - minPenWidth);
}

void BbItemTriangleData::setWeight(qreal weight)
{
    pen.setWidthF(minPenWidth + weight * (maxPenWidth - minPenWidth));
}

void BbItemTriangleData::writeStream(QDataStream &stream)
{
    BbItemData::writeStream(stream);

    QJsonObject jobj;
    jobj["pen_width"]   = pen.widthF();
    jobj["pen_color"]   = int(pen.color().rgba());
    jobj["pen_style"]   = int(pen.style());
    jobj["brush_color"] = int(brush.color().rgba());
    jobj["brush_style"] = int(brush.style());
    QJsonArray jarr;
    for(int i = 0; i < 3; ++i){
        jarr << points[i].x() << points[i].y();
    }
    jobj["coords"] = jarr;
    stream << QJsonDocument(jobj).toBinaryData();
}

void BbItemTriangleData::readStream(QDataStream &stream)
{
    BbItemData::readStream(stream);
    QByteArray data;
    stream >> data;
    auto jobj = QJsonDocument::fromBinaryData(data).object();
    pen.setWidthF(jobj["pen_width"].toDouble());
    pen.setColor(QColor::fromRgba(QRgb(jobj["pen_color"].toInt())));
    pen.setStyle(Qt::PenStyle(jobj["pen_style"].toInt()));
    brush.setColor(QColor::fromRgba(QRgb(jobj["brush_color"].toInt())));
    brush.setStyle(Qt::BrushStyle(jobj["brush_style"].toInt()));
    auto jarr = jobj["coords"].toArray();
    empty = jarr.size() < 6;
    if(!empty){
        for(auto i = 0; i < 3; ++i){
            points[i].setX(jarr[i*2].toDouble());
            points[i].setY(jarr[i*2+1].toDouble());
        }
    }
}
