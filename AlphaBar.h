#ifndef ALPHABAR_H
#define ALPHABAR_H

#include <QWidget>

class AlphaBar : public QWidget
{
    Q_OBJECT
    int _alpha = 255;
    int _hue = 0;
    int _saturation = 255;
    int _value = 255;
public:
    explicit AlphaBar(QWidget *parent = nullptr);

signals:
    void alphaChanged(int alpha);

protected:
    void paintEvent(QPaintEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

public slots:
    void setHue(int hue);
    void setSaturationValue(int saturation, int value);
    void setAlpha(int alpha);
};

#endif // ALPHABAR_H
