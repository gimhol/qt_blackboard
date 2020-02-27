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
    buttonGroup->addButton(ui->picker,BBTT_Picker);
    buttonGroup->addButton(ui->pen,BBTT_Pen);
    buttonGroup->addButton(ui->text,BBTT_Text);
    buttonGroup->addButton(ui->pointer,BBTT_Pointer);
    buttonGroup->addButton(ui->straight,BBTT_Straight);
    buttonGroup->addButton(ui->rect,BBTT_Rectangle);
    buttonGroup->addButton(ui->ellipse,BBTT_Ellipse);
    buttonGroup->addButton(ui->triangle,BBTT_Triangle);
    ui->blackboard->setFactory(new Factory(this));
    ui->blackboard->setToolType(BBTT_Pointer);
    ui->pointer->setChecked(true);
    auto signal = static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked);
    connect(buttonGroup,signal,this,[&](int id){
        blackboard()->setToolType(BbToolType(id));
    });


    penSettings = blackboard()->toolSettings<BbItemPenData>(BBTT_Pen);
    straightSettings = blackboard()->toolSettings<BbItemStraightData>(BBTT_Straight);
    textSettings = blackboard()->toolSettings<BbItemTextData>(BBTT_Text);
    rectSettings = blackboard()->toolSettings<BbItemRectData>(BBTT_Rectangle);
    ellipseSettings = blackboard()->toolSettings<BbItemEllipseData>(BBTT_Ellipse);
    triangleSettings = blackboard()->toolSettings<BbItemTriangleData>(BBTT_Triangle);

    ui->penWeight->setValue(int(penSettings->weight()*100));
    ui->penColor->setColor(penSettings->pen.color());
    ui->penColor->setProperty("WhichColor",WhichColor_Pen);
    connect(ui->penColor,&ColorDisplayer::clicked,
            this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->brushColor->setColor(rectSettings->brush.color());
    ui->brushColor->setProperty("WhichColor",WhichColor_Brush);
    connect(ui->brushColor,&ColorDisplayer::clicked,
            this,&BlackboardTestWindow::onColorDisplayerClicked);

    ui->textColor->setColor(textSettings->color);
    ui->textColor->setProperty("WhichColor",WhichColor_Text);
    connect(ui->textColor,&ColorDisplayer::clicked,
            this,&BlackboardTestWindow::onColorDisplayerClicked);

    QPixmap pm(5,5);
    pm.fill("red");
    ui->blackboard->setCanvasSize(ui->blackboard->baseSize().width(),1000);
    ui->blackboard->setPointerPixmap(pm);
    ui->blackboard->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->blackboard->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->blackboard->setContextMenuPolicy(Qt::CustomContextMenu);

    new BbMenu(ui->blackboard);
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

void BlackboardTestWindow::on_clear_clicked()
{
    blackboard()->clearItems();
}

void BlackboardTestWindow::on_remove_clicked()
{
    blackboard()->removeSelectedElement();
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
    straightSettings->setWeight(arg1 * 0.01);
    rectSettings->setWeight(arg1 * 0.01);
    ellipseSettings->setWeight(arg1 * 0.01);
    triangleSettings->setWeight(arg1 * 0.01);
}

void BlackboardTestWindow::on_textWeight_valueChanged(int arg1)
{
    textSettings->setPointWeight(arg1 * 0.01);
    ui->blackboard->scene()->enumSelected([&](IItemIndex *index,int num){
        if(index->toolType() == BBTT_Text){
            auto text = static_cast<BbItemText*>(index);
            text->setWeight(arg1 * 0.01);
        }
        return false;
    });
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
    blackboard()->selectedAll();
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
        straightSettings->pen.setColor(color);
        rectSettings->pen.setColor(color);
        ellipseSettings->pen.setColor(color);
        triangleSettings->pen.setColor(color);
        ui->penColor->setColor(color);
        break;
    case WhichColor_Brush:
        ui->brushColor->setColor(color);
        rectSettings->brush.setColor(color);
        ellipseSettings->brush.setColor(color);
        triangleSettings->brush.setColor(color);
        break;
    case WhichColor_Text:
        textSettings->color = color;
        ui->textColor->setColor(color);
        ui->blackboard->scene()->enumSelected([&](IItemIndex *index,int num){
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

void BlackboardTestWindow::on_deselectedAll_clicked()
{
    ui->blackboard->deselectAll();
}
