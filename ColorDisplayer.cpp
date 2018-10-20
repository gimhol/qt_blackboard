#include "ColorDisplayer.h"

#include <QPainter>

ColorDisplayer::ColorDisplayer(QWidget *parent) : QWidget(parent)
{

}

void ColorDisplayer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    int tbw = 3;
    for(int y = 0; (y*tbw*2) < height(); ++y)
    {
        for(int x = 0; (x*tbw*2) < width(); ++x)
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::lightGray);
            painter.drawRect(x*tbw*2,y*tbw*2,tbw,tbw);
            painter.drawRect(x*tbw*2+tbw,y*tbw*2+tbw,tbw,tbw);
            painter.setBrush(Qt::white);
            painter.drawRect(x*tbw*2+tbw,y*tbw*2,tbw,tbw);
            painter.drawRect(x*tbw*2,y*tbw*2+tbw,tbw,tbw);
        }
    }
    painter.setPen(Qt::black);
    painter.setBrush(_color);
    painter.drawRect(0,0,width()-1,height()-1);
}

void ColorDisplayer::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit clicked();
}

void ColorDisplayer::setColor(const QColor &color)
{
    _color = color;
    update();
}
