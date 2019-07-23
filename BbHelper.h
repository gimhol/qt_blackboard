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

class NSB_BLACKBOARD_EXPORT BbHelper
{
public:
    static IItemIndex *createItem(BbToolType bbtt);
    static IItemIndex *createItem(BbItemData *data);
    static BbItemData *createToolSettings(BbToolType bbtt);
};

#endif // BBHELPER_H
