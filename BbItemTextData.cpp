#include "BbItemTextData.h"

#include <QDebug>

static qreal minPointSize = 9;
static qreal maxPointSize = 72;
static QFont defalutFont = QFont(QStringLiteral("宋体"), minPointSize);
static QColor defalutColor = QColor(100,100,180);

const qreal &BbItemTextData::getMinPointSize()
{
    return minPointSize;
}

const qreal &BbItemTextData::getMaxPointSize()
{
    return maxPointSize;
}

const QFont &BbItemTextData::getDefalutFont()
{
    return defalutFont;
}

const QColor &BbItemTextData::getDefalutColor()
{
    return defalutColor;
}

void BbItemTextData::setMinPointSize(const qreal &value)
{
    minPointSize = value;
}

void BbItemTextData::setMaxPointSize(const qreal &value)
{
    maxPointSize = value;
}

void BbItemTextData::setDefalutFont(const QFont &value)
{
    defalutFont = value;
}

void BbItemTextData::setDefalutColor(const QColor &value)
{
    defalutColor = value;
}

BbItemTextData::BbItemTextData(BbItemData::CoordMode mode):
    BbItemData(mode),
    font(defalutFont),
    color(defalutColor)
{
    tooltype = BBTT_Text;
}

void BbItemTextData::setPointWeight(qreal weight)
{
    qreal pointSize = minPointSize + weight * (maxPointSize - minPointSize);
    font.setPointSizeF(pointSize);
}

qreal BbItemTextData::pointWeight()
{
    return (font.pointSizeF()-minPointSize)/(maxPointSize-minPointSize);
}

QJsonObject BbItemTextData::toJsonObject()
{
    auto jobj = BbItemData::toJsonObject();
    jobj["family"]      = font.family();
    jobj["point_size"]  = font.pointSizeF();
    jobj["weight"]      = font.weight();
    jobj["italic"]      = font.italic();
    jobj["bold"]        = font.bold();
    jobj["color"]       = int(color.rgba());
    jobj["text"]        = text;
    jobj["prev_text"]   = prevText;
    return jobj;
}

void BbItemTextData::fromJsonObject(QJsonObject jobj)
{
    BbItemData::fromJsonObject(jobj);
    font.setFamily(jobj["family"].toString());
    font.setPointSizeF(jobj["point_size"].toDouble());
    font.setWeight(jobj["weight"].toInt());
    font.setItalic(jobj["italic"].toBool());
    font.setBold(jobj["bold"].toBool());
    color.setRgba(QRgb(jobj["color"].toInt()));
    text = jobj["text"].toString();
    prevText = jobj["prev_text"].toString();
    empty = text.length() == 0;
}
