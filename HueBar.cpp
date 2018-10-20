#include "HueBar.h"

#include <QMouseEvent>
#include <QPainter>

HueBar::HueBar(QWidget *parent) : QWidget(parent)
{

}

void HueBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    QLinearGradient linearGradient(0,0,width(),height());
    linearGradient.setStart(0,0);
    linearGradient.setFinalStop(0,height());
    linearGradient.setColorAt(0/6.0,QColor(255,0,0));
    linearGradient.setColorAt(1/6.0,QColor(255,255,0));
    linearGradient.setColorAt(2/6.0,QColor(0,255,0));
    linearGradient.setColorAt(3/6.0,QColor(0,255,255));
    linearGradient.setColorAt(4/6.0,QColor(0,0,255));
    linearGradient.setColorAt(5/6.0,QColor(255,0,255));
    linearGradient.setColorAt(6/6.0,QColor(255,0,0));
    painter.setBrush(linearGradient);
    painter.drawRect(0,0,width()-1,height()-1);

    painter.setBrush(QColor::fromHsv(_hue,255,255));
    painter.drawRect(1,height()*_hue/360-3,width()-3,6);
}

void HueBar::mousePressEvent(QMouseEvent *event)
{
    qreal y = event->localPos().y() / height();
    _hue = 359 * (y < 0 ? 0 : y > 1 ? 1: y);
    update();
    hueChanged(_hue);
}

void HueBar::mouseMoveEvent(QMouseEvent *event)
{
    qreal y = event->localPos().y() / height();
    _hue = 359 * (y < 0 ? 0 : y > 1 ? 1: y);
    update();
    hueChanged(_hue);
}

void HueBar::mouseReleaseEvent(QMouseEvent *event)
{
    qreal y = event->localPos().y() / height();
    _hue = 359 * (y < 0 ? 0 : y > 1 ? 1: y);
    update();
    hueChanged(_hue);
}

void HueBar::setHue(int hue)
{
    _hue = hue;
    update();
}
