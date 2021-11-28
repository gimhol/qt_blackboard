#include "BbItemEllipseData.h"

static QColor defaultPenColor = QColor(100,100,180);

static qreal minPenWidth = 1.1;

static qreal maxPenWidth = 30;

static QPen defaultPen = QPen(defaultPenColor,minPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

static QColor defaultBrushColor = Qt::transparent;

static QBrush defaultBrush = defaultBrushColor;

const QColor &BbItemEllipseData::getDefaultPenColor()
{
    return defaultPenColor;
}

const qreal &BbItemEllipseData::getMinPenWidth()
{
    return minPenWidth;
}

const qreal &BbItemEllipseData::getMaxPenWidth()
{
    return maxPenWidth;
}

const QPen &BbItemEllipseData::getDefaultPen()
{
    return defaultPen;
}

const QColor &BbItemEllipseData::getDefaultBrushColor()
{
    return defaultBrushColor;
}

const QBrush &BbItemEllipseData::getDefaultBrush()
{
    return defaultBrush;
}

void BbItemEllipseData::setDefaultPenColor(const QColor &value)
{
    defaultPenColor = value;
}

void BbItemEllipseData::setMinPenWidth(const qreal &value)
{
    minPenWidth = value;
}

void BbItemEllipseData::setMaxPenWidth(const qreal &value)
{
    maxPenWidth = value;
}

void BbItemEllipseData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}

void BbItemEllipseData::setDefaultBrushColor(const QColor &value)
{
    defaultBrushColor = value;
}

void BbItemEllipseData::setDefaultBrush(const QBrush &value)
{
    defaultBrush = value;
}
BbItemEllipseData::BbItemEllipseData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Ellipse;
    needPen = true;
    needBrush = true;
    needSize = true;
    needPrivateData = false;
    pen = defaultPen;
    brush = defaultBrush;
}

qreal BbItemEllipseData::weight()
{
    return (pen.widthF() - minPenWidth)/(maxPenWidth - minPenWidth);
}

void BbItemEllipseData::setWeight(qreal weight)
{
    pen.setWidthF(minPenWidth + weight * (maxPenWidth - minPenWidth));
}

void BbItemEllipseData::fromJsonObject(const QJsonObject &jobj)
{
    BbItemData::fromJsonObject(jobj);
    empty = isSizeNull();
}
