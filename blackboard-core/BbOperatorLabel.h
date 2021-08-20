#ifndef BBOPERATORLABEL_H
#define BBOPERATORLABEL_H

#include "BbHeader.h"
#include <QWidget>

class NSB_BLACKBOARD_EXPORT BbOperatorLabel : public QWidget
{
    Q_OBJECT
    friend class BbOperatorLabelPrivate;
public:
    BbOperatorLabel(QWidget *parent);

    void setText(const QString &text);
    const QString &text() const;

    void setAnchor(const float &x,const float &y);
    void setOffset(const int &offsetX, const int &offsetY);
    void setShowDuration(int msec = -1);
    void setTargetPosition(const QPoint &point);
    void setFollowZone(const float &x, const float &y,int widget,int height);
    void setOperationCursor(const QPixmap &pixmap,const QPointF &hotPoint);
protected:
    //    void paintEvent(QPaintEvent *event) override;
   // bool eventFilter(QObject *watched, QEvent *event) override;
    bool follow();
    QPoint followPosition() const;
    void followerShowZonePosition(QPoint &point) const;
private:
    void setupUi();
    BbOperatorLabelPrivate *dptr;
    bool _showZoneisExist;
};

#endif // BBOPERATORLABEL_H
