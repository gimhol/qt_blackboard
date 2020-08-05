#ifndef CANVASSCENE3_H
#define CANVASSCENE3_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <functional>
#include <cmath>
#include "BbToolType.h"
#include "IStreamWR.h"
#include "IItemIndex.h"

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
class NSB_BLACKBOARD_EXPORT BbScene:
        public QGraphicsScene,
        public IStreamWR,
        public IJsonWR
{
    Q_OBJECT
public:
    enum PageSplitterPosition{
        PSP_NOWHERE,
        PSP_BACKGROUND,
        PSP_FOREGROUND
    };
protected:
    friend class Blackboard;

    BbToolType _toolType = BBTT_Pen;     // 当前工具类型
    QPointF _pickerBeginPos;
    QPointF _mousePos;
    Qt::MouseButtons _mouseButtons;
    QGraphicsRectItem * _pickerRect = nullptr;
    IItemIndex * _curItemIndex = nullptr;
    Qt::KeyboardModifiers _modifiers = Qt::NoModifier;
    QRectF _backgroundRect;
    QList<QPair<QString,QGraphicsItem*>> _backgrounds;
    QList<IItemIndex*> _deletingItems;
    QTimer *_pickerTimer;

    QPen _pageSplitterPen = QPen(QColor(255,255,255,100),1,Qt::DashLine);
    PageSplitterPosition _pageSplitterPosition = PSP_FOREGROUND;
    qreal _pageAspectRatio = 1.414285714285714;
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

    void remove(QString lid);

    void remove(IItemIndex *item);

    void add(IItemIndex *item);

    void clearItems();

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

    IItemIndex *readItemFromJsonObject(const QJsonObject &jobj);

    void selectedAll();

    void deselectAll();

    void copyItems();

    void pasteItems();

    QSizeF backgroundSize() const;

    bool hasBackground() const;

    /*
     * use "clearBackground, addBackground, layBackgrounds"
     */
    void setBackground(const QPixmap &pixmap);

    QString addBackground(const QPixmap &pixmap);

    QString addBackground(QGraphicsItem *graphicsItem);

    void addBackground(QString backgroundId, const QPixmap &pixmap);

    void addBackground(QString backgroundId, QGraphicsItem *graphicsItem);

    void clearBackground();

    int backgroundCount();

    void removeBackground(int index);

    void removeBackground(QString id);

    QGraphicsItem *background(int index);

    QGraphicsItem *background(QString id);

    void layBackgrounds();

    QRectF backgroundRect();

    QList<QPair<QString,QGraphicsItem *>> backgrounds();

    IItemIndex *currentItem();

    void setCurrentItem(IItemIndex *item);

    void unsetCurrentItem(IItemIndex *item);

    Qt::KeyboardModifiers modifiers();

    bool isPicking();
    void startPicking();
    void onPicking();
    void stopPicking();

    PageSplitterPosition pageSplitterPosition() { return _pageSplitterPosition; }

    QPen pageSplitterPen(){ return _pageSplitterPen; }

    qreal pageAspectRatio(){ return _pageAspectRatio; }

    void setPageSplitterPosition(PageSplitterPosition value) { _pageSplitterPosition = value; }

    void setPageSplitterPen(QPen value){ _pageSplitterPen = value; }

    void setPageAspectRatio(qreal value){ _pageAspectRatio = (std::max)(value,0.01); }

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

    void checkItemsPicking();

    void onToolChanged(BbToolType previous);

    void drawPageSplitterWhenNeeded(QPainter *painter, const QRectF &rect);

    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void drawForeground(QPainter *painter, const QRectF &rect) override;

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &stream) override;

    virtual void readStream(QDataStream &stream) override;

    void emitItemMovingSignals();

    void emitItemMovedSignals();

    void updatePickerRect();

    // IJsonWR interface
public:
    virtual QJsonObject toJsonObject() override;

    virtual void fromJsonObject(const QJsonObject &jobj) override;

};

#endif // CANVASSCENE3_H
