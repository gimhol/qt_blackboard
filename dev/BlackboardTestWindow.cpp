#include "BbItemPen.h"
#include "BbItemRect.h"
#include "BbItemStraight.h"
#include "BbItemText.h"
#include "BbItemData.h"
#include "BbItemEllipse.h"
#include "BbItemTriangle.h"
#include "BlackboardTestWindow.h"
#include "ColorPanel.h"
#include "ui_BlackboardTestWindow.h"
#include "BbItemImage.h"
#include <QDebug>
#include <QFileDialog>
#include <QDateTime>
#include <QButtonGroup>
#include <QMutex>
#include <BbItemImageData.h>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHostAddress>
#include <QMenu>
#include <QAction>
#include <QPointer>
#include <QHostAddress>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static QNetworkAccessManager *networkManager()
{
    static QNetworkAccessManager *ret = nullptr;
    if(!ret)
    {
        ret = new QNetworkAccessManager();
    }
    return ret;
};

class Factory: public BbFactory
{
public:
    Factory(QWidget *window):
        _window(window){}

    QString makeItemId() override
    {
        return _window->windowTitle()+BbFactory::makeItemId();
    }
private:
    QWidget *_window;
};


BlackboardTestWindow::BlackboardTestWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlackboardTestWindow),
    _me(new BlackboardClient(this))
{
    ui->setupUi(this);

    QButtonGroup * buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->picker);
    buttonGroup->addButton(ui->pen);
    buttonGroup->addButton(ui->text);
    buttonGroup->addButton(ui->pointer);
    buttonGroup->addButton(ui->straight);
    buttonGroup->addButton(ui->rect);
    buttonGroup->addButton(ui->ellipse);
    buttonGroup->addButton(ui->triangle);

    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Pointer);
        blackboard->setFactory(new Factory(this));
    }

    penSettings = blackboard()->toolSettings<BbItemPenData>(BBTT_Pen);
    straightSettings = blackboard()->toolSettings<BbItemStraightData>(BBTT_Straight);
    textSettings = blackboard()->toolSettings<BbItemTextData>(BBTT_Text);
    rectSettings = blackboard()->toolSettings<BbItemRectData>(BBTT_Rectangle);
    ellipseSettings = blackboard()->toolSettings<BbItemEllipseData>(BBTT_Ellipse);
    triangleSettings = blackboard()->toolSettings<BbItemTriangleData>(BBTT_Triangle);

    ui->penWeight->setValue(int(penSettings->weight() * 100));
    ui->penColor->setColor(penSettings->pen.color());
    connect(ui->penColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){penSettings->setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->penColor,&ColorDisplayer::setColor);
        }
        cp->setColor(penSettings->pen.color());
        cp->show();
    });

    ui->straightWeight->setValue(int(straightSettings->weight() * 100));
    ui->straightColor->setColor(straightSettings->pen.color());
    connect(ui->straightColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){straightSettings->pen.setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->straightColor,&ColorDisplayer::setColor);
        }
        cp->setColor(straightSettings->pen.color());
        cp->show();
    });



    ui->textWeight->setValue(int(textSettings->pointWeight() * 100));
    ui->textColor->setColor(textSettings->color);
    connect(ui->textColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){textSettings->color = color;});
            connect(cp,&ColorPanel::colorChanged,ui->textColor,&ColorDisplayer::setColor);
        }
        cp->setColor(textSettings->color);
        cp->show();
    });


    ui->rectWeight->setValue(int(rectSettings->weight() * 100));
    ui->rectPenColor->setColor(rectSettings->pen.color());
    ui->rectBrushColor->setColor(rectSettings->brush.color());
    connect(ui->rectPenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){rectSettings->pen.setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->rectPenColor,&ColorDisplayer::setColor);
        }
        cp->setColor(rectSettings->pen.color());
        cp->show();
    });
    connect(ui->rectBrushColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){rectSettings->brush.setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->rectBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(rectSettings->brush.color());
        cp->show();
    });

    ui->ellipseWeight->setValue(int(ellipseSettings->weight() * 100));
    ui->ellipsePenColor->setColor(ellipseSettings->pen.color());
    ui->ellipseBrushColor->setColor(ellipseSettings->brush.color());
    connect(ui->ellipsePenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){ellipseSettings->pen.setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->ellipsePenColor,&ColorDisplayer::setColor);
        }
        cp->setColor(ellipseSettings->pen.color());
        cp->show();
    });
    connect(ui->ellipseBrushColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){ellipseSettings->brush.setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->ellipseBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(ellipseSettings->brush.color());
        cp->show();
    });

    ui->triangleWeight->setValue(int(triangleSettings->weight() * 100));
    ui->trianglePenColor->setColor(triangleSettings->pen.color());
    ui->triangleBrushColor->setColor(triangleSettings->brush.color());
    connect(ui->trianglePenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){triangleSettings->pen.setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->trianglePenColor,&ColorDisplayer::setColor);
        }
        cp->setColor(rectSettings->pen.color());
        cp->show();
    });
    connect(ui->triangleBrushColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),
                    [&](const QColor &color){triangleSettings->brush.setColor(color);});
            connect(cp,&ColorPanel::colorChanged,ui->triangleBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(triangleSettings->brush.color());
        cp->show();
    });

    connect(ui->graphicsView,&Blackboard::itemSelected,[&](IItemIndex *index, bool selected){
        if(selected)
            ui->textBrowser->append(QString(QStringLiteral("选择: %1")).arg(index->id()));
    });

    QPixmap pm(5,5);
    pm.fill("red");
    ui->graphicsView->setCanvasSize(ui->graphicsView->width(),
                                    ui->graphicsView->width());
    ui->graphicsView->setPointerPixmap(pm);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _me->setObjectName("window");
    _me->connectToHost(QHostAddress::LocalHost,9527);

    connect(_me,&BlackboardClient::msgRead,this,&BlackboardTestWindow::onMeMsgRead);
    connect(ui->graphicsView,&Blackboard::scrolled,this,[&](float x, float y){
        _me->send(MsgTypeBlackboardScrolled,sizeof(int)*2)
                << static_cast<int>(x)
                << static_cast<int>(y);
    });

    auto onPointerShown = [&](QPoint localPoint){
        QJsonObject jobj;
        jobj["id"] = "remote_pointer";
        jobj["x"] = localPoint.x();
        jobj["y"] = localPoint.y();
        _me->send(MsgTypeBlackboardPointerShown,QJsonDocument(jobj).toBinaryData());
    };
    auto onPointerMoving = [&](QPoint localPoint){
        QJsonObject jobj;
        jobj["id"] = "remote_pointer";
        jobj["x"] = localPoint.x();
        jobj["y"] = localPoint.y();
        _me->send(MsgTypeBlackboardPointerMoving,QJsonDocument(jobj).toBinaryData());
    };
    auto onPointerMoved = [&](QPoint localPoint){
        QJsonObject jobj;
        jobj["id"] = "remote_pointer";
        jobj["x"] = localPoint.x();
        jobj["y"] = localPoint.y();
        _me->send(MsgTypeBlackboardPointerMoved,QJsonDocument(jobj).toBinaryData());
    };
    auto onPointerHidden = [&](QPoint){
        QJsonObject jobj;
        jobj["id"] = "remote_pointer";
        _me->send(MsgTypeBlackboardPointerHidden,QJsonDocument(jobj).toBinaryData());
    };

    connect(ui->graphicsView,&Blackboard::pointerShown,this,onPointerShown);
    connect(ui->graphicsView,&Blackboard::pointerMoving,this,onPointerMoving);
    connect(ui->graphicsView,&Blackboard::pointerMoved,this,onPointerMoved);
    connect(ui->graphicsView,&Blackboard::pointerHidden,this,onPointerHidden);
    connect(ui->graphicsView,&Blackboard::cursorShown,this,onPointerShown);
    connect(ui->graphicsView,&Blackboard::cursorMoving,this,onPointerMoving);
    connect(ui->graphicsView,&Blackboard::cursorMoved,this,onPointerMoved);
    connect(ui->graphicsView,&Blackboard::cursorHidden,this,onPointerHidden);
    connect(ui->graphicsView,&Blackboard::itemChanged,this,[=](BBItemEventType eventType,IItemIndex *index){
        switch(eventType){
        case BBIET_penDown:{
            auto pen = dynamic_cast<BbItemPen*>(index);
            QJsonObject jobj;
            jobj["id"] = pen->id();
            jobj["z"] = pen->z();
            jobj["color"] = int(pen->color().rgba());
            jobj["weight"] = pen->weight();
            auto dot = pen->changed()->first();
            jobj["x"] = qreal(dot.x())/ui->graphicsView->canvasWidth();
            jobj["y"] = qreal(dot.y())/ui->graphicsView->canvasWidth();
            _me->send(eventType,QJsonDocument(jobj).toBinaryData());
            break;
        }
        case BBIET_penDraw:{
            auto pen = dynamic_cast<BbItemPen*>(index);
            QJsonObject jobj;
            jobj["id"] = pen->id();
            QJsonArray dots;
            for(auto p: *pen->changed()){
                dots << (p.x()/ui->graphicsView->canvasWidth())
                     << (p.y()/ui->graphicsView->canvasWidth());
            }
            jobj["dots"] = dots;
            _me->send(eventType,QJsonDocument(jobj).toBinaryData());
            break;
        }
        case BBIET_penStraighting:{
            auto pen = dynamic_cast<BbItemPen*>(index);
            QJsonObject jobj;
            jobj["id"] = pen->id();
            jobj["x"] = qreal(pen->straightTo().x())/ui->graphicsView->canvasWidth();
            jobj["y"] = qreal(pen->straightTo().y())/ui->graphicsView->canvasWidth();
            _me->send(eventType,QJsonDocument(jobj).toBinaryData());
            break;
        }
        case BBIET_penDone:{
            QJsonObject jobj;
            jobj["id"] = index->id();
            _me->send(eventType,QJsonDocument(jobj).toBinaryData());
            break;
        }
        default:
            break;
        }
    });
}

