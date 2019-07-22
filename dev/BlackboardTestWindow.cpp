#include "BbItemPen.h"
#include "BbItemRect.h"
#include "BbItemStraight.h"
#include "BbItemText.h"
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
BlackboardTestWindow::BlackboardTestWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlackboardTestWindow)
{
    ui->setupUi(this);

    QButtonGroup * buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pen);
    buttonGroup->addButton(ui->text);
    buttonGroup->addButton(ui->pointer);
    buttonGroup->addButton(ui->picker);
    buttonGroup->addButton(ui->straight);
    buttonGroup->addButton(ui->rect);
    buttonGroup->addButton(ui->ellipse);

    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Pointer);
        blackboard->scene()->setItemIdGenerator([this](){
            return QString("%1_%2")
                    .arg(windowTitle())
                    .arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
        });
    }

    ui->penWeight->setValue(int(blackboard()->penWeight() * 100));
    ui->penColor->setColor(blackboard()->penColor());
    connect(ui->penColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setPenColor);
            connect(cp,&ColorPanel::colorChanged,ui->penColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->penColor());
        cp->show();
    });

    ui->straightWeight->setValue(int(blackboard()->straightPenWeight() * 100));
    ui->straightColor->setColor(blackboard()->straightPenColor());
    connect(ui->straightColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setStraightPenColor);
            connect(cp,&ColorPanel::colorChanged,ui->straightColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->straightPenColor());
        cp->show();
    });



    ui->textWeight->setValue(int(blackboard()->textPointWeight() * 100));
    ui->textColor->setColor(blackboard()->textColor());
    connect(ui->textColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setTextColor);
            connect(cp,&ColorPanel::colorChanged,ui->textColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->straightPenColor());
        cp->show();
    });


    ui->rectWeight->setValue(int(blackboard()->rectWeight() * 100));
    ui->rectPenColor->setColor(blackboard()->rectPenColor());
    ui->rectBrushColor->setColor(blackboard()->rectBrushColor());
    connect(ui->rectPenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setRectPenColor);
            connect(cp,&ColorPanel::colorChanged,ui->rectPenColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->rectPenColor());
        cp->show();
    });
    connect(ui->rectBrushColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setRectBrushColor);
            connect(cp,&ColorPanel::colorChanged,ui->rectBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->rectBrushColor());
        cp->show();
    });

    ui->ellipseWeight->setValue(int(blackboard()->ellipseWeight() * 100));
    ui->ellipsePenColor->setColor(blackboard()->ellipsePenColor());
    ui->ellipseBrushColor->setColor(blackboard()->ellipseBrushColor());
    connect(ui->ellipsePenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setEllipsePenColor);
            connect(cp,&ColorPanel::colorChanged,ui->ellipsePenColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->ellipsePenColor());
        cp->show();
    });
    connect(ui->ellipseBrushColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setEllipseBrushColor);
            connect(cp,&ColorPanel::colorChanged,ui->ellipseBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->ellipseBrushColor());
        cp->show();
    });

    ui->triangleWeight->setValue(int(blackboard()->triangleWeight() * 100));
    ui->trianglePenColor->setColor(blackboard()->trianglePenColor());
    ui->triangleBrushColor->setColor(blackboard()->triangleBrushColor());
    connect(ui->trianglePenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setTrianglePenColor);
            connect(cp,&ColorPanel::colorChanged,ui->trianglePenColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->trianglePenColor());
        cp->show();
    });
    connect(ui->triangleBrushColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(QStringLiteral("调色"));
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setTriangleBrushColor);
            connect(cp,&ColorPanel::colorChanged,ui->triangleBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->triangleBrushColor());
        cp->show();
    });

    connect(ui->graphicsView,&Blackboard::itemSelected,[&](IItemIndex *index, bool selected){
        if(selected)
        {
            ui->textBrowser->append(QString(QStringLiteral("选择: %1")).arg(index->id()));
        }
    });
}

BlackboardTestWindow::~BlackboardTestWindow()
{
    delete ui;
}

