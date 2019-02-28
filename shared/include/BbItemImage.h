#ifndef BBITEMIMAGE_H
#define BBITEMIMAGE_H

#include <QGraphicsPixmapItem>
#include "IStreamWR.h"
#include "IItemIndex.h"

class BbItemImageData;
class NSB_BLACKBOARD_EXPORT BbItemImage : public QGraphicsPixmapItem, public IStreamWR, public IItemIndex
{
protected:
    BbItemImageData * _myData;

public:
    BbItemImage();

    BbItemImage(BbItemImageData * data);

    virtual ~BbItemImage() override;

    void repaintWithItemData();

    void writeStream(QDataStream &stream) override;

    void readStream(QDataStream &stream) override;

    QString id() const override;

    void setId(const QString &id) override;

    BbToolType toolType() const override;

    BlackboardScene *scene() override;
};

#endif // BBITEMIMAGE_H
