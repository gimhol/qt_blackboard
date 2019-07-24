#ifndef BBITEMIMAGE_H
#define BBITEMIMAGE_H

#include <QGraphicsPixmapItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemImageData;
class NSB_BLACKBOARD_EXPORT BbItemImage : protected QGraphicsPixmapItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemImageData * _myData;

public:
    BbItemImage();

    BbItemImage(BbItemData * data);

    virtual ~BbItemImage() override;

    void init();

    void setPixmap(const QPixmap &pixmap);

    const QPixmap &pixmap();

    void repaint() override;

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;

    QString id() const override;

    void setId(const QString &id) override;
    BbToolType toolType() const override;
    BbScene *scene() override;
    BbItemData *data() override;
    Blackboard *blackboard() override;
    void toolDown(const QPointF &pos) override;
    void toolDraw(const QPointF &pos) override;
    void toolDone(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void added() override {}
    void removed() override {}
    qreal z() override;
    void setZ(const qreal &value) override;
    void toAbsoluteCoords() override;
};

#endif // BBITEMIMAGE_H
