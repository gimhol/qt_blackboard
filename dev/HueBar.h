#ifndef HUEBAR_H
#define HUEBAR_H

#include <QWidget>

class HueBar : public QWidget
{
    Q_OBJECT
    qreal _hue = 1;

public:
    explicit HueBar(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;
signals:
    void hueChanged(int hue);

public slots:
    void setHue(int hue);
};

#endif // HUEBAR_H
