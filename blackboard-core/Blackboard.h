#ifndef CANVASVIEW3_H
#define CANVASVIEW3_H

#include <QGraphicsView>
#include <QPointer>
#include "BBItemEventType.h"
#include "BbHelper.h"
#include "BbScene.h"
#include "BbFactory.h"

class BbItemData;
class BbCursor;
class BlackboardPrivate;
class NSB_BLACKBOARD_EXPORT Blackboard:
        public QGraphicsView,
        
        public IJsonWR
{
    Q_OBJECT
public:
    static void setDefaultFactory(BbFactory *defaultFactory);

    static BbFactory *defaultFactory();

    void setFactory(BbFactory *factory);

    BbFactory *factory();

    Blackboard(QWidget *parent = Q_NULLPTR);

    ~Blackboard() override;

    BbScene *scene() const;

    void onCanvasMouseTrack(const QString &remoteUserId, const float &x, const float &y, const QString name, int msec,const BbToolType &tool);

    QPointer<BbCursor> remoteCursor(const QString &remoteCursorId);

    QPointer<BbCursor> addRemoteCursor(const QString &remoteCursorId, const int &x, const int &y);

    QPointer<BbCursor> addRemoteCursor(const QString &remoteCursorId);

    bool moveRemoteCursor(const QString &remoteCursorId, const int &x, const int &y);

    bool showRemoteCursor(const QString &remoteCursorId);

    bool hideRemoteCursor(const QString &remoteCursorId);

    bool removeRemoteCursor(const QString &remoteCursorId);

    void setToolCursor(const BbToolType &tool, const QCursor &cursor);

    QCursor toolCursor(const BbToolType &tool);

    void toToolCursor(const BbToolType &tool);

    void revertToolCursor();

    void setPointerPixmap(const QPixmap & pixmap, const QPointF &hotAnchor = QPointF(0.5,0.5));

    void setScroll(const qreal &x, const qreal &y);

    QPointF getScrollValue();

    void setCanvasId(const QString &id);

    QString canvasId() const;

    qreal scaleRatio();

    QSizeF canvasSize();

    qreal canvasWidth();

    qreal canvasHeight();

    void onMousePress(const QPoint &pos, const Qt::MouseButton &button);

    void onMouseMove(const QPoint &pos);

    void onMouseRelease(const QPoint &pos, const Qt::MouseButton &button);

    bool eventFilter(QObject *object,QEvent *event) override;

    void tabletEvent(QTabletEvent *event) override;

    void resizeEvent(QResizeEvent * event) override;

//    void enterEvent(QEvent *event) override;

//    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void removeSelectedElement();

    void setToolType(BbToolType toolType);

    void setCanvasSize(const QSizeF& size);

    void setCanvasSize(const qreal &width, const qreal &height);

    void removeSelectedItems();

    void remove(QString lid);

    void remove(IItemIndex *item);

    void add(IItemIndex *item);

    void onScrollXChanged(int x);

    void onScrollYChanged(int y);

    void onScrollChanged(int x, int y);

    /**
     * @brief clearItems 清空数据
     */
    void clearItems();

    /**
     * @brief readItemData 读取一个item的数据，画到黑板上
     * @param itemData  item数据
     */
    IItemIndex *readItemData(BbItemData *itemData);

    void onToolChanged(BbToolType previous);
    BbToolType toolType();
    BbItemData *toolSettings(const BbToolType &toolType);
    BbItemImage *addImageItem(const qreal &width,const qreal &height);
    BbItemImage *addImageItem(const QPixmap & pixmap);
    BbItemImage *addImageItem(const qreal &width,const qreal &height,const QPixmap &pixmap);
    BbItemImage *addImageItemWithPath(const QString &path);
    BbItemImage *addImageItemWithPath(const qreal &width,const qreal &height,const QString &path);
    BbItemImage *addImageItemWithUrl(const qreal &width,const qreal &height,const QString &url);

    void selectedAll();
    void deselectAll();
    void cutItems();
    void copyItems();
    void pasteItems();

    QSizeF backgroundSize() const;

    /**
     * @brief hasBackground 是否存在背景
     * @return
     */
    bool hasBackground() const;

    /**
     * @brief setBackground 设置一个背景，就是按顺序调用了：
     *                             clearBackground();
     *                             addBackground(pixmap);
     *                             layBackgrounds();
     * @param pixmap 背景图
     */
    void setBackground(const QPixmap &pixmap);

    /**
     * @brief addBackground 添加一个背景
     * @param pixmap 背景图
     * @return 内部生成的背景ID
     */
    QString addBackground(const QPixmap &pixmap);

    /**
     * @brief addBackground 添加一个背景Item
     * @param graphicsItem 背景Item
     * @return 内部生成的背景ID
     */
    QString addBackground(QGraphicsItem *graphicsItem);

    /**
     * @brief addBackground 添加一个背景
     * @param id 背景的ID
     * @param pixmap 背景的图片
     */
    void addBackground(QString id,const QPixmap &pixmap);

    /**
     * @brief addBackground 添加一个背景Item
     * @param id 背景的ID
     * @param graphicsItem 背景Item
     */
    void addBackground(QString id,QGraphicsItem *graphicsItem);

    /**
     * @brief clearBackground 移除所有的背景
     */
    void clearBackground();
    /**
     * @brief backgroundCount 获取背景总数
     * @return 有几个背景
     */
    int backgroundCount();

    /**
     * @brief removeBackground 根据索引移除背景
     * @param id 第几个背景
     */
    void removeBackground(int index);

    /**
     * @brief removeBackground 根据ID移除背景
     * @param id 背景ID
     */
    void removeBackground(QString id);

    /**
     * @brief background 根据索引获取对应背景item
     * @param index 第几个背景
     * @return 背景item
     */
    QGraphicsItem *background(int index);

    /**
     * @brief background 根据ID获取对应背景
     * @param id 背景ID
     * @return 背景item
     */
    QGraphicsItem *background(QString id);

    /**
     * @brief layBackgrounds 自上而下的排列所有背景
     */
    void layBackgrounds();

    /**
     * @brief backgroundRect 计算并返回全部背景图的外接矩形，
     * @return 全部背景图的外接矩形
     */
    QRectF backgroundRect();

    /**
     * @brief backgroundRect 计算并返回全部背景图的外接矩形的尺寸，
     * @return 全部背景图的外接矩形的尺寸
     */
    QSizeF backgroundSize();

    /**
     * @brief backgrounds 获取所有背景图
     * @return 所有背景图列表
     */
    QList<QPair<QString,QGraphicsItem *>> backgrounds();

    void groupUp();

    void dismiss();

    void groupUp(QList<QString> itemIds, QString groupId);

    void dismiss(QList<QString> groupIds);

    template<class Cls> Cls* toolSettings(const BbToolType &toolType);
    template<class Cls> Cls* find(const std::string &lid);
    template<class Cls> Cls* find(const QString &lid);
    QPointF tabletPenPos();
    bool tabletActive();
signals:
    void scrolled(qreal x, qreal y);

    void itemChanged(BBItemEventType eventType, IItemIndex *index);
    void multipleItemChanged(BBItemEventType eventType, QList<IItemIndex*>list);

    void groupUped(QList<QGraphicsItemGroup *> groups);
    void dismissed(QList<QGraphicsItemGroup *> groups);

    void pointerShown(QPoint localPoint);
    void pointerMoving(QPoint localPoint);
    void pointerMoved(QPoint localPoint);
    void pointerHidden(QPoint localPoint);

    void cursorShown(QPoint localPoint);
    void cursorMoving(QPoint localPoint);
    void cursorMoved(QPoint localPoint);
    void cursorHidden(QPoint localPoint);
    void toolChanged(BbToolType previous, BbToolType current);

    void leftMouseTrack(QPoint localPoint);

public:
    virtual QJsonObject toJsonObject() override;
    virtual void fromJsonObject(const QJsonObject &jobj) override;

private:
    BlackboardPrivate *dptr;

public: // 'deprecated' zone
    [[deprecated("replace by addRemoteCursor.")]]
    QPointer<BbCursor> addPointer(const QString & remoteCursorId, const int &x, const int &y) {return addRemoteCursor(remoteCursorId); }

    [[deprecated("replace by addRemoteCursor.")]]
    QPointer<BbCursor> addPointer(const QString & remoteCursorId) {return addRemoteCursor(remoteCursorId); }

    [[deprecated("replace by findRemoteCursor.")]]
    QPointer<BbCursor> findPointer(const QString & remoteCursorId) {return remoteCursor(remoteCursorId); }

    [[deprecated("replace by moveRemoteCursor.")]]
    bool movePointer(const QString &remoteCursorId, const int &x, const int &y) { return moveRemoteCursor(remoteCursorId,x,y); }

    [[deprecated("replace by showRemoteCursor.")]]
    bool showPointer(const QString &remoteCursorId) {return showRemoteCursor(remoteCursorId); }

    [[deprecated("replace by hideRemoteCursor.")]]
    bool hidePointer(const QString &remoteCursorId) {return hideRemoteCursor(remoteCursorId); }

    [[deprecated("replace by removeRemoteCursor.")]]
    bool removePointer(const QString &remoteCursorId) {return removeRemoteCursor(remoteCursorId); }
};

template<class Cls>
Cls* Blackboard::toolSettings(const BbToolType &toolType)
{
    return dynamic_cast<Cls*>(toolSettings(toolType));
}
template<class Cls>
Cls* Blackboard::find(const std::string &lid)
{
    return find<Cls>(QString::fromStdString(lid));
}

template<class Cls>
Cls* Blackboard::find(const QString &lid)
{
    return scene()->find<Cls>(lid);
}

#endif // CANVASVIEW3_H