BlackboardTestWindow::~BlackboardTestWindow()
{
    delete ui;
}

static QByteArray ba;
void BlackboardTestWindow::start()
{
    BlackboardTestWindow *win0 = new BlackboardTestWindow();
    win0->setAttribute(Qt::WA_DeleteOnClose);
    win0->setWindowTitle(QStringLiteral("测试窗口0"));
    win0->show();
    win0->move(0,0);
    win0->blackboard()->readByteArray(ba);
    win0->ui->blackboardHeight->setValue(win0->blackboard()->width());

    BlackboardTestWindow *win1 = new BlackboardTestWindow();
    win1->setAttribute(Qt::WA_DeleteOnClose);
    win1->setWindowTitle(QStringLiteral("测试窗口1"));
    win1->show();
    win1->move(win0->width(),0);
    win1->ui->blackboardHeight->setValue(win1->blackboard()->width());

    bindBlackboard(win0->blackboard(),win1->blackboard());
    bindBlackboard(win1->blackboard(),win0->blackboard());

//    QTimer::singleShot(5000,win0,[&,win0](){
//        ba.clear();
//        win0->blackboard()->saveByteArray(ba);
//        win0->close();
//        BlackboardTestWindow::start();
//    });

}

void BlackboardTestWindow::bindBlackboard(Blackboard *blackboard0, Blackboard *blackboard1)
{
#define TOINT(_NUM_) static_cast<int>(_NUM_)
    auto copyFullItem = [&](Blackboard *blackboard1, IStreamWR *itemWR,IItemIndex *itemIndex)
    {
        QByteArray byteArray;
        QDataStream dataStream(&byteArray,QIODevice::WriteOnly);
        dataStream << itemIndex->toolType();
        itemWR->writeStream(dataStream);

        QByteArray byteArray2(byteArray);
        QDataStream dataStream2(&byteArray2,QIODevice::ReadOnly);
        blackboard1->scene()->readItemFromStream(dataStream2);
    };

    auto itemMoving = [blackboard1](QGraphicsItem *item)
    {
        if(item)
        {
            auto index = dynamic_cast<IItemIndex*>(item);
            if(index)
            {
                auto copy = blackboard1->find<IItemIndex>(index->id());
                if(copy)
                {
                    copy->moveToPosition(item->pos());
                }
            }
        }
    };
    auto itemMoved = [itemMoving](QGraphicsItem *item)
    {
        itemMoving(item);
        auto index = dynamic_cast<IItemIndex*>(item);
        if(index){
            qDebug() << QString("move item %1 from (%2,%3) to (%4,%5).").
                        arg(index->id()).
                        arg(index->data()->prevX).arg(index->data()->prevY).
                        arg(index->data()->x).arg(index->data()->y);
        }
    };
    auto itemDelete = [blackboard1](QGraphicsItem *item){
        if(item)
        {
            auto index = dynamic_cast<IItemIndex*>(item);
            blackboard1->remove(index->id());
        }
    };
    auto itemPaste = [&,blackboard1](QGraphicsItem *item){
        if(item)
        {
            auto index = dynamic_cast<IItemIndex*>(item);
            if(index)
            {
                copyFullItem(blackboard1,dynamic_cast<IStreamWR*>(index),index);
            }
        }
    };
    auto imageAdded = [blackboard1](BbItemImage *item){
        if(item)
        {
            auto data = dynamic_cast<BbItemImageData*>(item->data());
            auto copy = new BbItemImage();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZ(data->z);
                copy->resize(data->width,data->height);
                if(data->isPositionValid()){
                    copy->moveToPosition(data->x,data->y);
                    copy->updatePrevPosition();
                }
                copy->setUrl(data->url);
                copy->setPath(data->path);
                copy->setPixmap(data->pixmap);
                copy->setId(data->lid);
                copy->updatePrevSize();
                copy->update();
                loadImage(copy);
            }
        }
    };
    auto imageResizing = [blackboard1](BbItemImage *item){
        if(item)
        {
            auto data = dynamic_cast<BbItemImageData*>(item->data());
            auto copy = blackboard1->find<BbItemImage>(item->id());
            if(copy)
            {
                copy->resize(data->width,data->height);
                if(data->isPositionValid()){
                    copy->moveToPosition(data->x,data->y);
                    copy->updatePrevPosition();
                }
                copy->updatePrevSize();
                copy->update();
            }
        }
    };
    auto imageResized = [blackboard1](BbItemImage *item){
        if(item)
        {
            auto data = dynamic_cast<BbItemImageData*>(item->data());
            auto copy = blackboard1->find<BbItemImage>(item->id());
            if(copy)
            {
                copy->resize(data->width,data->height);
                if(data->isPositionValid()){
                    copy->moveToPosition(data->x,data->y);
                    copy->updatePrevPosition();
                }
                copy->updatePrevSize();
                copy->update();
            }
        }
    };
    auto imageHasUrl = [blackboard1](BbItemImage *item){
        qInfo() << "imageHasUrl: " << item->url();
    };
    auto triangleDown = [blackboard1](BbItemTriangle *item)
    {
        if(item)
        {
            auto copy = new BbItemTriangle();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZ(item->z());
                copy->setPenColor(item->penColor());
                copy->setWeight(item->weight());
                copy->setBrushColor(item->brushColor());
                copy->begin(item->point(0));
                copy->setId(item->id());
            }
        }
    };
    auto triangleDraw = [blackboard1](BbItemTriangle *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemTriangle>(item->id());
            if(copy)
            {
                copy->draw(item->point(1+item->step()));
            }
        }
    };
    auto triangleDone = [blackboard1](BbItemTriangle *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemTriangle>(item->id());
            if(copy)
            {
                copy->done();
            }
        }
    };
    auto ellipseDown =[blackboard1](BbItemEllipse *item){
        if(item)
        {
            auto copy = new BbItemEllipse();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZ(item->z());
                copy->setPenColor(item->penColor());
                copy->setWeight(item->weight());
                copy->setBrushColor(item->brushColor());
                copy->setId(item->id());

                copy->begin(item->beginPos());
            }
        }
    };
    auto ellipseDraw = [blackboard1](BbItemEllipse *item){
        if(item){ //
            auto data = dynamic_cast<BbItemEllipseData*>(item->data());
            auto copy = blackboard1->find<BbItemEllipse>(data->lid);
            if(copy){
                copy->begin(data->position());
                copy->draw(QPointF(data->x+data->size.width(),
                                   data->y+data->size.height()));
                copy->done();
            }
        }
    };
    auto ellipseDone = [blackboard1](BbItemEllipse *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemEllipse>(item->id());
            if(copy)
            {
                copy->done();
            }
        }
    };
    auto rectDown =[blackboard1](BbItemRect *item){
        if(item)
        {
            auto copy = new BbItemRect();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZ(item->z());
                copy->setPenColor(item->penColor());
                copy->setWeight(item->weight());
                copy->setBrushColor(item->brushColor());
                copy->begin(item->beginPos());
                copy->setId(item->id());
            }
        }
    };
    auto rectDraw = [blackboard1](BbItemRect *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemRect>(item->id());
            if(copy)
            {
                copy->draw(item->dragPos());
            }
        }
    };
    auto rectDone = [blackboard1](BbItemRect *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemRect>(item->id());
            if(copy)
            {
                copy->done();
            }
        }
    };
    auto textAdded = [blackboard1](BbItemText *item){
        if(item)
        {
            auto copy = new BbItemText();
            if(copy)
            {
                copy->setZ(copy->z());
                blackboard1->scene()->add(copy);
                copy->setFont(item->font());
                copy->setColor(item->color());
                copy->moveToPosition(item->pos());
                copy->setId(item->id());
            }
        }
    };
    auto textChanged = [blackboard1](BbItemText *item){
        auto copy = blackboard1->find<BbItemText>(item->id());
        auto data = dynamic_cast<BbItemTextData*>(item->data());
        if(copy)
        {
            copy->setText(item->text());
        }
    };
    auto textDone = [blackboard1](BbItemText *item){
        auto copy = blackboard1->find<BbItemText>(item->id());
        auto data = dynamic_cast<BbItemTextData*>(item->data());
        qDebug() << data->prevText << " > " << data->text;
        if(copy)
        {
            copy->setText(item->text());
        }
    };
    auto straightDown = [blackboard1](BbItemStraight *item){
        if(item)
        {
            auto copy = new BbItemStraight();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZ(item->z());
                copy->setColor(item->color());
                copy->setWeight(item->weight());
                copy->setId(item->id());
                copy->begin(item->a());
            }
        }
    };
    auto straightDraw = [blackboard1](BbItemStraight *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemStraight>(item->id());
            if(copy)
            {
                copy->draw(item->b());
            }
        }
    };
    auto straightDone = [blackboard1](BbItemStraight *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemStraight>(item->id());
            if(copy)
            {
                copy->done();
            }
        }
    };
