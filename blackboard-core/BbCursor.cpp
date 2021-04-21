#include "BbCursor.h"
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>

class BbPointerPrivate
{
public:
    BbCursor::Data data;
    QPixmap final;
    QPoint position;
    QPoint point;
};

QPixmap BbCursor::createPointerPixmap(
        const QPixmap &pixmap, const QPointF &pixmapAnchor,
        const QFont &font, const QString &text, const QPointF &textAnchor)
{
    QPainter painter;
    auto pixmapLWidth = 0;
    auto pixmapRWidth = 0;
    auto pixmapTHeight = 0;
    auto pixmapBHeight = 0;

    auto textLWidth = 0;
    auto textRWidth = 0;
    auto textTHeight = 0;
    auto textBHeight = 0;

    if(!pixmap.isNull()){
        auto pixmapWidth  = pixmap.width();
        auto pixmapHeight = pixmap.height();
        pixmapLWidth = pixmapAnchor.x() * pixmapWidth;
        pixmapRWidth = pixmapWidth - pixmapLWidth;
        pixmapTHeight = pixmapAnchor.y() * pixmapHeight;
        pixmapBHeight = pixmapHeight - pixmapTHeight;
    }

    if(!text.isEmpty()){
        auto fontMetrics = QFontMetrics(font);
        auto rect = fontMetrics.boundingRect(text);
        auto textWidth = rect.width();
        auto textHeight = rect.height();

        textLWidth = textAnchor.x() * textWidth;
        textRWidth = textWidth - textLWidth;
        textTHeight = textAnchor.y() * textHeight;
        textBHeight = textHeight - textTHeight;
    }

    auto width = qMax(pixmapLWidth,textLWidth)+qMax(pixmapRWidth,textRWidth);
    auto height = qMax(pixmapTHeight,textTHeight)+qMax(pixmapBHeight,textBHeight);
    if(0 >= width * height)
        return QPixmap();

    auto final = QPixmap(width, height);
    final.fill(Qt::transparent);
    painter.begin(&final);
    if(!pixmap.isNull()){
        painter.drawPixmap(
                    qMax(textLWidth-pixmapLWidth,0),
                    qMax(textTHeight-pixmapTHeight,0),pixmap);
    }
    if(!text.isEmpty()){
        painter.setFont(font);
        painter.drawText(
                    QRect(
                        qMax(pixmapLWidth-textLWidth,0),
                        qMax(pixmapTHeight-textTHeight,0),
                        textLWidth+textRWidth,
                        textTHeight+textBHeight),
                    Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap,
                    text);
    }
    painter.end();
    return final;
}

BbCursor::BbCursor(QWidget *parent) :
    QWidget(parent),
    dptr(new BbPointerPrivate)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setDisabled(true);
}

BbCursor::~BbCursor()
{
    delete dptr;
}

BbCursor *BbCursor::setPixmap(const QPixmap &pixmap)
{
    dptr->data.pixmap = pixmap;
    updateSelf();
    return this;
}

BbCursor *BbCursor::setText(const QString &text)
{
    dptr->data.text = text;
    updateSelf();
    return this;
}

BbCursor *BbCursor::setPixmapAnchor(const QPointF &pixmapAnchor)
{
    dptr->data.pixmapAnchor = pixmapAnchor;
    updateSelf();
    return this;
}

BbCursor *BbCursor::setTextAnchor(const QPointF &textAnchor)
{
    dptr->data.textAnchor = textAnchor;
    updateSelf();
    return this;
}

const QFont &BbCursor::font() const
{
    return dptr->data.font;
}

const QPixmap &BbCursor::pixmap() const
{
    return dptr->data.pixmap;
}

const QString &BbCursor::text() const
{
    return dptr->data.text;
}

const QPointF &BbCursor::pixmapAnchor() const
{
    return dptr->data.pixmapAnchor;
}

const QPointF &BbCursor::textAnchor() const
{
    return dptr->data.textAnchor;
}

BbCursor *BbCursor::setHotAnchor(const QPointF &hotPoint)
{
    dptr->data.hotAnchor = hotPoint;
    pointTo(dptr->position);
    return this;
}

const QPointF &BbCursor::hotAnchor() const
{
    return dptr->data.hotAnchor;
}

BbCursor *BbCursor::pointTo(const int &x, const int &y)
{
    dptr->position.rx() = x;
    dptr->position.ry() = y;
    dptr->point.rx() = x - dptr->data.hotAnchor.rx() * width();
    dptr->point.ry() = y - dptr->data.hotAnchor.ry() * height();
    move(dptr->point.rx(), dptr->point.ry());
    return this;
}

BbCursor *BbCursor::pointTo(const QPoint &position)
{
    return pointTo(position.x(),position.y());
}

const QPoint &BbCursor::point() const
{
    return dptr->point;
}

void BbCursor::updateSelf()
{
    dptr->final = createPointerPixmap(dptr->data.pixmap,dptr->data.pixmapAnchor,dptr->data.font,dptr->data.text,dptr->data.textAnchor);
    auto p = palette();
    p.setBrush(QPalette::Window, dptr->final);
    setPalette(p);
    resize(dptr->final.size());
    pointTo(dptr->position);
    update();
}

#ifdef QT_DEBUG
void BbCursor::moveEvent(QMoveEvent *event)
{
    qDebug() << "cursor move to: " << event->pos();
}

void BbCursor::resizeEvent(QResizeEvent *event)
{
    qDebug() << "cursor resize to: " << event->size();
}
#endif

void BbCursor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,dptr->final);
}

BbCursor::Data::Data(const BbCursor::Data &other):
    pixmap(other.pixmap),
    text(other.text),
    pixmapAnchor(other.pixmapAnchor),
    textAnchor(other.textAnchor),
    hotAnchor(other.hotAnchor),
    font(other.font){}

BbCursor::Data &BbCursor::Data::operator=(const BbCursor::Data &other)
{
    pixmap = (other.pixmap);
    text = (other.text);
    pixmapAnchor = (other.pixmapAnchor);
    textAnchor = (other.textAnchor);
    hotAnchor = (other.hotAnchor);
    font = (other.font);
    return *this;
}
