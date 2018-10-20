#ifndef CANVASELEMENTTEXT_HPP
#define CANVASELEMENTTEXT_HPP

#include "IItemIndex.h"
#include "IStreamWR.h"

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QTextDocument>
#include <QDebug>
#include <QGraphicsScene>
#include <QDebug>

class BbItemText;
typedef std::function<void(BbItemText *)> onTextChangedCallback;
class BbItemTextData;
class BbItemText : public QGraphicsTextItem, public IStreamWR, public IItemIndex{

    BbItemTextData *_myData;

    QString _lastContent;

    onTextChangedCallback _onFoucsOut;

    onTextChangedCallback _onContentChanged;

public:
    BbItemText();

    BbItemText(BbItemTextData * data);

    virtual ~BbItemText() override;

    virtual void focusOutEvent(QFocusEvent *event) override;

    void setOnFoucsOutCallback(const onTextChangedCallback &onFoucsOut);

    void setContentChangedCallback(const onTextChangedCallback &onContentChanged);

    void inputMethodEvent(QInputMethodEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void repaintWithItemData();

    // ItemDataWR interface
public:
    virtual void writeStream(QDataStream &stream) override;
    virtual void readStream(QDataStream &stream) override;

    // IItemIndex interface
public:
    virtual QString id() const override;
    virtual void setId(const QString &id) override;
    virtual BbToolType toolType() const override;
    virtual BlackboardScene *scene() override;
};


#endif // CANVASELEMENTTEXT_HPP
