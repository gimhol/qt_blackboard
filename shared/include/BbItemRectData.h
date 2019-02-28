#ifndef BBITEMRECTDATA_H
#define BBITEMRECTDATA_H

#include "BbItemData.h"
#include <QPen>
#include <QBrush>

class NSB_BLACKBOARD_EXPORT BbItemRectData : public BbItemData
{
public:
    static const QColor &getDefaultPenColor();
    static const qreal &getMinPenWidth();
    static const qreal &getMaxPenWidth();
    static const QPen &getDefaultPen();
    static const QColor &getDefaultBrushColor();
    static const QBrush &getDefaultBrush();

    static void setDefaultPenColor(const QColor &value);
    static void setMinPenWidth(const qreal &value);
    static void setMaxPenWidth(const qreal &value);
    static void setDefaultPen(const QPen &value);
    static void setDefaultBrushColor(const QColor &value);
    static void setDefaultBrush(const QBrush &value);

    QPen pen;

    QBrush brush;

    QSizeF size;

    explicit BbItemRectData(CoordMode mode = CM_ABSOLUTE);

    qreal weight();

    void setWeight(qreal weight);

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;
};

#endif // BBITEMRECTDATA_H
