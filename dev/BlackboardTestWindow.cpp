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


BlackboardTestWindow::BlackboardTestWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlackboardTestWindow)
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
        blackboard->scene()->setItemIdGenerator([this](){
            return QString("%1_%2")
                    .arg(windowTitle())
                    .arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
        });
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
    auto itemMoved = [itemMoving](QGraphicsItem *item)
    {
        itemMoving(item);
        auto index = dynamic_cast<IItemIndex*>(item);
        if(index)
        {

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
            auto copy = new BbItemImage();
            if(copy)
            {
                blackboard1->scene()->add(copy);
                copy->setZ(copy->z());
                copy->setPixmap(item->pixmap());
                copy->setId(item->id());
            }
        }
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
                copy->begin(item->beginPos());
                copy->setId(item->id());
            }
        }
    };
    auto ellipseDraw = [blackboard1](BbItemEllipse *item){
        if(item)
        {
            auto copy = blackboard1->find<BbItemEllipse>(item->id());
            if(copy)
            {
                copy->draw(item->dragPos());
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
                copy->setPos(item->pos());
                copy->setId(item->id());
            }
        }
    };
    auto textChanged = [blackboard1](BbItemText *item){
        auto copy = blackboard1->find<BbItemText>(item->id());
        auto data = dynamic_cast<BbItemTextData*>(item->data());
        auto t = item->document()->toRawText();
        for(auto a : data->text)
            qDebug() << a;
        if(copy)
        {
            copy->setText(item->text());
        }
    };
    auto textDone = textChanged;
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
                copy->setZ(item->z());
                copy->setColor(item->color());
                copy->setWeight(item->weight());
                copy->setId(item->id());
                blackboard1->scene()->add(copy);
                copy->penDown(item->changed()->first());
            }
        }
    };

#define HANDLE_ITEM_EVENT(_EVENT_TYPE_,_ITEM_TYPE_) \
    case BBIET_##_EVENT_TYPE_: \
        _EVENT_TYPE_(dynamic_cast<_ITEM_TYPE_*>(index)); \
        break

    connect(blackboard0,&Blackboard::multipleItemChanged,[=](BBItemEventType eventType,IItemIndex *index){
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
    connect(blackboard0,&Blackboard::itemChanged,[=](BBItemEventType eventType,IItemIndex *index){
        switch(eventType)
        {
            HANDLE_ITEM_EVENT(itemMoved,QGraphicsItem);
            HANDLE_ITEM_EVENT(penStraighting,BbItemPen);
            HANDLE_ITEM_EVENT(penDown,BbItemPen);
            HANDLE_ITEM_EVENT(penDraw,BbItemPen);
            HANDLE_ITEM_EVENT(penDone,BbItemPen);
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

void BlackboardTestWindow::on_rectWeight_valueChanged(int arg1)
{
    rectSettings->setWeight(arg1 * 0.01);
}
