#include "BbItemPen.h"
#include "BbItemStraight.h"
#include "BbItemText.h"
#include "BlackboardTestWindow.h"
#include "ColorPanel.h"
#include "ui_BlackboardTestWindow.h"
#include <QDebug>
#include <BbItemPenData.h>

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

    for(auto blackboard : findChildren<Blackboard*>())
    {
        blackboard->setToolType(BBTT_Pointer);
    }

    ui->penWeight->setValue(blackboard()->penWeight() * 100);
    ui->penColor->setColor(blackboard()->penColor());
    connect(ui->penColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(u8"调色");
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setPenColor);
            connect(cp,&ColorPanel::colorChanged,ui->penColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->penColor());
        cp->show();
    });

    ui->straightWeight->setValue(blackboard()->straightPenWeight() * 100);
    ui->straightColor->setColor(blackboard()->straightPenColor());
    connect(ui->straightColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(u8"调色");
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setStraightPenColor);
            connect(cp,&ColorPanel::colorChanged,ui->straightColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->straightPenColor());
        cp->show();
    });



    ui->textWeight->setValue(blackboard()->textPointWeight() * 100);
    ui->textColor->setColor(blackboard()->textColor());
    connect(ui->textColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(u8"调色");
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setTextColor);
            connect(cp,&ColorPanel::colorChanged,ui->textColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->straightPenColor());
        cp->show();
    });
}

BlackboardTestWindow::~BlackboardTestWindow()
{
    delete ui;
}

void BlackboardTestWindow::start()
{
    static BlackboardTestWindow win0, win1;
    win0.setWindowTitle(u8"测试窗口0");
    win1.setWindowTitle(u8"测试窗口1");
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

    connect(blackboard0,&Blackboard::textAdded,[blackboard1](BbItemText *item){
        auto copy = new BbItemText();
        blackboard1->scene()->add(copy);
        copy->setFont(item->font());
        copy->setColor(item->color());
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
    connect(blackboard0,&Blackboard::penStraighting,[blackboard1](BbItemPen *item){
        auto copy = blackboard1->scene()->find<BbItemPen>(item->id());
        copy->penStraighting(item->straightLineTo());
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
    blackboard()->setToolType(BBTT_STRAIGHT);
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
