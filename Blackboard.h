#ifndef CANVASVIEW3_H
#define CANVASVIEW3_H

#include <QGraphicsView>
#include "BbToolType.h"
#include "BlackboardScene.h"
class BbItemData;
class QLabel;
class Blackboard: public QGraphicsView, public IStreamWR
{
    Q_OBJECT
protected:
    float _scaleRatio;

    QSize _orginalSize;

    QSize _canvasSize;

    bool _leftButtonDown = false;

    QMap<BbToolType,QCursor> _cursors;

    QPixmap _pointerPixmap;

    QPoint _mousePos;

    QPoint _scrollValue;

    QPen _pen;

    qreal _penWeight = 0;

    QPen _straightPen;

    qreal _straightPenWeight = 0;

    QFont _font;

    QColor _textColor;

    qreal _textPointWeight = 0;

    QPen _rectPen;

    QBrush _rectBrush;

    qreal _rectWeight = 0;

    QPen _ellipsePen;

    QBrush _ellipseBrush;

    qreal _ellipseWeight = 0;

    QPen _trianglePen;

    QBrush _triangleBrush;

    qreal _triangleWeight = 0;

public:
    Blackboard(QWidget *parent = Q_NULLPTR);

    BlackboardScene *scene() const;

    QLabel * addPointer(const QString & pointerId, int x, int y);

    void movePointer(const QString & pointerId, int x, int y);

    void hidePointer(const QString & pointerId);

    void setToolCursor(const BbToolType &tool, const QCursor &cursor);

    void setPointerPixmap(const QPixmap & pixmap);

    void setScroll(int x,int y);

    void setCanvasId(const QString &id);

    QString canvasId() const;

    float scaleRatio();

    QSize orginalSize();

    int orginalWidth();

    int orginalHeight();

    void setOrginalSize(int width,int height);

    void setOrginalSize(const QSize &size);

    float orginalRatio();

    QSize canvasSize();

    int canvasWidth();

    int canvasHeight();

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

    void onToolChanged(BbToolType previous, BbToolType current);

    static void setDefaultPen(const QPen & defaultPen);

    static const QPen & defaultPen();

    static void setDefaultFont(const QFont & defaultFont);

    static const QFont & defaultFont();

    static void setDefaultTextColor(const QColor & color);

    static const QColor & defaultTextColor();

    const QFont & font();
    const QColor & textColor();
    qreal textPointWeight();
    void setFont(const QFont & font);
    void setTextColor(const QColor & color);
    void setTextPointWeight(const qreal & weight);

    const QPen & pen();
    qreal penWeight();
    QColor penColor();
    void setPen(const QPen & pen);
    void setPenWeight(const qreal & weight);
    void setPenColor(const QColor & color);

    const QPen & straightPen();
    qreal straightPenWeight();
    QColor straightPenColor();
    void setStraightPen(const QPen & pen);
    void setStraightPenWeight(const qreal & weight);
    void setStraightPenColor(const QColor & color);

    QColor rectPenColor();
    QColor rectBrushColor();
    qreal rectWeight();
    void setRectPenColor(const QColor & color);
    void setRectBrushColor(const QColor & color);
    void setRectWeight(const qreal & weight);

    QColor ellipsePenColor();
    QColor ellipseBrushColor();
    qreal ellipseWeight();
    void setEllipsePenColor(const QColor & color);
    void setEllipseBrushColor(const QColor & color);
    void setEllipseWeight(const qreal & weight);

    QColor trianglePenColor();
    QColor triangleBrushColor();
    qreal triangleWeight();
    void setTrianglePenColor(const QColor & color);
    void setTriangleBrushColor(const QColor & color);
    void setTriangleWeight(const qreal & weight);

    void addPixmapItem(const QPixmap & pixmap);
    void selectedAll();
    void deselectAll();
    void copyItems();
    void pasteItems();

    void setBackground(const QPixmap &pixmap);
    void clearBackground();
signals:
    void moved();
    void resized(float scale);
    void scrolled(float x, float y);

    void straightBegun(BbItemStraight *item);
    void straightDragged(BbItemStraight *item);
    void straightDone(BbItemStraight *item);
    void straightMoving(BbItemStraight *item);
    void straightMoved(BbItemStraight *item);
    void straightDelete(BbItemStraight *item);
    void straightPaste(BbItemStraight *item);

    void penDown(BbItemPen *item);
    void penDraw(BbItemPen *item);
    void penDone(BbItemPen *item);
    void penMoving(BbItemPen *item);
    void penMoved(BbItemPen *item);
    void penStraighting(BbItemPen *item);
    void penDelete(BbItemPen *item);
    void penPaste(BbItemPen *item);

    void textAdded(BbItemText *item);
    void textChanged(BbItemText *item);
    void textDone(BbItemText *item);
    void textMoving(BbItemText *item);
    void textMoved(BbItemText *item);
    void textDelete(BbItemText *item);
    void textPaste(BbItemText *item);

    void pointerShown(QPoint localPoint);
    void pointerMoving(QPoint localPoint);
    void pointerMoved(QPoint localPoint);
    void pointerHidden(QPoint localPoint);

    void cursorShown(QPoint localPoint);
    void cursorMoving(QPoint localPoint);
    void cursorMoved(QPoint localPoint);
    void cursorHidden(QPoint localPoint);
    void toolChanged(BbToolType previous, BbToolType current);

    void rectBegun(BbItemRect *item);
    void rectDragged(BbItemRect *item);
    void rectDone(BbItemRect *item);
    void rectMoving(BbItemRect *item);
    void rectMoved(BbItemRect *item);
    void rectDelete(BbItemRect *item);
    void rectPaste(BbItemRect *item);

    void ellipseBegun(BbItemEllipse *item);
    void ellipseDragged(BbItemEllipse *item);
    void ellipseDone(BbItemEllipse *item);
    void ellipseMoving(BbItemEllipse *item);
    void ellipseMoved(BbItemEllipse *item);
    void ellipseDelete(BbItemEllipse *item);
    void ellipsePaste(BbItemEllipse *item);

    void triangleBegun(BbItemTriangle *item);
    void triangleDragged(BbItemTriangle *item);
    void triangleDone(BbItemTriangle *item);
    void triangleMoving(BbItemTriangle *item);
    void triangleMoved(BbItemTriangle *item);
    void triangleDelete(BbItemTriangle *item);
    void trianglePaste(BbItemTriangle *item);

    void imageAdded(BbItemImage *item);
    void imageMoving(BbItemImage *item);
    void imageMoved(BbItemImage *item);
    void imageDelete(BbItemImage *item);
    void imagePaste(BbItemImage *item);

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &stream) override;
    virtual void readStream(QDataStream &stream) override;
};





#endif // CANVASVIEW3_H