#define HANDLE_ITEM_EVENT(_EVENT_TYPE_,_ITEM_TYPE_) \
    case BBIET_##_EVENT_TYPE_: \
        _EVENT_TYPE_(dynamic_cast<_ITEM_TYPE_*>(index)); \
        break

    connect(blackboard0,&Blackboard::multipleItemChanged,blackboard1,[=](BBItemEventType eventType,IItemIndex *index){
        while(index)
        {
            auto next = index->next;
            switch(eventType)
            {
                HANDLE_ITEM_EVENT(itemMoving,QGraphicsItem);
                HANDLE_ITEM_EVENT(itemMoved,QGraphicsItem);
                HANDLE_ITEM_EVENT(itemDelete,QGraphicsItem);
                HANDLE_ITEM_EVENT(itemPaste,QGraphicsItem);
                case BBIET_none:
                default:
                    break;
            }
            index = next;
        }
    });
    connect(blackboard0,&Blackboard::itemChanged,blackboard1,[=](BBItemEventType eventType,IItemIndex *index){
        switch(eventType)
        {
            HANDLE_ITEM_EVENT(straightDown,BbItemStraight);
            HANDLE_ITEM_EVENT(straightDraw,BbItemStraight);
            HANDLE_ITEM_EVENT(straightDone,BbItemStraight);
            HANDLE_ITEM_EVENT(textAdded,BbItemText);
            HANDLE_ITEM_EVENT(textChanged,BbItemText);
            HANDLE_ITEM_EVENT(textDone,BbItemText);
            HANDLE_ITEM_EVENT(rectDown,BbItemRect);
            HANDLE_ITEM_EVENT(rectDraw,BbItemRect);
            HANDLE_ITEM_EVENT(rectDone,BbItemRect);
            HANDLE_ITEM_EVENT(ellipseDown,BbItemEllipse);
            HANDLE_ITEM_EVENT(ellipseDraw,BbItemEllipse);
            HANDLE_ITEM_EVENT(ellipseDone,BbItemEllipse);
            HANDLE_ITEM_EVENT(triangleDown,BbItemTriangle);
            HANDLE_ITEM_EVENT(triangleDraw,BbItemTriangle);
            HANDLE_ITEM_EVENT(triangleDone,BbItemTriangle);
            HANDLE_ITEM_EVENT(imageAdded,BbItemImage);
            HANDLE_ITEM_EVENT(imageResizing,BbItemImage);
            HANDLE_ITEM_EVENT(imageResized,BbItemImage);
            HANDLE_ITEM_EVENT(imageHasUrl,BbItemImage);
            case BBIET_none:
            default:
                break;
        }
    });
}

