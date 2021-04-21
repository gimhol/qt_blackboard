#ifndef BBCURSOR_H
#define BBCURSOR_H
#include "BbHeader.h"
#include <QWidget>
class NSB_BLACKBOARD_EXPORT BbCursor : public QWidget
{
    Q_OBJECT
    friend class BbPointerPrivate;
public:
    class Data {
    public:
        QPixmap pixmap;
        QString text;
        QPointF pixmapAnchor = QPointF(1,1);
        QPointF textAnchor = QPointF(0,0);
        QPointF hotAnchor = QPointF(0.5, 0.5);
        QFont font;
        Data(){}
        Data(const Data &other);
        Data& operator=(const Data &other);
    };
    static QPixmap createPointerPixmap(
            const QPixmap &pixmap,
            const QPointF &pixmapAnchor,
            const QFont &font,
            const QString &text,
            const QPointF &textAnchor);

    explicit BbCursor(QWidget *parent = nullptr);

    ~BbCursor() override;

    BbCursor *setPixmap(const QPixmap & pixmap);

    BbCursor *setText(const QString &text);

    BbCursor *setPixmapAnchor(const QPointF &pixmapAnchor);

    BbCursor *setTextAnchor(const QPointF &textAnchor);

    BbCursor *setHotAnchor(const QPointF &hotPoint);

    BbCursor *pointTo(const int &x, const int &y);

    BbCursor *pointTo(const QPoint &position);

    const QFont &font() const;

    const QPixmap &pixmap() const;

    const QString &text() const;

    const QPointF &pixmapAnchor() const;

    const QPointF &textAnchor() const;

    const QPointF &hotAnchor() const;

    const QPoint &point() const;

protected:
    void updateSelf();

#ifdef QT_DEBUG
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
#endif
    void paintEvent(QPaintEvent *event) override;

    BbPointerPrivate *dptr;
};

#endif // BBCURSOR_H
