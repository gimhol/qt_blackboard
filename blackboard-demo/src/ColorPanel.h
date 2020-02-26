#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <QWidget>

namespace Ui {
    class ColorPanel;
}

class ColorPanel : public QWidget
{
    Q_OBJECT
    int _alpha = 255;
    int _hue = 1;
    int _saturation = 255;
    int _value = 255;
    QColor _color;
public:
    explicit ColorPanel(QWidget *parent = nullptr);
    ~ColorPanel();
signals:
    void colorChanged(const QColor & color);

public slots:
    void setColor(const QColor & color);
    void setAlpha(int alpha);
    void setSaturationValue(int saturation,int value);
    void setHue(int hue);
private slots:
    void on_lineEditColor_editingFinished();

private:
    Ui::ColorPanel *ui;
};

#endif // COLORPANEL_H
