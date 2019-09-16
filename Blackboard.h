#ifndef CANVASVIEW3_H
#define CANVASVIEW3_H

#include <QGraphicsView>
#include "BBItemEventType.h"
#include "BbHelper.h"
#include "BbScene.h"

class BbItemData;
class BbPointer;
class BlackboardPrivate;
class NSB_BLACKBOARD_EXPORT Blackboard: public QGraphicsView, public IStreamWR
{
    Q_OBJECT
public:
    Blackboard(QWidget *parent = Q_NULLPTR);

    ~Blackboard() override;

    BbScene *scene() const;

    BbPointer *addPointer(const QString & pointerId, int x, int y);

    void movePointer(const QString & pointerId, int x, int y);

    void hidePointer(const QString & pointerId);

    void setToolCursor(const BbToolType &tool, const QCursor &cursor);

    QCursor toolCursor(const BbToolType &tool);

    void toToolCursor(const BbToolType &tool);

    void revertToolCursor();

    void setPointerPixmap(const QPixmap & pixmap);

    void setScroll(int x,int y);

    QPoint getScrollValue();

    void setCanvasId(const QString &id);

    QString canvasId() const;

    qreal scaleRatio();

    QSize orginalSize();

    int orginalWidth();

    int orginalHeight();

    void setOrginalSize(int width,int height);

    void setOrginalSize(const QSize &size);

    qreal orginalRatio();

    QSize canvasSize();

    int canvasWidth();

    int canvasHeight();

    void onMousePress(const QPoint &pos, const Qt::MouseButton &button);

    void onMouseMove(const QPoint &pos);

    void onMouseRelease(const QPoint &pos, const Qt::MouseButton &button);

    bool eventFilter(QObject *object,QEvent *event) override;

    void tabletEvent(QTabletEvent *event) override;

    void moveEvent(QMoveEvent *event) override;

    void resizeEvent(QResizeEvent * event) override;

    void enterEvent(QEvent *event) override;

    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void removeSelectedElement();

    void setToolType(BbToolType toolType);

    void setCanvasSize(const QSize& size);

    void setCanvasSize(int width, int height);

    void setControlEnable(bool enable);

    void removeSelectedItems();

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
    void readItemData(BbItemData *itemData);

    void onToolChanged(BbToolType previous);
    BbToolType toolType();
    BbItemData *toolSettings(const BbToolType &toolType);
    BbItemImage *addImageItem(const qreal &width,const qreal &height);
    BbItemImage *addImageItem(const QPixmap & pixmap);
    BbItemImage *addImageItem(const qreal &width,const qreal &height,const QPixmap &pixmap);
    void selectedAll();
    void deselectAll();
    void copyItems();
    void pasteItems();
    QSizeF backgroundSize() const;
    bool hasBackground() const;
    void setBackground(const QPixmap &pixmap);
    void clearBackground();
    template<class T> inline T *toolSettings(const BbToolType &toolType)
    {
        return dynamic_cast<T*>(toolSettings(toolType));
    }
    template<typename T> inline T *find(const std::string &lid)
    {
        return find<T>(QString::fromStdString(lid));
    }
    template<typename T> inline T *find(const QString &lid)
    {
        return scene()->find<T>(lid);
    }
signals:
    void moved();
    void resized(float scale);
    void scrolled(float x, float y);
    void itemSelected(IItemIndex *index, bool selected);

    void itemChanged(BBItemEventType eventType,IItemIndex *index);
    void multipleItemChanged(BBItemEventType eventType,IItemIndex *first);

    void pointerShown(QPoint localPoint);
    void pointerMoving(QPoint localPoint);
    void pointerMoved(QPoint localPoint);
    void pointerHidden(QPoint localPoint);

    void cursorShown(QPoint localPoint);
    void cursorMoving(QPoint localPoint);
    void cursorMoved(QPoint localPoint);
    void cursorHidden(QPoint localPoint);
    void toolChanged(BbToolType previous, BbToolType current);

public:
    virtual void writeStream(QDataStream &stream) override;
    virtual void readStream(QDataStream &stream) override;

private:
    BlackboardPrivate *dptr;
};





#endif // CANVASVIEW3_H
