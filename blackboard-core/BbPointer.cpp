#include "BbPointer.h"

#include <QPainter>
#include <QVariant>
class BbPointerPrivate
{
public:
    QPixmap pixmap;
    QString text;
    QPointF pixmapAnchor;
    QPointF textAnchor;
};

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

void BbPointer::updatePixmap()
{
    auto bg = dptr->pixmap;
    auto p = palette();
    p.setBrush(QPalette::Window, bg);
    setPalette(p);

    resize(bg.size());
    update();
}

void BbPointer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(dptr->pixmap.rect(),dptr->pixmap);
}
