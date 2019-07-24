#include "BbItemText.h"
#include "BbItemTextData.h"
#include "Blackboard.h"
#include "BbScene.h"

#include <QDateTime>
#include <QKeyEvent>

BbItemText::BbItemText():
    QGraphicsTextItem(),
    IItemIndex(nullptr),
    _myData(new BbItemTextData()),
    _onFoucsOut(nullptr),
    _onContentChanged(nullptr)
{
    init();
}

BbItemText::BbItemText(BbItemData *data):
    QGraphicsTextItem(),
    IItemIndex (data),
    _myData(dynamic_cast<BbItemTextData*>(data)),
    _onFoucsOut(nullptr),
    _onContentChanged(nullptr)
{
    init();
}

BbItemText::~BbItemText()
{
    delete _myData;
}

void BbItemText::init()
{
    if(!_myData)
    {
        _myData = new BbItemTextData();
    }
    setFont(_myData->font);
    setDefaultTextColor(_myData->color);
}

void BbItemText::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    setTextInteractionFlags(Qt::NoTextInteraction);
    QTextCursor cursor = textCursor();
    cursor.setPosition(0);
    setTextCursor(cursor);
    clearFocus();
    scene()->unsetCurrentItem(this);
    if(_onFoucsOut)
    {
        _onFoucsOut();
    }
}

void BbItemText::inputMethodEvent(QInputMethodEvent *event)
{
    QGraphicsTextItem::inputMethodEvent(event);
    updateContent();
}

void BbItemText::keyPressEvent(QKeyEvent *event)
{
    QGraphicsTextItem::keyPressEvent(event);
    switch(event->key())
    {
        case Qt::Key_Return:
        {
            if(event->modifiers() == Qt::ControlModifier)
            {
                done();
            }
            break;
        }
        case Qt::Key_Escape:
        {
            done();
            break;
        }
        default:
        {
            updateContent();
            break;
        }
    }
}

void BbItemText::repaint()
{
    setFont(_myData->font);
    setDefaultTextColor(_myData->color);
    setPlainText(_myData->text);
    if( _myData->isPositionValid() ){
        setPos(_myData->x,_myData->y);
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

void BbItemText::done()
{
    clearFocus();
//    emit blackboard()->itemChanged(BBIET_textDone,this);
//    scene()->unsetCurrentItem(this);
}

void BbItemText::updateContent()
{
    auto curContent = text();
    if(_lastContent != curContent)
    {
        _myData->text = curContent;
        if(_onContentChanged)
        {
            _onContentChanged();
        }
        _lastContent = curContent;
    }
}

void BbItemText::setText(const QString &text)
{
    setPlainText(text);
    _myData->text = text;
    _lastContent = text;
}

QString BbItemText::text()
{
    return document()->toRawText();
}

bool BbItemText::isEmpty()
{
    return toPlainText().replace(QRegExp("\\s"),"").isEmpty();
}

void BbItemText::writeStream(QDataStream &stream)
{
    _myData->text = text();
    _myData->x = x();
    _myData->y = y();
    _myData->z = zValue();
    _myData->writeStream(stream);
}

void BbItemText::readStream(QDataStream &stream)
{
    _myData->readStream(stream);
    toAbsoluteCoords();
    repaint();
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

Blackboard *BbItemText::blackboard()
{
    return scene()->blackboard();
}

BbScene *BbItemText::scene()
{
    return dynamic_cast<BbScene *>(QGraphicsItem::scene());
}

BbItemData *BbItemText::data()
{
    return _myData;
}

bool BbItemText::doubleClicked(const QPointF &pos)
{
    if(boundingRect().contains(pos-this->pos())){
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus();
        return true;
    }
    return false;
}

void BbItemText::toolDown(const QPointF &pos)
{
    if(scene()->currentItem() == this)
    {
        setPos(pos.x(), pos.y() - 0.5 * boundingRect().height());
        _myData->updatePostion(this);
        _myData->prevX = _myData->x;
        _myData->prevY = _myData->y;
        if(!isEmpty())
        {
            emit blackboard()->itemChanged(BBIET_itemMoved,this);
        }
    }
    else
    {
        scene()->setCurrentItem(this);
        setId(scene()->generatItemId());
        setZValue(QDateTime::currentMSecsSinceEpoch());

        auto settings = blackboard()->toolSettings<BbItemTextData>(BBTT_Text);
        setFont(settings->font);
        setWeight(settings->pointWeight());
        setColor(settings->color);

        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus();
        setPos(pos.x(), pos.y() - 0.5 * boundingRect().height());

        if(!isEmpty())
        {
            emit blackboard()->itemChanged(BBIET_textAdded,this);
        }
    }
}

void BbItemText::toolDraw(const QPointF &pos)
{
    Q_UNUSED(pos);
    // do nothing.
}

void BbItemText::toolDone(const QPointF &pos)
{
    Q_UNUSED(pos);
    // do nothing.
}

void BbItemText::modifiersChanged(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    // do nothing.
}

void BbItemText::removed()
{
    _onContentChanged = nullptr;
    _onFoucsOut = nullptr;
}

void BbItemText::added()
{
    _onFoucsOut = [&]()
    {
        setActive(false);
        setSelected(false);
        if(isEmpty()) // 移除本地。
        {
            scene()->remove(this);
        }
        else
        {
            emit blackboard()->itemChanged(BBIET_textDone,this);
        }
    };
    _onContentChanged = [&]()
    {
        auto prevEmpty = _lastContent.replace(QRegExp("\\s"),"").isEmpty();
        auto currEmpty = isEmpty();
        if(prevEmpty && !currEmpty) // 无》有
        {
            emit blackboard()->itemChanged(BBIET_textAdded,this);
        }
        else if(!prevEmpty && currEmpty) // 有》无
        {
            emit blackboard()->itemChanged(BBIET_itemDelete,this);
        }
        else if(!prevEmpty && !currEmpty) // 有》有
        {
            emit blackboard()->itemChanged(BBIET_textChanged,this);
        }
    };
}

qreal BbItemText::z()
{
    return zValue();
}

void BbItemText::setZ(const qreal &value)
{
    setZValue(value);
    _myData->z = value;
}

void BbItemText::setPos(const QPointF &pos)
{
    QGraphicsItem::setPos(pos);
    _myData->x = pos.x();
    _myData->y = pos.y();
}

void BbItemText::setPos(qreal x, qreal y)
{
    QGraphicsItem::setPos(x,y);
    _myData->x = x;
    _myData->y = y;
}

QPointF BbItemText::pos()
{
    return QGraphicsItem::pos();
}

void BbItemText::toAbsoluteCoords()
{
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        _myData->mode = BbItemData::CM_ABSOLUTE;
        qreal ratio = scene()->width() / 100;
        if(_myData->isPositionValid())
        {
            _myData->x *= ratio;
            _myData->y *= ratio;
        }
    }
}