Blackboard *BlackboardTestWindow::blackboard()
{
    return ui->graphicsView;
}

void BlackboardTestWindow::on_pointer_clicked()
{
    blackboard()->setToolType(BBTT_Pointer);
}

void BlackboardTestWindow::on_pen_clicked()
{
     blackboard()->setToolType(BBTT_Pen);
}

void BlackboardTestWindow::on_clear_clicked()
{
    blackboard()->clearItems();
}

void BlackboardTestWindow::on_remove_clicked()
{
    blackboard()->removeSelectedElement();
}

void BlackboardTestWindow::on_picker_clicked()
{
    blackboard()->setToolType(BBTT_Picker);
}

void BlackboardTestWindow::on_straight_clicked()
{
    blackboard()->setToolType(BBTT_Straight);
}

void BlackboardTestWindow::on_text_clicked()
{
    blackboard()->setToolType(BBTT_Text);
}

void BlackboardTestWindow::on_repaint_clicked()
{
    static QMutex mutex;
    mutex.lock();
    for(auto blackboard : findChildren<Blackboard*>())
    {
        QByteArray ba;
        blackboard->saveByteArray(ba);
        blackboard->clearItems();
        blackboard->readByteArray(ba);
    }
    mutex.unlock();
}

void BlackboardTestWindow::on_penWeight_valueChanged(int arg1)
{
    penSettings->setWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_straightWeight_valueChanged(int arg1)
{
    straightSettings->setWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_textWeight_valueChanged(int arg1)
{
    textSettings->setPointWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_rect_clicked()
{
    blackboard()->setToolType(BBTT_Rectangle);
}

void BlackboardTestWindow::on_ellipse_clicked()
{
    blackboard()->setToolType(BBTT_Ellipse);
}

void BlackboardTestWindow::on_ellipseWeight_valueChanged(int arg1)
{
    ellipseSettings->setWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_triangle_clicked()
{
    blackboard()->setToolType(BBTT_Triangle);
}

void BlackboardTestWindow::on_triangleWeight_valueChanged(int arg1)
{
    triangleSettings->setWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_onlineImage_clicked()
{
//    QPixmap pm(100,100);
//    pm.fill(Qt::blue);
//    blackboard()->addPixmapItem(pm);
}

void BlackboardTestWindow::on_copy_clicked()
{
    blackboard()->copyItems();
}

void BlackboardTestWindow::on_paste_clicked()
{
    blackboard()->pasteItems();
}

void BlackboardTestWindow::on_selectedAll_clicked()
{
    blackboard()->setToolType(BBTT_Picker);
    blackboard()->selectedAll();
}

void BlackboardTestWindow::on_rectWeight_valueChanged(int arg1)
{
    rectSettings->setWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_imagePick_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,QStringLiteral("选择图片"),".","*.png;*.jpg");
    if(!fileName.isEmpty())
    {
        blackboard()->addImageItemWithPath(fileName);
    }
}
void BlackboardTestWindow::loadImage(BbItemImage *item)
{
    if(!item)
    {
        return;
    }
    Blackboard *blackboard = item->blackboard();
    if(!blackboard)
    {
        return;
    }
    QString url = item->url();
    if(url.isEmpty() || url.isNull())
    {
        return;
    }
    QString itemId = item->id();
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager()->get(request);
    auto onFinish = [blackboard,itemId,reply]()
    {
        BbItemImage *item = blackboard->find<BbItemImage>(itemId);
        if(item)
        {
            switch(reply->error())
            {
                case QNetworkReply::NoError:
                {
                    QByteArray bytes = reply->readAll();
                    QPixmap pixmap;
                    pixmap.loadFromData(bytes);
                    item->setPixmap(pixmap);
                    item->setText("");
                    item->setProgress(1);
                    break;
                }
                default:
                {
                    item->setText(reply->errorString());
                    item->setProgress(0);
                    break;
                }
            }
        }
    };
    auto onDownloadProgress = [blackboard,itemId](qint64 bytesReceived, qint64 bytesTotal)
    {
        BbItemImage *item = blackboard->find<BbItemImage>(itemId);
        if(item)
        {
            item->setText("downloading");
            item->setProgress(1.0*bytesReceived/bytesTotal);
        }
    };
    connect(reply,&QNetworkReply::finished,blackboard,onFinish);
    connect(reply,&QNetworkReply::downloadProgress,blackboard,onDownloadProgress);
}

void BlackboardTestWindow::on_imageInsert_clicked()
{
    BbItemImage *item = blackboard()->addImageItemWithUrl(
                ui->spinBoxImageWidth->value(),
                ui->spinBoxImageHeight->value(),
                ui->pictureUrl->text());
    loadImage(item);
}

void BlackboardTestWindow::on_blackboardHeight_editingFinished()
{
    auto width = blackboard()->canvasSize().width();
    blackboard()->setCanvasSize(width,ui->blackboardHeight->value());

}



void BlackboardTestWindow::on_btn_set_background_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,u8"选择图片",".","PNG(*.png);;JPEG(*.jpg)");
    if(!fileName.isEmpty())
    {
        QPixmap pm(fileName);
        blackboard()->setBackground(pm);
    }
}

void BlackboardTestWindow::on_btn_add_background_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,u8"选择图片",".","PNG(*.png);;JPEG(*.jpg)");
    if(!fileName.isEmpty())
    {
        QPixmap pm(fileName);
        blackboard()->addBackground(pm);
    }
}

