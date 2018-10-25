#include "BbItemText.h"
#include "BbItemTextData.h"
#include "BlackboardScene.h"

BbItemText::BbItemText():QGraphicsTextItem()
  ,_onFoucsOut(nullptr)
{
    _myData = new BbItemTextData();
    setFont(_myData->font);
    setDefaultTextColor(_myData->color);
}

BbItemText::BbItemText(BbItemTextData *data)
{
    _myData = data;
    setFont(_myData->font);
    setDefaultTextColor(_myData->color);
}

BbItemText::~BbItemText()
{
    delete _myData;
}

void BbItemText::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    setTextInteractionFlags(Qt::NoTextInteraction);
    QTextCursor cursor = textCursor();
    cursor.setPosition(0);
    setTextCursor(cursor);
    clearFocus();
    if(_onFoucsOut){
        _onFoucsOut(this);
    }
}

void BbItemText::setContentChangedCallback(const onTextChangedCallback &onContentChanged){
    _onContentChanged = onContentChanged;
}

void BbItemText::inputMethodEvent(QInputMethodEvent *event)
{
    QGraphicsTextItem::inputMethodEvent(event);

    auto curContent = toPlainText();
    if(_onContentChanged && _lastContent != curContent){
        _onContentChanged(this);
        _lastContent = curContent;
    }
}

void BbItemText::keyPressEvent(QKeyEvent *event)
{
    QGraphicsTextItem::keyPressEvent(event);
    auto curContent = toPlainText();

    if(_onContentChanged && _lastContent != curContent){
        _onContentChanged(this);
        _lastContent = curContent;
    }
}

void BbItemText::repaintWithItemData()
{
    setFont(_myData->font);
    setDefaultTextColor(_myData->color);
    setPlainText(_myData->text);

    if( _myData->x > -9998 && _myData->y > -9998 ){
        qreal x = _myData->x;
        qreal y = _myData->y;
        if(_myData->mode == BbItemData::CM_PERCENTAGE)
        {
            qreal width = scene()->width();
            qreal ratio = width / 100;
            x *= ratio;
            y *= ratio;
        }
        setPos(x,y);
    }

    setSelected(false);
    setEnabled(true);
    setZValue(_myData->z);
    update();
}

void BbItemText::setFont(const QFont &font)
{
    _myData->font = font;
    QGraphicsTextItem::setFont(_myData->font);
}

const QFont &BbItemText::font()
{
    return _myData->font;
}

void BbItemText::setColor(const QColor &color)
{
    _myData->color = color;
    QGraphicsTextItem::setDefaultTextColor(color);
}

void BbItemText::setDefaultTextColor(const QColor &color)
{
    _myData->color = color;
    QGraphicsTextItem::setDefaultTextColor(color);
}

const QColor &BbItemText::color()
{
    return _myData->color;
}

void BbItemText::setWeight(qreal weight)
{
    _myData->setPointWeight(weight);
    QGraphicsTextItem::setFont(_myData->font);
}

qreal BbItemText::weight()
{
    return _myData->pointWeight();
}

void BbItemText::writeStream(QDataStream &stream)
{
    _myData->text = toPlainText();
    _myData->x = x();
    _myData->y = y();
    _myData->z = zValue();
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        qreal ratio = scene()->width() / 100;
        _myData->x /= ratio;
        _myData->y /= ratio;
    }
    _myData->writeStream(stream);
}

void BbItemText::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    repaintWithItemData();
}

QString BbItemText::id() const
{
    return _myData->lid;
}

void BbItemText::setId(const QString &id)
{
    _myData->lid = id;
}

BbToolType BbItemText::toolType() const
{
    return _myData->tooltype;
}

BlackboardScene *BbItemText::scene()
{
    return dynamic_cast<BlackboardScene *>(QGraphicsTextItem::scene());
}

void BbItemText::setOnFoucsOutCallback(const onTextChangedCallback &onFoucsOut){
    _onFoucsOut = onFoucsOut;
}
