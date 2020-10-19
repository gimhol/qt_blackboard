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
    virtual QString makeItemId(BbToolType bbtt = BBTT_None);
    virtual QString makeBackgroundId();
    virtual qreal makeItemZ(BbToolType bbtt = BBTT_None);
    virtual BbItemData *createItemData(BbToolType bbtt);

    template<typename T>
    inline T *createItemData(BbToolType bbtt){
        return dynamic_cast<T*>(createItemData(bbtt));
    }

private:
    qint64 _itemIdCount = 0;
    qint64 _bbIdCount = 0;
    qint64 _zCount = 0;
    qint64 _prevZ = INT64_MIN;
};

#endif // BBFACTORY_H
