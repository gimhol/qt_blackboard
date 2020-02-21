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
#include "BlackboardConnector.h"
#include "BbMenu.h"
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
    ui(new Ui::BlackboardTestWindow)
{
    ui->setupUi(this);
    _colorPanel = new ColorPanel(this);
    connect(_colorPanel,&ColorPanel::colorChanged,this,&BlackboardTestWindow::onColorChanged);

    ui->blackboardHeight->setValue(1000);
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
    ui->penColor->setProperty("WhichColor",WhichColor_Pen);
    connect(ui->penColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->straightWeight->setValue(int(straightSettings->weight() * 100));
    ui->straightColor->setColor(straightSettings->pen.color());
    ui->straightColor->setProperty("WhichColor",WhichColor_Straight);
    connect(ui->straightColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->textWeight->setValue(int(textSettings->pointWeight() * 100));
    ui->textColor->setColor(textSettings->color);
    ui->textColor->setProperty("WhichColor",WhichColor_Text);
    connect(ui->textColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->rectWeight->setValue(int(rectSettings->weight() * 100));
    ui->rectPenColor->setColor(rectSettings->pen.color());
    ui->rectPenColor->setProperty("WhichColor",WhichColor_RectPen);
    connect(ui->rectPenColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->rectBrushColor->setColor(rectSettings->brush.color());
    ui->rectBrushColor->setProperty("WhichColor",WhichColor_RectBrush);
    connect(ui->rectBrushColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->ellipseWeight->setValue(int(ellipseSettings->weight() * 100));
    ui->ellipsePenColor->setColor(ellipseSettings->pen.color());
    ui->ellipsePenColor->setProperty("WhichColor",WhichColor_EllipsePen);
    connect(ui->ellipsePenColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->ellipseBrushColor->setColor(ellipseSettings->brush.color());
    ui->ellipseBrushColor->setProperty("WhichColor",WhichColor_EllipseBrush);
    connect(ui->ellipseBrushColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);


    ui->triangleWeight->setValue(int(triangleSettings->weight() * 100));
    ui->trianglePenColor->setColor(triangleSettings->pen.color());
    ui->trianglePenColor->setProperty("WhichColor",WhichColor_TrianglePen);
    connect(ui->trianglePenColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->triangleBrushColor->setColor(triangleSettings->brush.color());
    ui->triangleBrushColor->setProperty("WhichColor",WhichColor_TriangleBrush);
    connect(ui->triangleBrushColor,&ColorDisplayer::clicked,this,&BlackboardTestWindow::onColorDisplayerClicked);

    connect(ui->blackboard,&Blackboard::itemSelected,[&](IItemIndex *index, bool selected){
        if(selected)
            ui->textBrowser->append(QString(QStringLiteral("选择: %1")).arg(index->id()));
    });

    QPixmap pm(5,5);
    pm.fill("red");
    ui->blackboard->setCanvasSize(ui->blackboard->baseSize().width(),1000);
    ui->blackboard->setPointerPixmap(pm);
    ui->blackboard->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->blackboard->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->blackboard->setContextMenuPolicy(Qt::CustomContextMenu);

    auto blackboard = ui->blackboard;
    connect(blackboard,&QWidget::customContextMenuRequested,this,[blackboard](const QPoint &pos){
        auto menu = blackboard->findChild<BbMenu*>("right_click_menu");
        if(!menu){
            menu = new BbMenu(blackboard);
            menu->setAttribute(Qt::WA_DeleteOnClose);
        }
        menu->move(blackboard->mapToGlobal(pos));
        menu->show();
    });
    _connector = new BlackboardConnector(ui->blackboard);
}

BlackboardTestWindow::~BlackboardTestWindow()
{
    delete ui;
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
                copy->resize(data->size);
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
                copy->resize(data->size);
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
                copy->resize(data->size);
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
                HANDLE_ITEM_EVENT(itemPaste,QGraphicsItem);
                default:
                    break;
            }
            index = next;
        }
    });
    connect(blackboard0,&Blackboard::itemChanged,blackboard1,[=](BBItemEventType eventType,IItemIndex *index){
        switch(eventType)
        {
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
    return ui->blackboard;
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

void BlackboardTestWindow::onColorChanged(const QColor &color)
{
    switch(_whichColor){
    case WhichColor_Pen:
        penSettings->setColor(color);
        ui->penColor->setColor(color);
        break;
    case WhichColor_Text:
        textSettings->color = color;
        ui->textColor->setColor(color);
        break;
    case WhichColor_Straight:
        straightSettings->pen.setColor(color);
        ui->straightColor->setColor(color);
        break;
    case WhichColor_RectPen:
        rectSettings->pen.setColor(color);
        ui->rectPenColor->setColor(color);
        break;
    case WhichColor_RectBrush:
        rectSettings->brush.setColor(color);
        ui->rectBrushColor->setColor(color);
        break;
    case WhichColor_EllipsePen:
        ellipseSettings->pen.setColor(color);
        ui->ellipsePenColor->setColor(color);
        break;
    case WhichColor_EllipseBrush:
        ellipseSettings->brush.setColor(color);
        ui->ellipseBrushColor->setColor(color);
        break;
    case WhichColor_TrianglePen:
        triangleSettings->pen.setColor(color);
        ui->trianglePenColor->setColor(color);
        break;
    case WhichColor_TriangleBrush:
        triangleSettings->brush.setColor(color);
        ui->triangleBrushColor->setColor(color);
        break;
    case WhichColor_Invalid:
    case WhichColor_Max:
        break;
    }
}

void BlackboardTestWindow::onColorDisplayerClicked()
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

void BlackboardTestWindow::on_btnConnectionToggle_clicked()
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
