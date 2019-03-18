#ifndef CANVASSCENE3_H
#define CANVASSCENE3_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <functional>
#include "BbToolType.h"
#include "IStreamWR.h"
#include "IItemIndex.h"

typedef std::function<QString()> ItemIDGenerator3;
class BbItemPen;
class BbItemPenData;
class BbItemText;
class BbItemTextData;
class BbItemStraight;
class BbItemStraightData;
class BbItemData;
class BbItemRect;
class BbItemRectData;
class BbItemEllipse;
class BbItemEllipseData;
class BbItemTriangle;
class BbItemTriangleData;
class BbItemImage;
class BbItemImageData;
class Blackboard;
class NSB_BLACKBOARD_EXPORT BlackboardScene: public QGraphicsScene, public IStreamWR
{
    Q_OBJECT
protected:
    friend class Blackboard;

    BbToolType _toolType = BBTT_Pen;     // 当前工具类型

    QPointF _mouseBeginPos;

    bool _mouseLeftButtonDown = false;

    QGraphicsRectItem * _pickerRect = nullptr;

    QGraphicsItem * _curElement = nullptr;

    ItemIDGenerator3 _itemIdGenerator = nullptr;

    bool _controlEnable = true;

    bool _onlyShiftDown = false;

    bool _onlyAltDown = false;

    QSize _backgroundSize = QSize(-1,-1);

    QGraphicsItem * _backgroundItem = nullptr;

    QString _canvasId = "";
public:
    BlackboardScene(Blackboard *parent = Q_NULLPTR);

    virtual ~BlackboardScene() override;

    Blackboard *blackboard();

    /**
     * @brief setToolType   选择一个工具
     * @param toolType  工具类型
     */
    void setToolType(BbToolType toolType);

    BbToolType toolType();

    /**
     * @brief removeSelectedElement 移除当前所有被选择的元素
     */
    void removeSelectedItems();

    void remove(QGraphicsItem *item);

    void add(QGraphicsItem *item);

    template<typename T>
    inline T *find(const std::string &lid)
    {
        return find<T>(QString::fromStdString(lid));
    }

    template<typename T>
    inline T *find(const QString &lid)
    {
        for(auto item: items())
        {
            IItemIndex *idx = dynamic_cast<IItemIndex *>(item);
            if(idx && idx->id() == lid)
            {
                return dynamic_cast<T*>(item);
            }
        }
        return nullptr;
    }

    /**
     * @brief setItemIdGenerator 设置id生成器
     * @param itemIDGenerator id生成器, 一个返回字符串类型的匿名函数
     */
    void setItemIdGenerator(ItemIDGenerator3 itemIDGenerator);

    /**
     * @brief setControlEnable 设置能否进行画板操作
     * @param enable true表示可以涂写，否则不能涂写。
     */
    void setControlEnable(bool enable);

    void clearItems();

    bool isPrivateItem(QGraphicsItem *item);

    void readItemData(BbItemData *itemData);

    void readPenData(BbItemPenData *penData);

    void readTextData(BbItemTextData *textData);

    void readStraightData(BbItemStraightData *straightData);

    void readRectData(BbItemRectData *rectData);

    void readEllipseData(BbItemEllipseData *ellipseData);

    void readTriangleData(BbItemTriangleData *triangleData);

    void readImageData(BbItemImageData *data);

    bool isMouseLeftButtonDown();

    void addImageItem(const QPixmap &pixmap);

    QGraphicsItem * copyItemFromStream(QDataStream &stream);

    QGraphicsItem * readItemFromStream(QDataStream &stream);

    void selectedAll();

    void deselectAll();

    void copyItems();

    void pasteItems();

    QSizeF backgroundSize() const;

    bool hasBackground() const;

    void setBackground(const QPixmap &pixmap);

    void clearBackground();

    void setCanvasId(const QString &id);

    QString canvasId() const;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *e) override;

    virtual void keyReleaseEvent(QKeyEvent *e) override;

    void localRectBegin(const QPointF &pos);

    void localRectDrag(const QPointF &pos);

    void localRectDone();

    void localEllipseBegin(const QPointF &pos);

    void localEllipseDrag(const QPointF &pos);

    void localEllipseDone();

    void localTriangleBegin(const QPointF &pos);

    void localTriangleDrag(const QPointF &pos);

    void localTriangleDone(bool force);

    void localStraightBegin(const QPointF &pos);

    void localStraightDrag(const QPointF &pos);

    void localStraightDone();

    /**
     * @brief localPenDown 本地下笔
     * @param pos 本地下笔位置
     */
    void localPenDown(const QPointF &pos);

    /**
     * @brief loaclPenDraw 本地的笔继续画
     * @param pos 本地笔的位置
     */
    void loaclPenDraw(const QPointF &pos);

    /**
     * @brief localPenDone 本地抬笔
     */
    void localPenDone();

    /**
     * @brief localTextAdded 本地文本添加
     * @param pos 本地文本位置
     */
    void localTextAdded(const QPointF &pos);

    /**
     * @brief localTextAdded 本地文本完成
     */
    void localTextDone();

    /**
     * @brief pickingItems 框选画板上的item
     * @param mousePos 鼠标位置
     */
    void pickingItems(const QPointF &mousePos);

    /**
     * @brief generatItemId 生成itemid
     * @return itemId
     */
    QString generatItemId() const;

    void onToolChanged(BbToolType previous, BbToolType current);

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &stream) override;

    virtual void readStream(QDataStream &stream) override;

    void emitItemMovingSignals();

    void emitItemMovedSignals();
};

#endif // CANVASSCENE3_H