void BlackboardTestWindow::on_btn_lay_backgrounds_clicked()
{
    blackboard()->layBackgrounds();
}

void BlackboardTestWindow::on_btn_clear_background_clicked()
{
    blackboard()->clearBackground();
}

void BlackboardTestWindow::on_btn_make_sure_show_all_backgrouns_clicked()
{
    auto bb = blackboard();
    auto canvasW = bb->canvasWidth();
    auto canvasH = bb->canvasHeight();
    auto bgRect = bb->backgroundRect();
    bb->setCanvasSize(
                (std::max)(canvasW,int(bgRect.right())),
                (std::max)(canvasH,int(bgRect.bottom()))
                );
}
void BlackboardTestWindow::on_btn_remove_one_background_clicked()
{
    auto menu = new QMenu(ui->btn_remove_one_background);
    ui->btn_remove_one_background->setMenu(menu);
    auto bb = blackboard();
    for(auto pair: bb->backgrounds())
    {
        menu->addAction(pair.first);
    }
    connect(menu,&QMenu::triggered,this,[bb](QAction *action){
        bb->removeBackground(action->text());
    });
    connect(menu,&QMenu::aboutToHide,menu,&QObject::deleteLater);
    menu->move(ui->btn_remove_one_background->mapTo(this,QPoint(0,0)));
    menu->show();
}

