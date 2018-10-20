#ifndef SATURATIONVALUEZONE_H
#define SATURATIONVALUEZONE_H

#include <QWidget>

class SaturationValueZone: public QWidget
{
    Q_OBJECT
    int _hue = 0;
    bool _picking = false;
    QPointF _cursorPos;
public:
    explicit SaturationValueZone(QWidget * parent = nullptr);

public slots:
    void setHue(int hue);
    void setSaturationValue(int saturation, int value);
protected:
    void paintEvent(QPaintEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;
signals:
    void saturationValueChanged(int saturation,int value);

};

#endif // SATURATIONVALUEZONE_H
