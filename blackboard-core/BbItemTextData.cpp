#include "BbItemTextData.h"

#include <QDebug>

static qreal minFontSize = 17;
static qreal maxFontSize = 37;
static QFont makeDefaultFont(){
    QFont ret(QStringLiteral("宋体"));
    ret.setPixelSize(minFontSize);
    return ret;
}
static QFont defalutFont = makeDefaultFont();
static QColor defalutColor = QColor(100,100,180);

const qreal &BbItemTextData::getMinFontSize()
{
    return minFontSize;
}

const qreal &BbItemTextData::getMaxFontSize()
{
    return maxFontSize;
}

const QFont &BbItemTextData::getDefalutFont()
{
    return defalutFont;
}

const QColor &BbItemTextData::getDefalutColor()
{
    return defalutColor;
}

void BbItemTextData::setMinFontSize(const qreal &value)
{
    minFontSize = value;
    defalutFont.setPixelSize(minFontSize);
}

void BbItemTextData::setMaxFontSize(const qreal &value)
{
    maxFontSize = value;
}

void BbItemTextData::setDefalutFont(const QFont &value)
{
    defalutFont = value;
    defalutFont.setPixelSize(minFontSize);
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

void BbItemTextData::setFontSizeFactor(qreal factor)
{
    auto size = minFontSize + factor * (maxFontSize - minFontSize);
    font.setPixelSize(size);
}

qreal BbItemTextData::fontSizeFactor()
{
    return (font.pixelSize()-minFontSize)/(maxFontSize-minFontSize);
}

QJsonObject BbItemTextData::toJsonObject()
{
    auto jobj = BbItemData::toJsonObject();
    jobj["family"]              = font.family();
    jobj["weight"]              = font.weight();
    jobj["italic"]              = font.italic();
    jobj["bold"]                = font.bold();
    jobj["color"]               = int(color.rgba());
    jobj["text"]                = text;
    jobj["prev_text"]           = prevText;
    jobj["font_size_factor"]    = fontSizeFactor();
    return jobj;
}

void BbItemTextData::fromJsonObject(const QJsonObject &jobj)
{
    BbItemData::fromJsonObject(jobj);
    font.setFamily(jobj["family"].toString());
    font.setWeight(jobj["weight"].toInt());
    font.setItalic(jobj["italic"].toBool());
    font.setBold(jobj["bold"].toBool());
    color.setRgba(QRgb(jobj["color"].toInt()));
    text = jobj["text"].toString();
    prevText = jobj["prev_text"].toString();
    empty = text.length() == 0;
    setFontSizeFactor(jobj["font_size_factor"].toDouble());
}

void BbItemTextData::setFont(const QFont &font)
{
    auto factor = fontSizeFactor();
    this->font = font;
    setFontSizeFactor(factor);
}
