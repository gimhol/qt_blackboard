#include "BbItemTailData.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


static qreal minWidth = 1.1;
static qreal maxWidth = 30;
static QPen defaultPen = QPen(QColor(100,100,180), minWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
static bool defaultCubic = true;
static int defaultFadeOutDuration = 500;
static int defaultFadeOutDelay = 500;

const qreal & BbItemTailData::getMinWidth()
{
    return minWidth;
}
const qreal & BbItemTailData::getMaxWidth()
{
    return maxWidth;
}
const QPen & BbItemTailData::getDefaultPen()
{
    return defaultPen;
}

const bool &BbItemTailData::getDefaultCubic()
{
    return defaultCubic;
}

const int &BbItemTailData::getDefaultFadeOutDuration()
{
    return defaultFadeOutDuration;
}

const int &BbItemTailData::getDefaultFadeOutDelay()
{
    return defaultFadeOutDelay;
}
void BbItemTailData::setMinWidth(const qreal &value)
{
    minWidth = value;
}
void BbItemTailData::setMaxWidth(const qreal &value)
{
    maxWidth = value;
}

void BbItemTailData::setDefaultPen(const QPen &value)
{
    defaultPen = value;
}
void BbItemTailData::setDefaultCubic(const bool &value)
{
    defaultCubic = value;
}

void BbItemTailData::setDefaultFadeOutDuration(const int &value)
{
    defaultFadeOutDuration = value;
}

void BbItemTailData::setDefaultFadeOutDelay(const int &value)
{
    defaultFadeOutDelay = value;
}

BbItemTailData::BbItemTailData(CoordMode mode):
    BbItemData(mode)
{
    tooltype = BBTT_Tail;
    needPen = true;
    persistence = false;
    pen = defaultPen;
    cubic = defaultCubic;
    fadeOutDuration = defaultFadeOutDuration;
    fadeOutDelay = defaultFadeOutDelay;
}

void BbItemTailData::setColor(const QColor &color)
{
    pen.setColor(color);
}

void BbItemTailData::setWeight(qreal weight)
{
    pen.setWidthF(minWidth + weight * (maxWidth - minWidth));
}

qreal BbItemTailData::weight()
{
    return (pen.widthF() - minWidth) / (maxWidth - minWidth);
}
