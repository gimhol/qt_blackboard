#include "BbItemText.h"
#include "BbItemTextData.h"
#include "Blackboard.h"
#include "BbScene.h"
#include <QKeyEvent>
#include<QDebug>

BbItemText::BbItemText():
    QGraphicsTextItem(),
    _myData(new BbItemTextData())
{
    init();
}

BbItemText::BbItemText(BbItemData *data):
    QGraphicsTextItem(),
    _myData(dynamic_cast<BbItemTextData*>(data))
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
    setFlag(ItemSendsScenePositionChanges);
    setFont(_myData->font);
    setDefaultTextColor(_myData->color);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void BbItemText::focusOutEvent(QFocusEvent *)
{
    // NOTE: 文本编辑状态下失焦，才去done。
    if(textInteractionFlags() == Qt::TextEditorInteraction)
    {
        blackboard()->revertToolCursor();
        // NOTE: 由于会修改光标样式，所以里需要还原
        setTextInteractionFlags(Qt::NoTextInteraction);
        // NOTE: 如果框选了一些，然后失焦，会导致保持这个状态，所以需要清除选择
        auto cursor = textCursor();
        cursor.clearSelection();
        setTextCursor(cursor);

        // NOTE: 失去焦點后移除選擇狀態
        setSelected(false);

        bbScene()->unsetCurrentItem(this);

        if(isEmpty())
        {
            if(!isRemoved)
            {
                bbScene()->remove(this); // 空白的不要保留，移除本地的。
            }
        }
        else if(_myData->prevText != _myData->text)
        {
            emit blackboard()->itemChanged(BBIET_textDone,this);
            _myData->prevText = _myData->text;
        }
    }
}

// 输入法会走这个。
void BbItemText::inputMethodEvent(QInputMethodEvent *event)
{
    QGraphicsTextItem::inputMethodEvent(event);
    updateContent();
}

// 没输入法时，或其他功能按键会走这个。
void BbItemText::keyPressEvent(QKeyEvent *event)
{
    /*
     * Note：
     *      done会清除焦点，触发focusOutEvent。
     *      在focusOutEvent中，文本为空白时，会移除自己，内存会被释放。
     *      若再调用基类的keyPressEvent，会导致奔溃。
     *      所以执行done后不要调用keyPressEvent。
     */
    const auto key = event->key();
    const auto modifiers = event->modifiers();
    if(Qt::Key_Escape == key ||
       (Qt::Key_Return == key && Qt::ControlModifier == modifiers))
    {
        done();
        return;
    }
    /*
     * NOTE:必須先調用keyPressEvent,
     *      updateContent中獲取的文本内容才會是當前文本内容，
     *      否則會獲取到修改前的。
     */
    QGraphicsTextItem::keyPressEvent(event);
#ifdef QT_DEBUG
    qInfo() << modifiers << key;
    qInfo() << text();
#endif
    updateContent();

    // Note: 移除粘贴来的文本的格式。若还有其他粘贴的路径呢？-Gim
    if(Qt::ControlModifier == modifiers && key == Qt::Key_V){
        auto a = textCursor();
        auto p = a.position();
        // 不能直接QGraphicsTextItem::setPlainText，会带颜色的。
        document()->setPlainText(text());
        a.setPosition(p);
        setTextCursor(a);
    }
}

QVariant BbItemText::itemChange(GraphicsItemChange change,
                                const QVariant &value)
{
    if(ItemSelectedChange == change && value == 0){
        blackboard()->revertToolCursor();
        // NOTE: 由于会修改光标样式，所以里需要还原
        setTextInteractionFlags(Qt::NoTextInteraction);
        // NOTE: 如果框选了一些，然后失焦，会导致保持这个状态，所以需要清除选择
        auto cursor = textCursor();
        cursor.clearSelection();
        setTextCursor(cursor);
    }
    return QGraphicsTextItem::itemChange(change,value);
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
}

void BbItemText::updateContent()
{
    _myData->text = text();
    if(_lastContent != _myData->text)
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
        _lastContent = _myData->text;
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
    absolutize();
    repaint();
}

BbItemData *BbItemText::data()
{
    return _myData;
}

void BbItemText::toolDown(const QPointF &pos)
{
    if(bbScene()->currentItem() == this)
    {
        if(!boundingRect().contains(pos - this->pos()))
        {
            if(!isEmpty())
            {
                done();
            }
            else
            {
                setPos(pos.x(), pos.y() - 0.5 * boundingRect().height());
                _myData->updatePostion(this);
                _myData->updatePrevPostion();
                if(!isEmpty())
                {
                    emit blackboard()->itemChanged(BBIET_itemMoved,this);
                }
            }
        }
    }
    else
    {
        setId(blackboard()->factory()->makeItemId());
        setZ(blackboard()->factory()->makeItemZ());
        updatePrevZ();

        auto settings = blackboard()->toolSettings<BbItemTextData>(BBTT_Text);
        setFont(settings->font);
        setWeight(settings->pointWeight());
        setColor(settings->color);

        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus();
        setPos(pos.x(), pos.y() - 0.5 * boundingRect().height());
        bbScene()->setCurrentItem(this);

        if(!isEmpty())
        {
            emit blackboard()->itemChanged(BBIET_textAdded,this);
        }
    }
}

void BbItemText::toolDraw(const QPointF &)
{
    // do nothing.
}

void BbItemText::toolDone(const QPointF &)
{
    if(bbScene()->toolType() != BBTT_Text)
    {
        clearFocus();
    }
}

void BbItemText::removed()
{
    isRemoved = true;
}

void BbItemText::absolutize()
{
    if(_myData->mode == BbItemData::CM_PERCENTAGE)
    {
        _myData->mode = BbItemData::CM_ABSOLUTE;
        qreal ratio = bbScene()->width() / 100;
        if(_myData->isPositionValid())
        {
            _myData->x *= ratio;
            _myData->y *= ratio;
        }
        if(_myData->isPrevPositionValid())
        {
            _myData->prevX *= ratio;
            _myData->prevY *= ratio;
        }
    }
}

void BbItemText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // NOTE: 通过Picker工具双击文本来编辑时，外部的鼠标样式极可能不是“文本光标”，所以需要这里修改一下
    blackboard()->toToolCursor(BBTT_Text);
    setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsItem::mouseDoubleClickEvent(event);
}
