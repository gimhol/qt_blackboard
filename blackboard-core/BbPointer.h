#ifndef BBPOINTER_H
#define BBPOINTER_H
#include "BbHeader.h"
#include <QWidget>
class BbPointerPrivate;

class NSB_BLACKBOARD_EXPORT BbPointer : public QWidget
{
    Q_OBJECT
public:
    static QPixmap createPointerPixmap(
            const QPixmap &pixmap, const QPointF &pixmapAnchor,
            const QFont &font, const QString &text, const QPointF &textAnchor);

    explicit BbPointer(QWidget *parent = nullptr);

    ~BbPointer() override;

    BbPointer *setPixmap(const QPixmap & pixmap);

    BbPointer *setText(const QString &text);

    BbPointer *setPixmapAnchor(const QPointF &pixmapAnchor);

    BbPointer *setTextAnchor(const QPointF &textAnchor);

    const QFont &font() const;

    QPixmap pixmap() const;

    QString text() const;

    QPointF pixmapAnchor() const;

    QPointF textAnchor() const;
protected:
    void updatePixmap();

    virtual void paintEvent(QPaintEvent *event) override;

    BbPointerPrivate *dptr;
};

#endif // BBPOINTER_H
