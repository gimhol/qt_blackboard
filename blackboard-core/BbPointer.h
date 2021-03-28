#ifndef BBPOINTER_H
#define BBPOINTER_H
#include "BbHeader.h"
#include <QWidget>
class BbPointerPrivate;
class NSB_BLACKBOARD_EXPORT BbPointer : public QWidget
{
    Q_OBJECT
public:
    explicit BbPointer(QWidget *parent = nullptr);

    ~BbPointer() override;

    BbPointer *setPixmap(const QPixmap & pixmap);

    BbPointer *setText(const QString &text);

    BbPointer *setPixmapAnchor(const QPointF &pixmapAnchor);

    BbPointer *setTextAnchor(const QPointF &textAnchor);

    void pixmap();

    void text();

    void pixmapAnchor();

    void textAnchor();
protected:
    void updatePixmap();

    virtual void paintEvent(QPaintEvent *event) override;

    BbPointerPrivate *dptr;
};

#endif // BBPOINTER_H
