#include "BbHelper.h"
#include <QDebug>

IItemIndex *BbHelper::createItemWhenToolDown(BbToolType bbtt)
{
    switch(bbtt)
    {
        case BBTT_Triangle: return new BbItemTriangle();
        case BBTT_Ellipse: return new BbItemEllipse();
        case BBTT_Rectangle: return new BbItemRect();
        case BBTT_Pen: return new BbItemPen();
        case BBTT_Text: return new BbItemText();
        case BBTT_Straight: return new BbItemStraight();
        case BBTT_Image: return new BbItemImage();
        default:
        {
            qWarning() << "can not create item, not handling tool type: " << bbtt << " !";
            return nullptr;
        }
    }
}

IItemIndex *BbHelper::createItem(BbToolType bbtt)
{
    switch(bbtt)
    {
        case BBTT_Triangle: return new BbItemTriangle();
        case BBTT_Ellipse: return new BbItemEllipse();
        case BBTT_Rectangle: return new BbItemRect();
        case BBTT_Pen: return new BbItemPen();
        case BBTT_Text: return new BbItemText();
        case BBTT_Straight: return new BbItemStraight();
        case BBTT_Image: return new BbItemImage();
        default:
        {
            qWarning() << "can not create item, not handling tool type: " << bbtt << " !";
            return nullptr;
        }
    }
}

IItemIndex *BbHelper::createItem(BbItemData *data)
{
    if(data)
    {
        switch(data->tooltype)
        {
            case BBTT_Triangle: return new BbItemTriangle(data);
            case BBTT_Ellipse: return new BbItemEllipse(data);
            case BBTT_Rectangle: return new BbItemRect(data);
            case BBTT_Pen: return new BbItemPen(data);
            case BBTT_Text: return new BbItemText(data);
            case BBTT_Straight: return new BbItemStraight(data);
            case BBTT_Image: return new BbItemImage(data);
            default:
            {
                qWarning() << "can not create item, not handling tool type: " << data->tooltype << " !";
                return nullptr;
            }
        }
    }
    qWarning() << "can not create item, data is nullptr!";
    return nullptr;
}

BbItemData *BbHelper::createToolSettings(BbToolType bbtt)
{
    switch(bbtt)
    {
        case BBTT_Triangle:
        {
            auto ret = new BbItemTriangleData();
            ret->pen = BbItemTriangleData::getDefaultPen();
            ret->brush = BbItemTriangleData::getDefaultBrush();
            return ret;
        }
        case BBTT_Ellipse:
        {
            auto ret = new BbItemEllipseData();
            ret->pen = BbItemEllipseData::getDefaultPen();
            ret->brush = BbItemEllipseData::getDefaultBrush();
            return ret;
        }
        case BBTT_Rectangle:
        {
            auto ret = new BbItemRectData();
            ret->pen = BbItemRectData::getDefaultPen();
            ret->brush = BbItemRectData::getDefaultBrush();
            return ret;
        }
        case BBTT_Pen:
        {
            auto ret = new BbItemPenData();
            ret->pen = BbItemPenData::getDefaultPen();
            return ret;
        }
        case BBTT_Straight:
        {
            auto ret = new BbItemStraightData();
            ret->pen = BbItemPenData::getDefaultPen();
            return ret;
        }
        case BBTT_Text:
        {
            auto ret = new BbItemTextData();
            ret->font = BbItemTextData::getDefalutFont();
            ret->color = BbItemTextData::getDefalutColor();
            return ret;
        }
        case BBTT_Image:
        {
            auto ret = new BbItemImageData();
            return ret;
        }
        default:
        {
            qWarning() << "can not create settings, not handling tool type: " << bbtt << " !";
            return nullptr;
        }
    }
}
