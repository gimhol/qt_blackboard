#include "BbItemRectData.h"

static QColor defaultPenColor = QColor(100,100,180);

static qreal minPenWidth = 1.1;

static qreal maxPenWidth = 30;

static QPen defaultPen = QPen(defaultPenColor,minPenWidth,
                              Qt::SolidLine,
                              Qt::SquareCap,
                              Qt::MiterJoin);

static QColor defaultBrushColor = Qt::transparent;

static QBrush defaultBrush = defaultBrushColor;

const QColor &BbItemRectData::getDefaultPenColor()
{
    return defaultPenColor;
}

const qreal &BbItemRectData::getMinPenWidth()
{
    return minPenWidth;
}

const qreal &BbItemRectData::getMaxPenWidth()
{
    return maxPenWidth;
}

const QPen &BbItemRectData::getDefaultPen()
{
    return defaultPen;
}

const QColor &BbItemRectData::getDefaultBrushColor()
{
    return defaultBrushColor;
}

const QBrush &BbItemRectData::getDefaultBrush()
{
    return defaultBrush;
}

void BbItemRectData::setDefaultPenColor(const QColor &value)
{
    defaultPenColor = value;
}

void BbItemRectData::setMinPenWidth(const qreal &value)
{
    minPenWidth = value;
}

void BbItemRectData::setMaxPenWidth(const qreal &value)
{
    maxPenWidth = value;
}

void BbItemRectData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

void BbItemRectData::setDefaultBrushColor(const QColor &value)
{
    defaultBrushColor = value;
}

void BbItemRectData::setDefaultBrush(const QBrush &value)
{
    defaultBrush = value;
}

BbItemRectData::BbItemRectData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Rectangle;
    needPen = true;
    needBrush = true;
    needSize = true;
    needPrivateData = false;
    pen = defaultPen;
    brush = defaultBrush;
}

qreal BbItemRectData::weight()
{
    return (pen.widthF() - minPenWidth)/(maxPenWidth - minPenWidth);
}

void BbItemRectData::setWeight(qreal weight)
{
    pen.setWidthF(minPenWidth + weight * (maxPenWidth - minPenWidth));
}

void BbItemRectData::fromJsonObject(const QJsonObject &jobj)
{
    BbItemData::fromJsonObject(jobj);
    empty = isSizeNull();
}
