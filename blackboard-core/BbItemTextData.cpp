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

void BbItemTextData::setFont(const QFont &font)
{
    auto factor = fontSizeFactor();
    this->font = font;
    setFontSizeFactor(factor);
}

QJsonObject BbItemTextData::privateData()
{
    QJsonObject jdata;
    jdata["family"]              = font.family();
    jdata["weight"]              = font.weight();
    jdata["italic"]              = font.italic();
    jdata["bold"]                = font.bold();
    jdata["color"]               = int(color.rgba());
    jdata["text"]                = text;
    jdata["prev_text"]           = prevText;
    jdata["font_size_factor"]    = fontSizeFactor();
    return jdata;
}

void BbItemTextData::readPrivateData(const QJsonObject &jdata)
{
    font.setFamily(jdata["family"].toString());
    font.setWeight(jdata["weight"].toInt());
    font.setItalic(jdata["italic"].toBool());
    font.setBold(jdata["bold"].toBool());
    color.setRgba(QRgb(jdata["color"].toInt()));
    text = jdata["text"].toString();
    prevText = jdata["prev_text"].toString();
    empty = text.length() == 0;
    setFontSizeFactor(jdata["font_size_factor"].toDouble());
}
