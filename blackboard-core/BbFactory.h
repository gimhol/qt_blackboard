#ifndef BBFACTORY_H
#define BBFACTORY_H
#include <BbHelper.h>

class NSB_BLACKBOARD_EXPORT BbFactory
{
public:
    BbFactory();
    virtual ~BbFactory();
    virtual BbScene *createScene(Blackboard *blackboard);
    virtual IItemIndex *createItemWhenToolDown(BbToolType bbtt);
    virtual IItemIndex *createItem(BbToolType bbtt);
    virtual IItemIndex *createItem(BbItemData *data);
    virtual IItemIndex *createItem(const QJsonObject &jobj);
    virtual BbItemData *createToolSettings(BbToolType bbtt);
    virtual QString makeItemId();
    virtual QString makeBackgroundId();
    virtual qreal makeItemZ();
private:
    qint64 _itemIdCount = 0;
    qint64 _bbIdCount = 0;
    qint64 _zCount = 0;
};

#endif // BBFACTORY_H