void BlackboardTestWindow::start()
{
    static BlackboardTestWindow win0, win1;
    win0.setWindowTitle(QStringLiteral("测试窗口0"));
    win1.setWindowTitle(QStringLiteral("测试窗口1"));
    win0.show();
    win1.show();
    win0.move(0,0);
    win1.move(win0.width(),0);
    bindBlackboard(win0.blackboard(),win1.blackboard());
    bindBlackboard(win1.blackboard(),win0.blackboard());
}

void BlackboardTestWindow::bindBlackboard(Blackboard *blackboard0, Blackboard *blackboard1)
{
    static QPixmap * pm;
    if(!pm)
    {
        pm = new QPixmap(5,5);
        pm->fill("red");
    }

    blackboard0->setCanvasSize(blackboard0->width(),blackboard0->width()*10);
    blackboard0->setPointerPixmap(*pm);
    connect(blackboard0,&Blackboard::resized,[](float scale){qDebug()<< "resized, scale : " << scale;});



#define TOINT(_NUM_) static_cast<int>(_NUM_)

    connect(blackboard0,&Blackboard::scrolled,[blackboard1](float x, float y){
        blackboard1->setScroll(TOINT(x),TOINT(y));
    });
    connect(blackboard0,&Blackboard::pointerShown,[blackboard1](QPoint localPoint){
        blackboard1->addPointer("remote_pointer", localPoint.x(),localPoint.y());
    });
    connect(blackboard0,&Blackboard::pointerMoved,[blackboard1](QPoint localPoint){
        blackboard1->movePointer("remote_pointer", localPoint.x(),localPoint.y());
    });
    connect(blackboard0,&Blackboard::pointerMoving,[blackboard1](QPoint localPoint){
        blackboard1->movePointer("remote_pointer", localPoint.x(),localPoint.y());
    });
    connect(blackboard0,&Blackboard::pointerHidden,[blackboard1](QPoint localPoint){
        Q_UNUSED(localPoint);
        blackboard1->hidePointer("remote_pointer");
    });

    connect(blackboard0,&Blackboard::cursorShown,[blackboard1](QPoint localPoint){
        blackboard1->addPointer("remote_pointer", localPoint.x(),localPoint.y());
    });
    connect(blackboard0,&Blackboard::cursorMoved,[blackboard1](QPoint localPoint){
        blackboard1->movePointer("remote_pointer", localPoint.x(),localPoint.y());
    });
    connect(blackboard0,&Blackboard::cursorMoving,[blackboard1](QPoint localPoint){
        blackboard1->movePointer("remote_pointer", localPoint.x(),localPoint.y());
    });
    connect(blackboard0,&Blackboard::cursorHidden,[blackboard1](QPoint localPoint){
        Q_UNUSED(localPoint);
        blackboard1->hidePointer("remote_pointer");
    });
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
                auto copy = blackboard1->find<QGraphicsItem>(index->id());
                if(copy)
                {
                    copy->setPos(item->pos());
                }
            }
        }
    };
    auto itemMoved = itemMoving;
    auto itemDelete = [blackboard1](QGraphicsItem *item){
        if(item)
        {
            auto index = dynamic_cast<IItemIndex*>(item);
            if(index)
            {
                auto copy = blackboard1->find<QGraphicsItem>(index->id());
                if(copy)
                {
                    copy->scene()->removeItem(copy);
                }
            }
        }
    };
    auto itemPaste = [&](QGraphicsItem *item){
        if(item)
        {
            auto index = dynamic_cast<IItemIndex*>(item);
            if(index)
            {
                copyFullItem(blackboard1,dynamic_cast<IStreamWR*>(index),index);
            }
        }
    };
    auto penMoved = itemMoving;
    auto penMoving = itemMoving;
    auto imageMoved = itemMoving;
    auto imageMoving = itemMoving;
    auto triangleMoved = itemMoving;
    auto triangleMoving = itemMoving;
    auto ellipseMoved = itemMoving;
    auto ellipseMoving = itemMoving;
    auto rectMoved = itemMoving;
    auto rectMoving = itemMoving;
    auto textMoved = itemMoving;
    auto textMoving = itemMoving;
    auto straightMoved = itemMoving;
    auto straightMoving = itemMoving;
    auto penDelete = itemDelete;
    auto imageDelete = itemDelete;
    auto triangleDelete = itemDelete;
    auto ellipseDelete = itemDelete;
    auto rectDelete = itemDelete;
    auto textDelete = itemDelete;
    auto straightDelete = itemDelete;
    auto imageAdded = [blackboard1](BbItemImage *item){
        if(item)
        {
            auto copy = new BbItemImage();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZValue(copy->zValue());
                copy->setPixmap(item->pixmap());
                copy->setId(item->id());
            }
        }
    };
    auto triangleBegun = [blackboard1](BbItemTriangle *item)
    {
        if(item)
        {
            auto copy = new BbItemTriangle();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZValue(item->zValue());
                copy->setPenColor(item->penColor());
                copy->setWeight(item->weight());
                copy->setBrushColor(item->brushColor());
                copy->begin(item->point(0));
                copy->setId(item->id());
            }
        }
    };
    auto triangleDragged = [blackboard1](BbItemTriangle *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemTriangle>(item->id());
            if(copy)
            {
                copy->drag(item->point(1+item->step()));
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
    auto ellipseBegun =[blackboard1](BbItemEllipse *item){
        if(item)
        {
            auto copy = new BbItemEllipse();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZValue(item->zValue());
                copy->setPenColor(item->penColor());
                copy->setWeight(item->weight());
                copy->setBrushColor(item->brushColor());
                copy->begin(item->beginPos());
                copy->setId(item->id());
            }
        }
    };
    auto ellipseDragged = [blackboard1](BbItemEllipse *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemEllipse>(item->id());
            if(copy)
            {
                copy->drag(item->dragPos());
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
    auto rectBegun =[blackboard1](BbItemRect *item){
        if(item)
        {
            auto copy = new BbItemRect();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZValue(item->zValue());
                copy->setPenColor(item->penColor());
                copy->setWeight(item->weight());
                copy->setBrushColor(item->brushColor());
                copy->begin(item->beginPos());
                copy->setId(item->id());
            }
        }
    };
    auto rectDragged = [blackboard1](BbItemRect *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemRect>(item->id());
            if(copy)
            {
                copy->drag(item->dragPos());
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
                copy->setZValue(copy->zValue());
                blackboard1->scene()->add(copy);
                copy->setFont(item->font());
                copy->setColor(item->color());
                copy->setPos(item->pos());
                copy->setId(item->id());
            }
        }
    };
    auto textChanged = [blackboard1](BbItemText *item){
        auto copy = blackboard1->find<BbItemText>(item->id());
        if(!copy)
        {
            copy->setPlainText(item->toPlainText());
        }
    };
    auto textDone = textChanged;
    auto straightBegun = [blackboard1](BbItemStraight *item){
        if(item)
        {
            auto copy = new BbItemStraight();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZValue(item->zValue());
                copy->setColor(item->color());
                copy->setWeight(item->weight());
                copy->setId(item->id());
                copy->begin(item->a());
            }
        }
    };
    auto straightDragged = [blackboard1](BbItemStraight *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemStraight>(item->id());
            if(copy)
            {
                copy->drag(item->b());
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
    auto penDone = [blackboard1](BbItemPen *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemPen>(item->id());
            if(copy)
            {
                copy->done();
            }
        }
    };
    auto penDraw = [blackboard1](BbItemPen *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemPen>(item->id());
            if(copy)
            {
                for(auto point : *item->changed())
                {
                    copy->penDraw(point);
                }
            }
        }
    };
    auto penStraighting = [blackboard1](BbItemPen *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemPen>(item->id());
            if(copy)
            {
                copy->penStraighting(item->straightTo());
            }
        }
    };
    auto penDown = [blackboard1](BbItemPen *item){
        if(item)
        {
            BbItemPen *copy = new BbItemPen();
            if(copy)
            {
                copy->setZValue(item->zValue());
                copy->setColor(item->color());
                copy->setWeight(item->weight());
                copy->setId(item->id());
                blackboard1->scene()->add(copy);
                copy->penDown(item->changed()->first());
            }
        }
    };




