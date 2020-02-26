#include "BbPointer.h"

#include <QPainter>
#include <QVariant>
class BbPointerPrivate
{
public:
    QPixmap pixmap;
};

BbPointer::BbPointer(QWidget *parent) :
    QWidget(parent),
    _private(new BbPointerPrivate)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setDisabled(true);
}

BbPointer::~BbPointer()
{
    delete _private;
}

void BbPointer::setPixmap(const QPixmap &pixmap)
{
    _private->pixmap = pixmap;
    resize(pixmap.size());
    update();
}

void BbPointer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(_private->pixmap.rect(),_private->pixmap);
}
