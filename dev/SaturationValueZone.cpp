#include "SaturationValueZone.h"

#include <QPainter>
#include <QMouseEvent>
SaturationValueZone::SaturationValueZone(QWidget * parent):QWidget(parent)
{

}

void SaturationValueZone::setHue(int hue)
{
    _hue = hue;
    update();
}

void SaturationValueZone::setSaturationValue(int saturation, int value)
{
    _cursorPos.setX((255-saturation)/255.0);
    _cursorPos.setY((255-value)/255.0);
    update();
}

void SaturationValueZone::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QColor baseColor = QColor::fromHsv(_hue,255,255);
    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor);
    painter.drawRect(0,0,width()-1,height()-1);

    QLinearGradient saturation(0,0,width(),height());
    saturation.setStart(0,0);
    saturation.setFinalStop(width()-1,0);
    saturation.setColorAt(1,QColor(255,255,255,255));
    saturation.setColorAt(0,QColor(255,255,255,0));
    painter.setBrush(saturation);
    painter.drawRect(0,0,width()-1,height()-1);

    QLinearGradient value(0,0,width(),height());
    value.setStart(0,0);
    value.setFinalStop(0,height()-1);
    value.setColorAt(0,QColor(0,0,0,0));
    value.setColorAt(1,QColor(0,0,0,255));
    painter.setBrush(value);
    painter.drawRect(0,0,width()-1,height()-1);

    auto x = _cursorPos.x()*width();
    auto y = _cursorPos.y()*height();

    painter.setPen(Qt::black);
    painter.setBrush(Qt::white);
    painter.drawEllipse(x-7,y-7,14,14);

    QColor color = QColor::fromHsv(_hue,255*(1-_cursorPos.x()),255*(1-_cursorPos.y()));
    painter.setBrush(color);
    painter.drawEllipse(x-5,y-5,10,10);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0,0,width()-1,height()-1);
}

void SaturationValueZone::mousePressEvent(QMouseEvent *event)
{
    _picking = true;
    qreal x = event->localPos().x() / width();
    qreal y = event->localPos().y() / height();
    x = x < 0 ? 0 : x > 1 ? 1: x;
    y = y < 0 ? 0 : y > 1 ? 1: y;
    _cursorPos.setX(x);
    _cursorPos.setY(y);
    update();
    saturationValueChanged(255*(1-_cursorPos.x()),255*(1-_cursorPos.y()));
}

void SaturationValueZone::mouseMoveEvent(QMouseEvent *event)
{
    qreal x = event->localPos().x() / width();
    qreal y = event->localPos().y() / height();
    x = x < 0 ? 0 : x > 1 ? 1: x;
    y = y < 0 ? 0 : y > 1 ? 1: y;
    _cursorPos.setX(x);
    _cursorPos.setY(y);
    update();
    saturationValueChanged(255*(1-_cursorPos.x()),255*(1-_cursorPos.y()));

}

void SaturationValueZone::mouseReleaseEvent(QMouseEvent *event)
{
    _picking = false;
    update();
    saturationValueChanged(255*(1-_cursorPos.x()),255*(1-_cursorPos.y()));
}
