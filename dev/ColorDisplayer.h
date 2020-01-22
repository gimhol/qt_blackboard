#ifndef COLORDISPLAYER_H
#define COLORDISPLAYER_H

#include <QWidget>

class ColorDisplayer : public QWidget
{
    Q_OBJECT
    QColor _color = Qt::red;
public:
    explicit ColorDisplayer(QWidget *parent = nullptr);

    QColor color();
protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
signals:
    void clicked();

public slots:
    void setColor(const QColor & color);
};

#endif // COLORDISPLAYER_H
