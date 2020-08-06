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

    void setPixmap(const QPixmap & pixmap);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

    BbPointerPrivate *_private;
};

#endif // BBPOINTER_H
