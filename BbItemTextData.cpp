#include "BbItemTextData.h"

#include <QDebug>

qreal BbItemTextData::minPointSize = 9;

qreal BbItemTextData::maxPointSize = 72;

QFont BbItemTextData::defalutFont = QFont(u8"宋体", BbItemTextData::minPointSize);

QColor BbItemTextData::defalutColor = QColor(100,100,180);

BbItemTextData::BbItemTextData(BbItemData::CoordMode mode):
    BbItemData(mode)
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
