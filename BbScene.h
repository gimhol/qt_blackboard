#ifndef CANVASSCENE3_H
#define CANVASSCENE3_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <functional>
#include "BbToolType.h"
#include "IStreamWR.h"
#include "IItemIndex.h"

typedef std::function<QString()> ItemIDGenerator;
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
class NSB_BLACKBOARD_EXPORT BbScene: public QGraphicsScene, public IStreamWR
{
    Q_OBJECT
protected:
    friend class Blackboard;

    BbToolType _toolType = BBTT_Pen;     // 当前工具类型
    QPointF _mouseBeginPos;
    QPointF _mousePos;
    bool _mouseLeftButtonDown = false;
    QGraphicsRectItem * _pickerRect = nullptr;
    IItemIndex * _curItemIndex = nullptr;
    IItemIndex * _editingItemIndex = nullptr;
    ItemIDGenerator _itemIdGenerator = nullptr;
    bool _controlEnable = true;
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier;
    bool _onlyShiftDown = false;
    bool _onlyAltDown = false;
    bool _onlyCtrlDown = false;

    QSizeF _backgroundSize;
    QList<QGraphicsItem*> _backgrounds;
public:
    BbScene(Blackboard *parent = Q_NULLPTR);

    virtual ~BbScene() override;

    Blackboard *blackboard();

    /**
     * @brief setToolType   选择一个工具
     * @param toolType  工具类型
     */
    void setToolType(BbToolType toolType);

    BbToolType toolType();

    IItemIndex *enumAll(std::function<bool(IItemIndex *,int)> job);

    IItemIndex *enumSelected(std::function<bool(IItemIndex *,int)> job);

    /**
     * @brief removeSelectedElement 移除当前所有被选择的元素
     */
    void removeSelected();

    void remove(IItemIndex *item);

    void add(IItemIndex *item);

    /**
     * @brief setItemIdGenerator 设置id生成器
     * @param itemIDGenerator id生成器, 一个返回字符串类型的匿名函数
     */
    void setItemIdGenerator(ItemIDGenerator itemIDGenerator);

    /**
     * @brief setControlEnable 设置能否进行画板操作
     * @param enable true表示可以涂写，否则不能涂写。
     */
    void setControlEnable(bool enable);

    void clearItems();

    bool isPrivateItem(QGraphicsItem *item);

    IItemIndex *readItemData(BbItemData *itemData);

    bool isMouseLeftButtonDown();

    BbItemImage *addImageItem(const qreal &width, const qreal &height);

    BbItemImage *addImageItem(const QPixmap &pixmap);

    BbItemImage *addImageItem(const qreal &width, const qreal &height, const QPixmap &pixmap);

    BbItemImage *addImageItemWithPath(const QString &path);

    BbItemImage *addImageItemWithPath(const qreal &width,const qreal &height,const QString &path);

    BbItemImage *addImageItemWithUrl(const qreal &width,const qreal &height,const QString &url);

    IItemIndex *copyItemFromStream(QDataStream &stream);

    IItemIndex *readItemFromStream(QDataStream &stream);

    void selectedAll();

    void deselectAll();

    void copyItems();

    void pasteItems();

    QSizeF backgroundSize() const;

    void setBackgroundSize(QSizeF size);

    bool hasBackground() const;

    // use "clearBackground & addBackground" instand of "setBackground"
    void setBackground(const QPixmap &pixmap);

    void addBackground(const QPixmap &pixmap);

    void addBackground(QGraphicsItem *graphicsItem);

    void clearBackground();

    int backgroundCount();

    void removeBackground(int index);

    QGraphicsItem *background(int index);

    /**
     * @brief generatItemId 生成itemid
     * @return itemId
     */
    QString generatItemId() const;

    IItemIndex *currentItem();

    void setCurrentItem(IItemIndex *item);

    void unsetCurrentItem(IItemIndex *item);

    IItemIndex *editingItem();

    void setEditingItem(IItemIndex *item);

    void unsetEditingItem(IItemIndex *item);

    bool onlyShiftDown();

    Qt::KeyboardModifiers modifiers();

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

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *e) override;

    virtual void keyReleaseEvent(QKeyEvent *e) override;

    /**
     * @brief pickingItems 框选画板上的item
     * @param mousePos 鼠标位置
     */
    void pickingItems(const QPointF &mousePos);

    void onToolChanged(BbToolType previous);

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &stream) override;

    virtual void readStream(QDataStream &stream) override;

    void emitItemMovingSignals();

    void emitItemMovedSignals();

    void setItemPicking(bool picking);
};

#endif // CANVASSCENE3_H