void BlackboardTestWindow::onMeMsgRead()
{
    switch(_me->msgType()){
    case MsgTypeBlackboardScrolled:{
        int x = 0;
        int y = 0;
        _me->msgBodyReader() >> x >> y;
        ui->graphicsView->setScroll(x,y);
        return;
    }
    case MsgTypeBlackboardPointerShown:
    case MsgTypeBlackboardPointerMoving:
    case MsgTypeBlackboardPointerMoved:{
        auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
        ui->graphicsView->movePointer(jobj["id"].toString(),jobj["x"].toInt(),jobj["y"].toInt());
        return;
    }
    case MsgTypeBlackboardPointerHidden:{
        auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
        ui->graphicsView->hidePointer(jobj["id"].toString());
        return;
    }
    default:break;
    }

    switch(static_cast<BBItemEventType>(_me->msgType())){
    case BBIET_penDown:{
        auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
        BbItemPen *item = new BbItemPen();
        ui->graphicsView->add(item);
        item->setId(jobj["id"].toString());
        item->setZ(jobj["z"].toDouble());
        item->setColor(QColor::fromRgba(uint32_t(jobj["color"].toInt())));
        item->setWeight(item->weight());
        QPointF dot(jobj["x"].toDouble()*ui->graphicsView->canvasWidth(),
                    jobj["y"].toDouble()*ui->graphicsView->canvasWidth());
        item->penDown(dot);
        return;
    }
    case BBIET_penDraw:{
        auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
        BbItemPen *item = ui->graphicsView->find<BbItemPen>(jobj["id"].toString());
        if(!item)
            return;
        auto dots = jobj["dots"].toArray();
        for(auto itr = dots.begin(); itr!=dots.end(); ++itr){

            QPointF dot;
            dot.setX(itr->toDouble()*ui->graphicsView->canvasWidth());
            ++itr;
            dot.setY(itr->toDouble()*ui->graphicsView->canvasWidth());
            item->penDraw(dot);
        }
        return;
    }
    case BBIET_penStraighting:{
        auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
        BbItemPen *item = new BbItemPen();
        ui->graphicsView->add(item);
        QPointF dot(jobj["x"].toDouble()*ui->graphicsView->canvasWidth(),
                    jobj["y"].toDouble()*ui->graphicsView->canvasWidth());
        item->penStraighting(dot);
        break;
    }
    case BBIET_penDone:{
        auto jobj = QJsonDocument::fromBinaryData(_me->msgBody());
        BbItemPen *item = ui->graphicsView->find<BbItemPen>(jobj["id"].toString());
        if(item)
            item->done();
        return;
    }
    default:break;
    }
}
