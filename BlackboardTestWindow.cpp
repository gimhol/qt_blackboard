#include "BbItemPen.h"
#include "BbItemRect.h"
#include "BbItemStraight.h"
#include "BbItemText.h"
#include "BbItemEllipse.h"
#include "BbItemTriangle.h"
#include "BlackboardTestWindow.h"
#include "ColorPanel.h"
#include "ui_BlackboardTestWindow.h"
#include <QDebug>
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


    ui->rectWeight->setValue(blackboard()->rectWeight() * 100);
    ui->rectPenColor->setColor(blackboard()->rectPenColor());
    ui->rectBrushColor->setColor(blackboard()->rectBrushColor());
    connect(ui->rectPenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(u8"调色");
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
            cp->setWindowTitle(u8"调色");
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setRectBrushColor);
            connect(cp,&ColorPanel::colorChanged,ui->rectBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->rectBrushColor());
        cp->show();
    });

    ui->ellipseWeight->setValue(blackboard()->ellipseWeight() * 100);
    ui->ellipsePenColor->setColor(blackboard()->ellipsePenColor());
    ui->ellipseBrushColor->setColor(blackboard()->ellipseBrushColor());
    connect(ui->ellipsePenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(u8"调色");
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
            cp->setWindowTitle(u8"调色");
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setEllipseBrushColor);
            connect(cp,&ColorPanel::colorChanged,ui->ellipseBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->ellipseBrushColor());
        cp->show();
    });

    ui->triangleWeight->setValue(blackboard()->triangleWeight() * 100);
    ui->trianglePenColor->setColor(blackboard()->trianglePenColor());
    ui->triangleBrushColor->setColor(blackboard()->triangleBrushColor());
    connect(ui->trianglePenColor,&ColorDisplayer::clicked,[&](){
        static ColorPanel * cp = nullptr;
        if(!cp)
        {
            cp = new ColorPanel();
            cp->setWindowModality(Qt::WindowModality::ApplicationModal);
            cp->setWindowTitle(u8"调色");
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
            cp->setWindowTitle(u8"调色");
            connect(cp,&ColorPanel::colorChanged,blackboard(),&Blackboard::setTriangleBrushColor);
            connect(cp,&ColorPanel::colorChanged,ui->triangleBrushColor,&ColorDisplayer::setColor);
        }
        cp->setColor(blackboard()->triangleBrushColor());
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

    connect(blackboard0,&Blackboard::triangleBegun,[blackboard1](BbItemTriangle *item){
        auto copy = new BbItemTriangle();
        copy->setZValue(copy->zValue());
        blackboard1->scene()->add(copy);
        copy->setPenColor(item->penColor());
        copy->setWeight(item->weight());
        copy->setBrushColor(item->brushColor());
        copy->begin(item->point(0));
        copy->setId(item->id());
    });
    connect(blackboard0,&Blackboard::triangleDragged,[blackboard1](BbItemTriangle *item){
        auto copy = blackboard1->scene()->find<BbItemTriangle>(item->id());
        copy->drag(item->point(1+item->step()));
    });
    connect(blackboard0,&Blackboard::triangleDone,[blackboard1](BbItemTriangle *item){
        auto copy = blackboard1->scene()->find<BbItemTriangle>(item->id());
        copy->done();
    });
    connect(blackboard0,&Blackboard::triangleMoved,[blackboard1](BbItemTriangle *item){
        auto copy = blackboard1->scene()->find<BbItemTriangle>(item->id());
        copy->setPos(item->pos());
    });
    connect(blackboard0,&Blackboard::triangleDelete,[blackboard1](BbItemTriangle *item){
        auto copy = blackboard1->scene()->find<BbItemTriangle>(item->id());
        blackboard1->scene()->remove(copy);
    });

    connect(blackboard0,&Blackboard::ellipseBegun,[blackboard1](BbItemEllipse *item){
        auto copy = new BbItemEllipse();
        copy->setZValue(copy->zValue());
        blackboard1->scene()->add(copy);
        copy->setPenColor(item->penColor());
        copy->setWeight(item->weight());
        copy->setBrushColor(item->brushColor());
        copy->begin(item->beginPos());
        copy->setId(item->id());
    });
    connect(blackboard0,&Blackboard::ellipseDragged,[blackboard1](BbItemEllipse *item){
        auto copy = blackboard1->scene()->find<BbItemEllipse>(item->id());
        copy->drag(item->dragPos());
    });
    connect(blackboard0,&Blackboard::ellipseDone,[blackboard1](BbItemEllipse *item){
        auto copy = blackboard1->scene()->find<BbItemEllipse>(item->id());
        copy->done();
    });
    connect(blackboard0,&Blackboard::ellipseMoved,[blackboard1](BbItemEllipse *item){
        auto copy = blackboard1->scene()->find<BbItemEllipse>(item->id());
        copy->setPos(item->pos());
    });
    connect(blackboard0,&Blackboard::ellipseDelete,[blackboard1](BbItemEllipse *item){
        auto copy = blackboard1->scene()->find<BbItemEllipse>(item->id());
        blackboard1->scene()->remove(copy);
    });

    connect(blackboard0,&Blackboard::rectBegun,[blackboard1](BbItemRect *item){
        auto copy = new BbItemRect();
        copy->setZValue(copy->zValue());
        blackboard1->scene()->add(copy);
        copy->setPenColor(item->penColor());
        copy->setWeight(item->weight());
        copy->setBrushColor(item->brushColor());
        copy->begin(item->beginPos());
        copy->setId(item->id());
    });
    connect(blackboard0,&Blackboard::rectDragged,[blackboard1](BbItemRect *item){
        auto copy = blackboard1->scene()->find<BbItemRect>(item->id());
        copy->drag(item->dragPos());
    });
    connect(blackboard0,&Blackboard::rectDone,[blackboard1](BbItemRect *item){
        auto copy = blackboard1->scene()->find<BbItemRect>(item->id());
        copy->done();
    });
    connect(blackboard0,&Blackboard::rectMoved,[blackboard1](BbItemRect *item){
        auto copy = blackboard1->scene()->find<BbItemRect>(item->id());
        copy->setPos(item->pos());
    });
    connect(blackboard0,&Blackboard::rectDelete,[blackboard1](BbItemRect *item){
        auto copy = blackboard1->scene()->find<BbItemRect>(item->id());
        blackboard1->scene()->remove(copy);
    });


    connect(blackboard0,&Blackboard::textAdded,[blackboard1](BbItemText *item){
        auto copy = new BbItemText();
        copy->setZValue(copy->zValue());
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
        copy->setZValue(copy->zValue());
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
        copy->penStraighting(item->straightTo());
    });
    connect(blackboard0, &Blackboard::penDown, [blackboard1](BbItemPen *item){
        auto copy = new BbItemPen();
        copy->setZValue(copy->zValue());
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
