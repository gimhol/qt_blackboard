#ifndef BBITEMELLIPSEDATA_H
#define BBITEMELLIPSEDATA_H

#include "BbItemData.h"
#include <QPen>
#include <QBrush>

class BbItemEllipseData: public BbItemData
{
public:
    static QColor defaultPenColor;

    static qreal minPenWidth;

    static qreal maxPenWidth;

    static QPen defaultPen;

    static QColor defaultBrushColor;

    static QBrush defaultBrush;

    QPen pen = defaultPen;

    QBrush brush = defaultBrush;

    QSizeF size;

    explicit BbItemEllipseData(CoordMode mode = CM_ABSOLUTE);

    qreal weight();

    void setWeight(qreal weight);

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;
};

#endif // BBITEMELLIPSEDATA_H