#ifdef BLACKBOARD_ITEM_INDEX_SIGNAL

#define HANDLE_ITEM_EVENT(_EVENT_TYPE_,_ITEM_TYPE_) \
    case BBIET_##_EVENT_TYPE_: \
        _EVENT_TYPE_(dynamic_cast<_ITEM_TYPE_*>(index)); \
        break


    connect(blackboard0,&Blackboard::itemChanged,[=](BBItemEventType eventType,IItemIndex *index){
        switch(eventType)
        {
            HANDLE_ITEM_EVENT(itemMoving,QGraphicsItem);
            HANDLE_ITEM_EVENT(itemMoved,QGraphicsItem);
            HANDLE_ITEM_EVENT(itemDelete,QGraphicsItem);
            HANDLE_ITEM_EVENT(itemPaste,QGraphicsItem);
            HANDLE_ITEM_EVENT(penStraighting,BbItemPen);
            HANDLE_ITEM_EVENT(penDown,BbItemPen);
            HANDLE_ITEM_EVENT(penDraw,BbItemPen);
            HANDLE_ITEM_EVENT(penDone,BbItemPen);
            HANDLE_ITEM_EVENT(straightBegun,BbItemStraight);
            HANDLE_ITEM_EVENT(straightDragged,BbItemStraight);
            HANDLE_ITEM_EVENT(straightDone,BbItemStraight);
            HANDLE_ITEM_EVENT(textAdded,BbItemText);
            HANDLE_ITEM_EVENT(textChanged,BbItemText);
            HANDLE_ITEM_EVENT(textDone,BbItemText);
            HANDLE_ITEM_EVENT(rectBegun,BbItemRect);
            HANDLE_ITEM_EVENT(rectDragged,BbItemRect);
            HANDLE_ITEM_EVENT(rectDone,BbItemRect);
            HANDLE_ITEM_EVENT(ellipseBegun,BbItemEllipse);
            HANDLE_ITEM_EVENT(ellipseDragged,BbItemEllipse);
            HANDLE_ITEM_EVENT(ellipseDone,BbItemEllipse);
            HANDLE_ITEM_EVENT(triangleBegun,BbItemTriangle);
            HANDLE_ITEM_EVENT(triangleDragged,BbItemTriangle);
            HANDLE_ITEM_EVENT(triangleDone,BbItemTriangle);
            HANDLE_ITEM_EVENT(imageAdded,BbItemImage);
            case BBIET_none:
            default:
                break;
        }
    });
