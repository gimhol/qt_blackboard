#include "BbItemPen.h"
#include "BbItemRect.h"
#include "BbItemStraight.h"
#include "BbItemText.h"
#include "BbItemData.h"
#include "BbItemEllipse.h"
#include "BbItemTriangle.h"
#include "BlackboardClientWindow.h"
#include "ColorPanel.h"
#include "ui_BlackboardClientWindow.h"
#include "BbItemImage.h"
#include "BlackboardConnector.h"
#include "BbMenu.h"
#include "DemoBbFactory.h"
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
#include <QListView>
#include <QGraphicsDropShadowEffect>

static QNetworkAccessManager *networkManager()
{
    static QNetworkAccessManager *ret = nullptr;
    if(!ret)
    {
        ret = new QNetworkAccessManager();
    }
    return ret;
}

BlackboardClientWindow::BlackboardClientWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlackboardClientWindow)
{
    ui->setupUi(this);
    _colorPanel = new ColorPanel(this);
    connect(_colorPanel,&ColorPanel::colorChanged,this,&BlackboardClientWindow::onColorChanged);

    ui->blackboardHeight->setValue(1000);
    QButtonGroup * buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->picker,BBTT_Picker);
    buttonGroup->addButton(ui->pen,BBTT_Pen);
    buttonGroup->addButton(ui->text,BBTT_Text);
    buttonGroup->addButton(ui->pointer,BBTT_Pointer);
    buttonGroup->addButton(ui->straight,BBTT_Straight);
    buttonGroup->addButton(ui->rect,BBTT_Rectangle);
    buttonGroup->addButton(ui->ellipse,BBTT_Ellipse);
    buttonGroup->addButton(ui->triangle,BBTT_Triangle);
    ui->blackboard->setFactory(new DemoBbFactory(this));
    ui->blackboard->setToolType(BBTT_Pointer);
    ui->pointer->setChecked(true);
    auto signal = static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked);
    connect(buttonGroup,signal,this,[&](int id){
        blackboard()->setToolType(BbToolType(id));
    });

    auto bbtts = QList<BbToolType>({BBTT_Pen,
                                    BBTT_Straight,
                                    BBTT_Text,
                                    BBTT_Rectangle,
                                    BBTT_Ellipse,
                                    BBTT_Triangle});
    for(auto bbtt: bbtts)
        itemSettings[bbtt] = blackboard()->toolSettings(bbtt);


    auto penSetting = blackboard()->toolSettings(BBTT_Pen);
    ui->penWeight->setValue(int(penSetting->weight()*100));
    ui->penColor->setColor(penSetting->pen.color());
    ui->penColor->setProperty("WhichColor",WhichColor_Pen);
    connect(ui->penColor,&ColorDisplayer::clicked,
            this,&BlackboardClientWindow::onColorDisplayerClicked);

    ui->brushColor->setColor(blackboard()->toolSettings(BBTT_Rectangle)->brush.color());
    ui->brushColor->setProperty("WhichColor",WhichColor_Brush);
    connect(ui->brushColor,&ColorDisplayer::clicked,
            this,&BlackboardClientWindow::onColorDisplayerClicked);

    ui->textColor->setColor(blackboard()->toolSettings<BbItemTextData>(BBTT_Text)->color);
    ui->textColor->setProperty("WhichColor",WhichColor_Text);
    connect(ui->textColor,&ColorDisplayer::clicked,
            this,&BlackboardClientWindow::onColorDisplayerClicked);

    QPixmap pm(5,5);
    pm.fill("red");
    ui->blackboard->setCanvasSize(ui->blackboard->baseSize().width(),1000);
    ui->blackboard->setPointerPixmap(pm);
    ui->blackboard->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->blackboard->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->blackboard->setContextMenuPolicy(Qt::CustomContextMenu);

    auto menu = new BbMenu(ui->blackboard);
    connect(menu,&BbMenu::toast,ui->textBrowser,&QTextBrowser::append);

    _connector = new BlackboardConnector(ui->blackboard);


    QMap<Qt::PenStyle,QString> penStyles = {
        {Qt::NoPen,"NoPen"},
        {Qt::SolidLine,"SolidLine"},
        {Qt::DashLine,"DashLine"},
        {Qt::DotLine,"DotLine"},
        {Qt::DashDotDotLine,"DashDotDotLine"},
        {Qt::CustomDashLine,"CustomDashLine"},
        {Qt::MPenStyle,"MPenStyle"},
    };
    QMap<Qt::PenCapStyle,QString> penCapStyles = {
        {Qt::FlatCap,"FlatCap"},
        {Qt::SquareCap,"SquareCap"},
        {Qt::RoundCap,"RoundCap"},
        {Qt::MPenCapStyle,"MPenCapStyle"},
    };
    QMap<Qt::PenJoinStyle,QString> penJoinStyles = {
        {Qt::MiterJoin,"MiterJoin"},
        {Qt::BevelJoin,"BevelJoin"},
        {Qt::RoundJoin,"RoundJoin"},
        {Qt::SvgMiterJoin,"SvgMiterJoin"},
        {Qt::MPenJoinStyle,"MPenJoinStyle"},
    };

    auto initComboBox = [](QComboBox *cb){
        cb->clear();
        auto listView = new QListView(cb);
        auto shadowEffect = new QGraphicsDropShadowEffect(listView);
        shadowEffect->setOffset(-5, 5);
        shadowEffect->setColor(Qt::black);
        shadowEffect->setBlurRadius(10);
        listView->setGraphicsEffect(shadowEffect);
        cb->setView(listView);
    };

    initComboBox(ui->cb_pen_style);
    for(auto itr=penStyles.begin();itr!=penStyles.end();++itr)
        ui->cb_pen_style->addItem(itr.value(),int(itr.key()));
    ui->cb_pen_style->setCurrentText(penStyles[penSetting->pen.style()]);

    initComboBox(ui->cb_pen_cap);
    for(auto itr=penCapStyles.begin();itr!=penCapStyles.end();++itr)
        ui->cb_pen_cap->addItem(itr.value(),int(itr.key()));
    ui->cb_pen_cap->setCurrentText(penCapStyles[penSetting->pen.capStyle()]);

    initComboBox(ui->cb_pen_join);
    for(auto itr=penJoinStyles.begin();itr!=penJoinStyles.end();++itr)
        ui->cb_pen_join->addItem(itr.value(),int(itr.key()));
    ui->cb_pen_join->setCurrentText(penJoinStyles[penSetting->pen.joinStyle()]);
}

