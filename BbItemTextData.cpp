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

void BbItemTextData::writeStream(QDataStream &stream)
{
    //先写入基础数据
    BbItemData::writeStream(stream);

    //再写入文本数据
    stream << font.family()
           << font.weight()
           << font.italic()
           << font.bold()
           << color.rgba()
           << text;
}

void BbItemTextData::readStream(QDataStream &stream)
{
    //先读取基础数据
    BbItemData::readStream(stream);

    //再读取文本数据
    QString fontFamily;

    int weight;
    bool italic, bold;
    QRgb rgba;

    stream >> fontFamily
            >> weight
            >> italic
            >> bold
            >> rgba
            >> text;

    isEmpty = text.length() == 0;

    font.setFamily(fontFamily);
    font.setWeight(weight);
    font.setItalic(italic);
    font.setBold(bold);
    color.setRgb(rgba);
}
void BbItemTextData::setPointWeight(qreal weight)
{
    qreal pointSize = minPointSize + weight * (maxPointSize - minPointSize);
    font.setPointSizeF(pointSize);

    qDebug() << font.pointSize();
}

qreal BbItemTextData::pointWeight()
{
    return (font.pointSizeF()-minPointSize)/(maxPointSize-minPointSize);
}
