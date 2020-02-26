#include "AlphaBar.h"
#include <QMouseEvent>
#include <qpainter.h>
AlphaBar::AlphaBar(QWidget *parent) : QWidget(parent)
{

}

void AlphaBar::paintEvent(QPaintEvent *event)
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
    QLinearGradient linearGradient(0,0,width(),height());
    linearGradient.setStart(0,0);
    linearGradient.setFinalStop(width(),0);
    linearGradient.setColorAt(1,QColor::fromHsv(_hue,_saturation,_value));
    linearGradient.setColorAt(0,QColor::fromHsv(_hue,_saturation,_value,0));
    painter.setBrush(linearGradient);
    painter.drawRect(0,0,width()-1,height()-1);

    painter.setBrush(Qt::NoBrush);
    painter.drawRect(width()*_alpha/255-3,1,6,height()-3);

    painter.setPen(Qt::white);
    painter.drawRect(width()*_alpha/255-2,2,4,height()-5);
}

void AlphaBar::mousePressEvent(QMouseEvent *event)
{
    qreal x = event->localPos().x() / width();
    _alpha = 255 * (x < 0 ? 0 : x > 1 ? 1: x);
    update();
    alphaChanged(_alpha);
}

void AlphaBar::mouseMoveEvent(QMouseEvent *event)
{
    qreal x = event->localPos().x() / width();
    _alpha = 255 * (x < 0 ? 0 : x > 1 ? 1: x);
    update();
    alphaChanged(_alpha);
}

void AlphaBar::mouseReleaseEvent(QMouseEvent *event)
{
    qreal x = event->localPos().x() / width();
    _alpha = 255 * (x < 0 ? 0 : x > 1 ? 1: x);
    update();
    alphaChanged(_alpha);
}

void AlphaBar::setHue(int hue)
{
    _hue = hue;
    update();
}

void AlphaBar::setSaturationValue(int saturation, int value)
{
    _saturation = saturation;
    _value = value;
    update();
}

void AlphaBar::setAlpha(int alpha)
{
    _alpha = alpha;
    update();
}
