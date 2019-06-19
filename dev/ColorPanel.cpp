#include "ColorPanel.h"
#include "ui_ColorPanel.h"

ColorPanel::ColorPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorPanel)
{
    ui->setupUi(this);
    connect(ui->hueBar,&HueBar::hueChanged,ui->svZone,&SaturationValueZone::setHue);
    connect(ui->hueBar,&HueBar::hueChanged,ui->alphaBar,&AlphaBar::setHue);
    connect(ui->svZone,&SaturationValueZone::saturationValueChanged,ui->alphaBar,&AlphaBar::setSaturationValue);

    connect(ui->hueBar,&HueBar::hueChanged,this,&ColorPanel::setHue);
    connect(ui->alphaBar,&AlphaBar::alphaChanged,this,&ColorPanel::setAlpha);
    connect(ui->svZone,&SaturationValueZone::saturationValueChanged,this,&ColorPanel::setSaturationValue);

    connect(this,&ColorPanel::colorChanged,ui->colorDisplay,&ColorDisplayer::setColor);
}

ColorPanel::~ColorPanel()
{
    delete ui;
}

void ColorPanel::setColor(const QColor &color)
{
    _color = color.toHsv();
    _alpha = _color.alpha();
    _saturation = _color.hsvSaturation();
    _hue = _color.hsvHue();
    _value = _color.value();

    ui->hueBar->setHue(_hue);
    ui->svZone->setSaturationValue(_saturation,_value);
    ui->svZone->setHue(_hue);
\
    ui->alphaBar->setAlpha(_alpha);
    ui->alphaBar->setHue(_hue);
    ui->alphaBar->setSaturationValue(_saturation,_value);

    ui->colorDisplay->setColor(color);

}

void ColorPanel::setAlpha(int alpha)
{
    _alpha = alpha;
    _color = QColor::fromHsv(_hue,_saturation,_value,_alpha);
    emit colorChanged(_color);
}

void ColorPanel::setSaturationValue(int saturation, int value)
{
    _saturation = saturation;
    _value = value;
    _color = QColor::fromHsv(_hue,_saturation,_value,_alpha);
    emit colorChanged(_color);
}

void ColorPanel::setHue(int hue)
{
    _hue = hue;
    _color = QColor::fromHsv(_hue,_saturation,_value,_alpha);
    emit colorChanged(_color);
}
