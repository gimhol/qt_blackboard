#include "BbItemPen.h"
#include "BbItemStraight.h"
#include "BbItemText.h"
#include "BlackboardTestWindow.h"
#include "ui_BlackboardTestWindow.h"
#include <QDebug>

BlackboardTestWindow::BlackboardTestWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlackboardTestWindow)
{
    ui->setupUi(this);

    bindBlackboard(ui->graphicsView,ui->graphicsView_2);
    bindBlackboard(ui->graphicsView_2,ui->graphicsView);

    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Pointer);
    }
}

BlackboardTestWindow::~BlackboardTestWindow()
{
    delete ui;
}

void BlackboardTestWindow::bindBlackboard(Blackboard *blackboard0, Blackboard *blackboard1)
{
    static QPixmap pm(":/res/images/cursor-lazer-pen.png");
    blackboard0->setCanvasSize(800,800);
    blackboard0->setPointerPixmap(pm);
    connect(blackboard0,&Blackboard::resized,[](float scale){qDebug()<< "resized, scale : " << scale;});

    connect(blackboard0,&Blackboard::textAdded,[blackboard1](BbItemText *item){
        auto copy = new BbItemText();
        blackboard1->scene()->add(copy);
        copy->setPos(item->pos());
        copy->setId(item->id());
    });
    connect(blackboard0,&Blackboard::textChanged,[blackboard1](BbItemText *item){
        auto copy = blackboard1->scene()->find<BbItemText>(item->id());
        copy->setPlainText(item->toPlainText());
    });
    connect(blackboard0,&Blackboard::textDone,[blackboard1](BbItemText *item){
        auto copy = blackboard1->scene()->find<BbItemText>(item->id());
        copy->setPlainText(item->toPlainText());
        copy->setPlainText(item->toPlainText());
    });
    connect(blackboard0,&Blackboard::textMoved,[blackboard1](BbItemText *item){
        auto copy = blackboard1->scene()->find<BbItemText>(item->id());
        copy->setPos(item->pos());
    });
    connect(blackboard0,&Blackboard::textDelete,[blackboard1](BbItemText *item){
        auto copy = blackboard1->scene()->find<BbItemText>(item->id());
        blackboard1->scene()->remove(copy);
    });


    connect(blackboard0,&Blackboard::straightBegun,[blackboard1](BbItemStraight *item){
        auto copy = new BbItemStraight();
        blackboard1->scene()->add(copy);
        copy->setColor(item->color());
        copy->setWeight(item->weight());
        copy->setId(item->id());
        copy->begin(item->a());
    });
    connect(blackboard0,&Blackboard::straightDragged,[blackboard1](BbItemStraight *item){
        auto copy = blackboard1->scene()->find<BbItemStraight>(item->id());
        copy->drag(item->b());
    });
    connect(blackboard0,&Blackboard::straightDone,[blackboard1](BbItemStraight *item){
        auto copy = blackboard1->scene()->find<BbItemStraight>(item->id());
        copy->done();
    });
    connect(blackboard0,&Blackboard::straightMoved,[blackboard1](BbItemStraight *item){
        auto copy = blackboard1->scene()->find<BbItemStraight>(item->id());
        copy->setPos(item->pos());
    });
    connect(blackboard0,&Blackboard::straightDelete,[blackboard1](BbItemStraight *item){
        auto copy = blackboard1->scene()->find<BbItemStraight>(item->id());
        blackboard1->scene()->remove(copy);
    });

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
    connect(blackboard0,&Blackboard::cursorHidden,[blackboard1](QPoint localPoint){
        Q_UNUSED(localPoint);
        blackboard1->hidePointer("remote_pointer");
    });

    connect(blackboard0,&Blackboard::penDelete,[blackboard1](BbItemPen *item){
        auto copy = blackboard1->scene()->find<BbItemPen>(item->id());
        blackboard1->scene()->remove(copy);
    });
    connect(blackboard0,&Blackboard::penMoved,[blackboard1](BbItemPen *item){
        auto copy = blackboard1->scene()->find<BbItemPen>(item->id());
        copy->setPos(item->pos());
    });
    connect(blackboard0,&Blackboard::penDone,[blackboard1](BbItemPen *item){
        auto copy = blackboard1->scene()->find<BbItemPen>(item->id());
        copy->done();
    });
    connect(blackboard0,&Blackboard::penDraw,[blackboard1](BbItemPen *item){
        auto copy = blackboard1->scene()->find<BbItemPen>(item->id());
        for(auto point : *item->changed())
        {
            copy->penDraw(point);
        }
    });
    connect(blackboard0, &Blackboard::penDown, [blackboard1](BbItemPen *item){
        auto copy = new BbItemPen();
        blackboard1->scene()->add(copy);
        copy->setColor(item->color());
        copy->setWeight(item->weight());
        copy->setId(item->id());
        copy->penDown(item->changed()->first());
    });
}

void BlackboardTestWindow::on_pointer_clicked()
{
    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Pointer);
    }
}

void BlackboardTestWindow::on_pen_clicked()
{
    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Pen);
    }
}

void BlackboardTestWindow::on_clear_clicked()
{
    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->clearItems();
    }
}

void BlackboardTestWindow::on_remove_clicked()
{
    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->removeSelectedElement();
    }
}

void BlackboardTestWindow::on_picker_clicked()
{
    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Picker);
    }
}

void BlackboardTestWindow::on_straight_clicked()
{
    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_STRAIGHT);
    }
}

void BlackboardTestWindow::on_text_clicked()
{
    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Text);
    }
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