#else
    connect(blackboard0,&Blackboard::imageAdded,imageAdded);
    connect(blackboard0,&Blackboard::triangleBegun,triangleBegun);
    connect(blackboard0,&Blackboard::triangleDragged,triangleDragged);
    connect(blackboard0,&Blackboard::triangleDone,triangleDone);
    connect(blackboard0,&Blackboard::ellipseMoved,ellipseMoved);
    connect(blackboard0,&Blackboard::ellipseMoving,ellipseMoving);
    connect(blackboard0,&Blackboard::ellipseBegun,ellipseBegun);
    connect(blackboard0,&Blackboard::ellipseDragged,ellipseDragged);
    connect(blackboard0,&Blackboard::ellipseDone,ellipseDone);
    connect(blackboard0,&Blackboard::rectBegun,rectBegun);
    connect(blackboard0,&Blackboard::rectDragged,rectDragged);
    connect(blackboard0,&Blackboard::rectDone,rectDone);
    connect(blackboard0,&Blackboard::textAdded,textAdded);
    connect(blackboard0,&Blackboard::textChanged,textChanged);
    connect(blackboard0,&Blackboard::textDone,textDone);
    connect(blackboard0,&Blackboard::straightBegun,straightBegun);
    connect(blackboard0,&Blackboard::straightDragged,straightDragged);
    connect(blackboard0,&Blackboard::straightDone,straightDone);
    connect(blackboard0,&Blackboard::penDone,penDone);
    connect(blackboard0,&Blackboard::penDraw,penDraw);
    connect(blackboard0,&Blackboard::penStraighting,penStraighting);
    connect(blackboard0,&Blackboard::penDown,penDown);
    connect(blackboard0,&Blackboard::penDelete,penDelete);
    connect(blackboard0,&Blackboard::penMoving,penMoving);
    connect(blackboard0,&Blackboard::penMoved,penMoved);
    connect(blackboard0,&Blackboard::imageMoved,imageMoved);
    connect(blackboard0,&Blackboard::imageMoving,imageMoving);
    connect(blackboard0,&Blackboard::imageDelete,imageDelete);
    connect(blackboard0,&Blackboard::triangleMoved,triangleMoved);
    connect(blackboard0,&Blackboard::triangleMoving,triangleMoving);
    connect(blackboard0,&Blackboard::triangleDelete,triangleDelete);
    connect(blackboard0,&Blackboard::ellipseDelete,ellipseDelete);
    connect(blackboard0,&Blackboard::rectMoved,rectMoved);
    connect(blackboard0,&Blackboard::rectMoving,rectMoving);
    connect(blackboard0,&Blackboard::textMoving,textMoving);
    connect(blackboard0,&Blackboard::textMoved,textMoved);
    connect(blackboard0,&Blackboard::rectDelete,rectDelete);
    connect(blackboard0,&Blackboard::textDelete,textDelete);
    connect(blackboard0,&Blackboard::straightMoved,straightMoved);
    connect(blackboard0,&Blackboard::straightMoving,straightMoving);
    connect(blackboard0,&Blackboard::straightDelete,straightDelete);
    connect(blackboard0,&Blackboard::imagePaste,[copyFullItem,blackboard1](BbItemImage *item){copyFullItem(blackboard1,item,item);});
    connect(blackboard0,&Blackboard::trianglePaste,[copyFullItem,blackboard1](BbItemTriangle *item){copyFullItem(blackboard1,item,item);});
    connect(blackboard0,&Blackboard::ellipsePaste,[copyFullItem,blackboard1](BbItemEllipse *item){copyFullItem(blackboard1,item,item);});
    connect(blackboard0,&Blackboard::rectPaste,[copyFullItem,blackboard1](BbItemRect *item){copyFullItem(blackboard1,item,item);});
    connect(blackboard0,&Blackboard::textPaste,[copyFullItem,blackboard1](BbItemText *item){copyFullItem(blackboard1,item,item);});
    connect(blackboard0,&Blackboard::penPaste,[copyFullItem,blackboard1](BbItemPen *item){copyFullItem(blackboard1,item,item);});
    connect(blackboard0,&Blackboard::straightPaste,[copyFullItem,blackboard1](BbItemStraight *item){copyFullItem(blackboard1,item,item);});
