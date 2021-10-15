#include "BbOperatorLabel.h"
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QTimer>
#include "BbCursor.h"
class BbOperatorLabelPrivate
{
public:
    int showTime = 0;
    float anchorX = 0;
    float anchorY = 0;
    QPoint offset;
    QPoint targetPosition;

    QRectF followeZone;
    QPointer<BbCursor> nameCursor;
    QPointer<QLabel> text;
    QPointer<QTimer> timer;
};

BbOperatorLabel::BbOperatorLabel(QWidget *parent) :
    QWidget(parent),
    dptr(new BbOperatorLabelPrivate)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setDisabled(true);
    setupUi();

    _showZoneisExist = false;
    auto p = parentWidget()->rect();
    setFollowZone(0,0,100,100);

    dptr->nameCursor = new BbCursor(parentWidget());

    dptr->timer = new QTimer(this);
    connect(dptr->timer, &QTimer::timeout, this,[&](){
        dptr->timer->stop();
        dptr->nameCursor->setVisible(false);
        this->setVisible(false);
    });
}

void BbOperatorLabel::setText(const QString &text)
{
    dptr->text->setText(text);
    adjustSize();
}

const QString &BbOperatorLabel::text() const
{
    return dptr->text->text();
}

void BbOperatorLabel::setShowDuration(int msec)
{
   dptr->showTime = msec;
   if(dptr->showTime == 0)
   {
       this->setVisible(false);
       dptr->nameCursor->setVisible(false);
   }
   else
   {
       this->setVisible(true);
       dptr->nameCursor->setVisible(true);
       if(dptr->showTime>0)
           dptr->timer->start(dptr->showTime);
   }
}

void BbOperatorLabel::setAnchor(const float &x, const float &y)
{
    dptr->anchorX = x;
    dptr->anchorY = y;
    follow();
}

void BbOperatorLabel::setOffset(const int &offsetX, const int &offsetY)
{
    dptr->offset.setX(offsetX);
    dptr->offset.setY(offsetY);
    follow();
}

void BbOperatorLabel::setTargetPosition(const QPoint &point)
{
    dptr->targetPosition = point;
    follow();    
}

void BbOperatorLabel::setFollowZone(const float &x, const float &y, int widget, int height)
{
     _showZoneisExist = true;
     dptr->followeZone = QRectF(x,y,widget,height);
}

bool BbOperatorLabel::follow()
{
    dptr->nameCursor->pointTo(dptr->targetPosition);

    auto fp = followPosition();
    move(fp);
    updateGeometry();
    raise();
    setShowDuration(dptr->showTime);
    return true;
}

void BbOperatorLabel::setOperationCursor(const QPixmap &pixmap, const QPointF &hotPoint)
{
    dptr->nameCursor->setPixmap(pixmap);
    dptr->nameCursor->setHotAnchor(hotPoint);
}

QPoint BbOperatorLabel::followPosition() const
{
    QPoint ret;
    if(dptr->nameCursor)
    {       
        ret = dptr->nameCursor->mapToParent(QPoint(dptr->nameCursor->width(),0));
    }
    else
    {
        ret.rx() = dptr->targetPosition.x();
        ret.ry() = dptr->targetPosition.y();
    }

    auto w = this->width();
    auto h = this->height();
    ret.rx() -= int(dptr->anchorX * w);
    ret.ry() -= int(dptr->anchorY * h);

    ret += dptr->offset;

    followerShowZonePosition(ret);

    return ret;
}

void BbOperatorLabel::followerShowZonePosition(QPoint &point) const
{
    if(_showZoneisExist)
    {
        auto zone = dptr->followeZone;
        auto w = parentWidget()->width();
        auto h = parentWidget()->height();

        if(point.x() < zone.x()*w/100)

        {
            point.rx() = 0;
        }

        if(point.y() < zone.y()*h/100)
        {
            point.ry() = 0;
        }

        if(point.x()+ this->width() > zone.width()*w/100)
        {
            point.rx() = w -this->width();
        }

        if(point.y()+this->height() > zone.height()*h/100)
        {
            point.ry() = h -this->height() ;
        }
    }
}

//bool BbOperatorLabel::eventFilter(QObject *watched, QEvent *event)
//{
//    if(watched == followed() && event->type() == QEvent::Resize){
//        follow();
//    }else if(watched == followed() && event->type() == QEvent::Move){
//        follow();
//        setOperationShowTime(dptr->showTime);
//    }

//    return QObject::eventFilter(watched,event);
//}

void BbOperatorLabel::setupUi()
{
    dptr->text = new QLabel(this);

    QString style = u8R"(
                    padding:6px;
                    color: #A7A7A7;
                    font-size: 12px;
                    background: #373739;
                    border-top-left-radius:8px;
                    border-top-right-radius:8px;
                    border-bottom-right-radius:8px;
                    border-bottom-left-radius:0px;
                    )";
    dptr->text->setStyleSheet(style);

    auto layout = new QGridLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    layout->addWidget(dptr->text);

    this->setLayout(layout);

//    this->setMinimumHeight(25);
}
