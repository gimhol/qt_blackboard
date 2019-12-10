#ifndef CANVASELEMENTTEXT_HPP
#define CANVASELEMENTTEXT_HPP

#include "IItemIndex.h"
#include "IStreamWR.h"
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QTextDocument>
#include <QGraphicsScene>
#include <functional>

class BbItemText;
typedef std::function<void()> onTextChangedCallback;
class BbItemTextData;
class NSB_BLACKBOARD_EXPORT BbItemText : public QGraphicsTextItem, public IStreamWR, public IItemIndex
{
    BB_HIDE_POS_SETTER
public:
    BbItemText();

    BbItemText(BbItemData * data);

    virtual ~BbItemText() override;

    void init();

    void focusOutEvent(QFocusEvent *event) override;

    void inputMethodEvent(QInputMethodEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void repaint() override;

    void setFont(const QFont & font);

    const QFont & font();

    void setColor(const QColor & color);

    void setDefaultTextColor(const QColor &color);

    const QColor & color();

    void setWeight(qreal weight);

    qreal weight();

    void done();

    void updateContent();

    void setText(const QString &text);

    QString text();

    bool isEmpty();

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &stream) override;
    virtual void readStream(QDataStream &stream) override;

    // IItemIndex interface
public:
    BbItemData *data() override;
    void toolDown(const QPointF &pos) override;
    void toolDraw(const QPointF &pos) override;
    void toolDone(const QPointF &pos) override;
    void modifiersChanged(Qt::KeyboardModifiers modifiers) override;
    void removed() override;
    void absolutize() override;
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    BbItemTextData *_myData;
    QString _lastContent;
    bool isRemoved = false;
};


#endif // CANVASELEMENTTEXT_HPP
