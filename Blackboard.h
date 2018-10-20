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

    qreal _penWeight;

    QPen _straightPen;

    qreal _straightPenWeight;

    QFont _font;

    QColor _textColor;

    qreal _textPointWeight;
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

    QString canvasId();

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

    void setStraightPen(const QPen & pen);

    const QPen & straightPen();

    void setPen(const QPen & pen);

    const QPen & pen(){return _pen;}

    void setFont(const QFont & font);

    const QFont & font();

    void setTextColor(const QColor & color);

    const QColor & textColor();

    void setPenWeight(const qreal & weight);

    void setStraightPenWeight(const qreal & weight);

    void setTextPointWeight(const qreal & weight);

    qreal textPointWeight();

    qreal penWeight();

    qreal straightPenWeight();

    QColor penColor();

    void setPenColor(const QColor & color);

    QColor straightPenColor();

    void setStraightPenColor(const QColor & color);


signals:
    void resized(float scale);
    void scrolled(float x, float y);

    void straightBegun(BbItemStraight *item);
    void straightDragged(BbItemStraight *item);
    void straightDone(BbItemStraight *item);
    void straightMoved(BbItemStraight *item);
    void straightDelete(BbItemStraight *item);

    void penDown(BbItemPen *item);
    void penDraw(BbItemPen *item);
    void penDone(BbItemPen *item);
    void penMoved(BbItemPen *item);
    void penDelete(BbItemPen *item);

    void textAdded(BbItemText *item);
    void textChanged(BbItemText *item);
    void textDone(BbItemText *item);
    void textMoved(BbItemText *item);
    void textDelete(BbItemText *item);

    void pointerShown(QPoint localPoint);
    void pointerMoved(QPoint localPoint);
    void pointerHidden(QPoint localPoint);

    void cursorShown(QPoint localPoint);
    void cursorMoved(QPoint localPoint);
    void cursorHidden(QPoint localPoint);
    void toolChanged(BbToolType previous, BbToolType current);

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &stream) override;
    virtual void readStream(QDataStream &stream) override;
};





#endif // CANVASVIEW3_H
