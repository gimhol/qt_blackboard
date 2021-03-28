#ifndef BBHELPER_H
#define BBHELPER_H

#include "BbItemData.h"
#include "BbItemEllipse.h"
#include "BbItemEllipseData.h"
#include "BbItemPen.h"
#include "BbItemPenData.h"
#include "BbItemRect.h"
#include "BbItemRectData.h"
#include "BbItemStraight.h"
#include "BbItemStraightData.h"
#include "BbItemText.h"
#include "BbItemTextData.h"
#include "BbItemTriangle.h"
#include "BbItemTriangleData.h"
#include "BbItemImage.h"
#include "BbItemImageData.h"
#include "BbItemPen2.h"
#include "BbItemPenData2.h"

class NSB_BLACKBOARD_EXPORT BbHelper
{
public:
    /**
     * @brief createItemWhenToolDown 黑板选择了某工具，并在黑板上点击时，创建对应的ITEM
     * @param bbtt 工具类型
     * @return 若工具类型支持创建item，返回对应类型的item的index指针。
     */
    static IItemIndex *createItemWhenToolDown(BbToolType bbtt);
    static IItemIndex *createItem(BbToolType bbtt);
    static IItemIndex *createItem(BbItemData *data);
    static BbItemData *createToolSettings(BbToolType bbtt);
    static BbItemData *createItemData(BbToolType bbtt);
    static QString toolTypeName(BbToolType bbtt);
};

#endif // BBHELPER_H
