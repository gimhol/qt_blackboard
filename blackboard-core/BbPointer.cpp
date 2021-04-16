#include "BbPointer.h"

#include <QPainter>
#include <QVariant>
class BbPointerPrivate
{
public:
    QPixmap pixmap;
    QPixmap final;
    QString text;
    QPointF pixmapAnchor;
    QPointF textAnchor;
    QFont font;
};

QPixmap BbPointer::createPointerPixmap(
        const QPixmap &pixmap, const QPointF &pixmapAnchor,
        const QFont &font, const QString &text, const QPointF &textAnchor)
{
    QPainter painter;
    painter.setBrush(Qt::red);
    auto pixmapLWidth = 0;
    auto pixmapRWidth = 0;
    auto pixmapTHeight = 0;
    auto pixmapBHeight = 0;

    auto textLWidth = 0;
    auto textRWidth = 0;
    auto textTHeight = 0;
    auto textBHeight = 0;

    if(pixmap.isNull()){
        auto pixmapWidth  = pixmap.width();
        auto pixmapHeight = pixmap.height();
        pixmapLWidth = pixmapAnchor.x() * pixmapWidth;
        pixmapRWidth = pixmapWidth - pixmapLWidth;
        pixmapTHeight = pixmapAnchor.y() * pixmapHeight;
        pixmapBHeight = pixmapHeight - pixmapTHeight;
    }

    if(!text.isEmpty()){
        painter.setFont(font);

        auto fontMetrics = painter.fontMetrics();
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
    final.save("D://www_ok.png");
    return final;
}

BbPointer::BbPointer(QWidget *parent) :
    QWidget(parent),
    dptr(new BbPointerPrivate)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setDisabled(true);
}

BbPointer::~BbPointer()
{
    delete dptr;
}

BbPointer *BbPointer::setPixmap(const QPixmap &pixmap)
{
    dptr->pixmap = pixmap;
    updatePixmap();
    return this;
}

BbPointer *BbPointer::setText(const QString &text)
{
    dptr->text = text;
    updatePixmap();
    return this;
}

BbPointer *BbPointer::setPixmapAnchor(const QPointF &pixmapAnchor)
{
    dptr->pixmapAnchor = pixmapAnchor;
    updatePixmap();
    return this;
}

BbPointer *BbPointer::setTextAnchor(const QPointF &textAnchor)
{
    dptr->textAnchor = textAnchor;
    updatePixmap();
    return this;
}

const QFont &BbPointer::font() const
{
    return dptr->font;
}

QPixmap BbPointer::pixmap() const
{
    return dptr->pixmap;
}

QString BbPointer::text() const
{
    return dptr->text;
}

QPointF BbPointer::pixmapAnchor() const
{
    return dptr->pixmapAnchor;
}

QPointF BbPointer::textAnchor() const
{
    return dptr->textAnchor;
}

void BbPointer::updatePixmap()
{
    dptr->final = createPointerPixmap(dptr->pixmap,dptr->pixmapAnchor,dptr->font,dptr->text,dptr->textAnchor);

    auto p = palette();
    p.setBrush(QPalette::Window, dptr->final);
    setPalette(p);

    resize(dptr->final.size());
    update();
}

void BbPointer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
}