#endif
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
    for(auto blackboard : findChildren<Blackboard*>())
    {
        QByteArray ba;
        blackboard->saveByteArray(ba);
        blackboard->clearItems();
        blackboard->readByteArray(ba);
    }
}

void BlackboardTestWindow::on_penWeight_valueChanged(int arg1)
{
    blackboard()->setPenWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_straightWeight_valueChanged(int arg1)
{
    blackboard()->setStraightPenWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_textWeight_valueChanged(int arg1)
{
    blackboard()->setTextPointWeight(arg1 * 0.01);
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
    blackboard()->setEllipseWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_triangle_clicked()
{
    blackboard()->setToolType(BBTT_Triangle);
}

void BlackboardTestWindow::on_triangleWeight_valueChanged(int arg1)
{
    blackboard()->setTriangleWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_localImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,QStringLiteral("选择图片"),".","*.png;*.jpg");

    if(fileName.isEmpty())
    {
        return;
    }

    QPixmap pm(fileName);
    blackboard()->addPixmapItem(pm);
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

void BlackboardTestWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,QStringLiteral("选择图片"),".","*.png;*.jpg");
    if(fileName.isEmpty())
    {
        return;
    }
    QPixmap pm(fileName);
    blackboard()->setBackground(pm);
}

void BlackboardTestWindow::on_pushButton_2_clicked()
{
    blackboard()->clearBackground();
}