BlackboardClientWindow::~BlackboardClientWindow()
{
    delete ui;
}

Blackboard *BlackboardClientWindow::blackboard()
{
    return ui->blackboard;
}

void BlackboardClientWindow::on_clear_clicked()
{
    blackboard()->clearItems();
}

void BlackboardClientWindow::on_remove_clicked()
{
    blackboard()->removeSelectedElement();
}

void BlackboardClientWindow::on_repaint_clicked()
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

void BlackboardClientWindow::on_penWeight_valueChanged(int arg1)
{
    for(auto settings: itemSettings){
        settings->setWeight(arg1 * 0.01);
    }
}

void BlackboardClientWindow::on_textWeight_valueChanged(int arg1)
{
    blackboard()->toolSettings<BbItemTextData>(BBTT_Text)->setPointWeight(arg1 * 0.01);
    ui->blackboard->scene()->enumSelected([&](IItemIndex *index,int num){
        if(index->toolType() == BBTT_Text){
            auto text = static_cast<BbItemText*>(index);
            text->setWeight(arg1 * 0.01);
        }
        return false;
    });
}

void BlackboardClientWindow::on_onlineImage_clicked()
{
//    QPixmap pm(100,100);
//    pm.fill(Qt::blue);
//    blackboard()->addPixmapItem(pm);
}

void BlackboardClientWindow::on_copy_clicked()
{
    blackboard()->copyItems();
}

void BlackboardClientWindow::on_paste_clicked()
{
    blackboard()->pasteItems();
}

void BlackboardClientWindow::on_selectedAll_clicked()
{
    blackboard()->selectedAll();
}

void BlackboardClientWindow::on_imagePick_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,QStringLiteral("选择图片"),".","*.png;*.jpg");
    if(!fileName.isEmpty())
    {
        blackboard()->addImageItemWithPath(fileName);
    }
}
void BlackboardClientWindow::loadImage(BbItemImage *item)
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

void BlackboardClientWindow::onColorChanged(const QColor &color)
{
    switch(_whichColor){
    case WhichColor_Pen:
        for(auto setting: itemSettings)
            setting->pen.setColor(color);
        ui->penColor->setColor(color);
        break;
    case WhichColor_Brush:
        ui->brushColor->setColor(color);
        for(auto setting: itemSettings)
            setting->brush.setColor(color);
        break;
    case WhichColor_Text:
        blackboard()->toolSettings<BbItemTextData>(BBTT_Text)->color = color;
        ui->textColor->setColor(color);
        ui->blackboard->scene()->enumSelected([&](IItemIndex *index,int){
            if(index->toolType() == BBTT_Text){
                auto text = static_cast<BbItemText*>(index);
                text->setColor(color);
            }
            return false;
        });
        break;
    case WhichColor_Max:
    case WhichColor_Invalid:
        break;
    }
}

void BlackboardClientWindow::onColorDisplayerClicked()
{
    auto colorDisplayer = qobject_cast<ColorDisplayer*>(sender());
    _whichColor = WhichColor(colorDisplayer->property("WhichColor").toInt());
    auto pos = QPoint(colorDisplayer->width(),
                      colorDisplayer->height());
    pos = colorDisplayer->mapToGlobal(pos);
    _colorPanel->move(pos);
    _colorPanel->show();
    _colorPanel->setColor(colorDisplayer->color());
}

void BlackboardClientWindow::on_imageInsert_clicked()
{
    BbItemImage *item = blackboard()->addImageItemWithUrl(
                ui->spinBoxImageWidth->value(),
                ui->spinBoxImageHeight->value(),
                ui->pictureUrl->text());
    loadImage(item);
}

void BlackboardClientWindow::on_blackboardHeight_editingFinished()
{
    auto width = blackboard()->canvasSize().width();
    blackboard()->setCanvasSize(width,ui->blackboardHeight->value());

}



void BlackboardClientWindow::on_btn_set_background_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,u8"选择图片",".","PNG(*.png);;JPEG(*.jpg)");
    if(!fileName.isEmpty())
    {
        QPixmap pm(fileName);
        blackboard()->setBackground(pm);
    }
}

void BlackboardClientWindow::on_btn_add_background_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,u8"选择图片",".","PNG(*.png);;JPEG(*.jpg)");
    if(!fileName.isEmpty())
    {
        QPixmap pm(fileName);
        blackboard()->addBackground(pm);
    }
}

void BlackboardClientWindow::on_btn_lay_backgrounds_clicked()
{
    blackboard()->layBackgrounds();
}

void BlackboardClientWindow::on_btn_clear_background_clicked()
{
    blackboard()->clearBackground();
}

void BlackboardClientWindow::on_btn_make_sure_show_all_backgrouns_clicked()
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
void BlackboardClientWindow::on_btn_remove_one_background_clicked()
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

void BlackboardClientWindow::on_btnConnectionToggle_clicked()
{
    if(_connector->isConnected()){
        ui->btnConnectionToggle->setText(QStringLiteral("进入"));
        _connector->disconnectFromServer();
    }else{
        ui->btnConnectionToggle->setText(QStringLiteral("退出"));
        _connector->connectToServer(
                    ui->leAddress->text(),
                    ui->lePort->text().toInt());
    }
}

void BlackboardClientWindow::on_deselectedAll_clicked()
{
    ui->blackboard->deselectAll();
}

void BlackboardClientWindow::on_cb_pen_join_activated(int index)
{
    auto style = static_cast<Qt::PenJoinStyle>(ui->cb_pen_join->currentData().toInt());
    for(auto setting:itemSettings)
        setting->pen.setJoinStyle(style);
}

void BlackboardClientWindow::on_cb_pen_style_activated(int index)
{
    auto style = static_cast<Qt::PenStyle>(ui->cb_pen_style->currentData().toInt());
    for(auto setting:itemSettings)
        setting->pen.setStyle(style);
}

void BlackboardClientWindow::on_cb_pen_cap_activated(int index)
{
    auto style = static_cast<Qt::PenCapStyle>(ui->cb_pen_cap->currentData().toInt());
    for(auto setting:itemSettings)
        setting->pen.setCapStyle(style);

}

void BlackboardClientWindow::on_cut_clicked()
{
    blackboard()->cutItems();
}
