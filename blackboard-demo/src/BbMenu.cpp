#include "BbMenu.h"
#include "BbLazyPixmapGrabber.h"
#include "BbPixmapSaver.h"

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

static QString makeSaveDir(){
    auto ret = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) +
            QStringLiteral("/牛师帮在线课堂/")+
            QDateTime::currentDateTime().toString("yyyyMMdd");
    QDir dir(ret);
    if(!dir.exists())
        dir.mkpath(ret);
    return ret;
}
static QString makeSaveName(int index,QString suffix){
    return QDateTime::currentDateTime().toString("HHmmss")+"_"+QString::number(index)+suffix;
}

BbMenu::BbMenu(Blackboard *parent):
    QMenu(parent),
    _blackboard(parent)
{
    setStyleSheet("QMenu{background-color:#333333;border:1px solid black;border-radius:2px;}"
                  "QMenu::item:selected{background-color:#555555;}"
                  "QMenu::item:disabled{background-color:#333333;color:#888888;}");
    connect(_blackboard,&QWidget::customContextMenuRequested,
            this,&BbMenu::onBlackboardCustomContextMenuRequested);
}

void BbMenu::initToolMenu()
{
    auto toolMenu = new QMenu(this);
    auto action = addMenu(toolMenu);
    action->setText(QStringLiteral("工具"));
    using Pair = QPair<BbToolType,QString>;
    auto tools = QList<Pair>();
    tools << Pair(BBTT_Picker,QStringLiteral("选择器"));
    tools << Pair(BBTT_Pointer,QStringLiteral("激光笔"));
    tools << Pair(BBTT_Pen,QStringLiteral("画笔"));
    tools << Pair(BBTT_Straight,QStringLiteral("直线"));
    tools << Pair(BBTT_Text,QStringLiteral("文本框"));
    tools << Pair(BBTT_Rectangle,QStringLiteral("矩形"));
    tools << Pair(BBTT_Ellipse,QStringLiteral("椭圆形"));
    tools << Pair(BBTT_Triangle,QStringLiteral("三角形"));
    for(auto pair:tools){
        auto isMe = pair.first == _blackboard->toolType();
        action = toolMenu->addAction(pair.second);
        action->setData(pair.first);
        action->setCheckable(true);
        action->setDisabled(isMe);
        action->setChecked(isMe);
    }
    connect(toolMenu,&QMenu::triggered,this,&BbMenu::onToolMenuTriggered);
}

void BbMenu::init(){
    clear();
    initToolMenu();
    addSeparator();
    connect(addAction(QStringLiteral("组合")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::groupUp);
    connect(addAction(QStringLiteral("解除组合")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::dismiss);
    addSeparator();
    connect(addAction(QStringLiteral("全选")),
            &QAction::triggered,
            this,
            &BbMenu::onSelectedAllActionTriggered);
    connect(addAction(QStringLiteral("取消选择")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::deselectAll);
    connect(addAction(QStringLiteral("删除")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::removeSelectedItems);
    addSeparator();
    connect(addAction(QStringLiteral("剪切")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::cutItems);
    connect(addAction(QStringLiteral("复制")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::copyItems);
    connect(addAction(QStringLiteral("粘贴")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::pasteItems);

    addSeparator();
    connect(addAction(QStringLiteral("保存选中图形为图片（单图）")),
            &QAction::triggered,
            this,
            &BbMenu::onSaveSelectedItemsToPicutreAction);
    connect(addAction(QStringLiteral("保存全部非空页为图片（多图）")),
            &QAction::triggered,
            this,
            &BbMenu::onSaveAllItemsToPicturePageByPage);
    connect(addAction(QStringLiteral("保存黑板内容为JSON文件")),
            &QAction::triggered,
            this,
            &BbMenu::saveBlackboardToJsonFile);
    connect(addAction(QStringLiteral("从JSON文件读取黑板内容")),
            &QAction::triggered,
            this,
            &BbMenu::readBlackboardFromJsonFile);

    connect(addAction(QStringLiteral("保存黑板内容为nsbbb文件")),
            &QAction::triggered,
            this,
            &BbMenu::saveBlackboardToFile);
    connect(addAction(QStringLiteral("从nsbbb文件读取黑板内容")),
            &QAction::triggered,
            this,
            &BbMenu::readBlackboardFromFile);
}

void BbMenu::onToolMenuTriggered(QAction *action)
{
    _blackboard->setToolType(BbToolType(action->data().toInt()));
}

void BbMenu::saveItemsToPicture(QList<QGraphicsItem*> items)
{
    auto first = true;
    auto l=0.0;
    auto r=0.0;
    auto t=0.0;
    auto b=0.0;
    auto sceneRect = _blackboard->scene()->sceneRect();
    if(!items.empty()){
        for(auto item : items){
            auto boundingRect = item->sceneBoundingRect();
            item->setSelected(false);
            if(first){
                l = boundingRect.left();
                r = boundingRect.right();
                t = boundingRect.top();
                b = boundingRect.bottom();
                first = false;
            }else{
                if(l > boundingRect.left())
                    l = boundingRect.left();
                if(r < boundingRect.right())
                    r = boundingRect.right();
                if(t > boundingRect.top())
                    t = boundingRect.top();
                if(b < boundingRect.bottom())
                    b = boundingRect.bottom();
            }
        }
        l=(std::max)(0.0,l-50);
        r=(std::min)(sceneRect.width(),r+50);
        t=(std::max)(0.0,t-50);
        b=(std::min)(sceneRect.height(),b+50);
        saveAreaToPicture(QRectF(l,t,r-l,b-t));
    }
}

void BbMenu::saveAreaToPicture(QRectF srcArea)
{
    auto pm = areaToPixmap(srcArea);
    pm.save(makeSaveDir()+"/"+makeSaveName(0,".png"));
}

void BbMenu::saveBlackboardToJsonFile()
{
    QJsonDocument jDoc(_blackboard->toJsonObject());
    auto pathStr = makeSaveDir()+"/"+makeSaveName(0,".json");
    QFile file(pathStr);
    if(file.open(QIODevice::WriteOnly)){
        file.write(jDoc.toJson());
        file.flush();
        file.close();
        QMessageBox::information(nullptr,
                                 QStringLiteral("保存成功！"),
                                 QStringLiteral("已保存至文件：")+pathStr);
    }else{
        QMessageBox::critical(nullptr,
                                 QStringLiteral("保失败存！！"),
                                 QStringLiteral("无法打开文件：")+pathStr);
    }
}

void BbMenu::readBlackboardFromJsonFile()
{
    auto time = QDateTime::currentDateTime();
    auto dir = makeSaveDir();
    auto path = QFileDialog::getOpenFileName(nullptr,u8"打开json文件",dir,"*.json");
    QFile file(path);
    if(!file.exists() || !file.open(QIODevice::ReadOnly))
        return;
    auto jDoc = QJsonDocument::fromJson(file.readAll());
    _blackboard->clearItems();
    _blackboard->fromJsonObject(jDoc.object());
}

void BbMenu::saveBlackboardToFile()
{
    QJsonDocument jDoc(_blackboard->toJsonObject());
    auto pathStr = makeSaveDir()+"/"+makeSaveName(0,".nsbbb");
    QFile file(pathStr);
    if(file.open(QIODevice::WriteOnly)){
        file.write(jDoc.toBinaryData());
        file.flush();
        file.close();
        QMessageBox::information(nullptr,
                                 QStringLiteral("保存成功！"),
                                 QStringLiteral("已保存至文件：")+pathStr);
    }else{
        QMessageBox::critical(nullptr,
                                 QStringLiteral("保失败存！！"),
                                 QStringLiteral("无法打开文件：")+pathStr);
    }
}

void BbMenu::readBlackboardFromFile()
{
    auto time = QDateTime::currentDateTime();
    auto dir = makeSaveDir();
    auto path = QFileDialog::getOpenFileName(nullptr,u8"打开nsbbb文件",dir,"*.nsbbb");
    QFile file(path);
    if(!file.exists())
        return;
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    auto jDoc = QJsonDocument::fromBinaryData(file.readAll());
    _blackboard->clearItems();
    _blackboard->fromJsonObject(jDoc.object());
}

QPixmap BbMenu::areaToPixmap(QRectF srcArea)
{
    int srcWidth = int(srcArea.width());
    int srcHeight = int(srcArea.height());
    int dstWidth = 2 * srcWidth;
    int dstHeight = 2 * srcHeight;
    QPixmap pm(dstWidth,dstHeight);
    pm.fill(_blackboard->palette().window().color());
    QPainter painter(&pm);
    _blackboard->scene()->render(&painter,pm.rect(),srcArea);
    return pm;
}

void BbMenu::savePixmapsToFiles(QList<QPixmap*> pixmaps)
{
    static QTime time;
    time.start();
    emit toast(QStringLiteral("开启保存图片..."));
    QHash<QString,QPixmap*> pms;
    auto i = 0;
    auto dirPath = makeSaveDir();
    for(auto pixmap : pixmaps){
        ++i;
        auto filePath = dirPath+"/"+makeSaveName(i,".png");
        pms[filePath] = pixmap;
    }
    auto total = pixmaps.count();
    auto pixmapSaver = new BbPixmapSaver(pms);
    auto onPixmapSaverProgress = [this](int current,int total){
        auto content = QStringLiteral("图片保存进度：%1/%2，耗时：%3毫秒")
                .arg(current)
                .arg(total)
                .arg(time.restart());
        emit toast(content);
    };
    auto onPixmapSaverFinish = [this,total,pixmapSaver](){
        auto content = QStringLiteral("图片保存线程结束，耗时：%2毫秒。")
                .arg(total)
                .arg(time.restart());
        emit toast(content);
        pixmapSaver->deleteLater();
    };
    connect(pixmapSaver,&BbPixmapSaver::progress,pixmapSaver,onPixmapSaverProgress);
    connect(pixmapSaver,&BbPixmapSaver::finished,pixmapSaver,onPixmapSaverFinish);
    pixmapSaver->start();
}

void BbMenu::onSaveSelectedItemsToPicutreAction()
{
    auto items = _blackboard->scene()->selectedItems();
    saveItemsToPicture(items);
}

void BbMenu::onSaveAllItemsToPicturePageByPage()
{
    static QTime time;
    time.start();

    emit toast(QStringLiteral("开始生成图片..."));
    if(!_blackboard){
        emit toast(QStringLiteral("黑板指针已空？！"));
        return;
    }

    auto scene = _blackboard->scene();
    auto itemCount = scene->items().count();
    emit toast(QStringLiteral("正在检测非空页..."));
    auto areas = scene->getNotEmptyPageAreas();
    if(areas.empty()){
        emit toast(QStringLiteral("没有找到非空页。"));
        return;
    }

    emit toast(QStringLiteral("共有图形数量：%1，非空页数量：%2，耗时：%3毫秒")
               .arg(itemCount)
               .arg(areas.size())
               .arg(time.restart()));

    auto grabber = new BbLazyPixmapGrabber();
    grabber->setBlackboard(_blackboard);
    grabber->setAreas(areas.values());
    auto onGrabberProgress = [&](int current, int total){
        auto content = QStringLiteral("图片生成进度，%1/%2。耗时：%3毫秒")
                .arg(current)
                .arg(total)
                .arg(time.restart());
        emit toast(content);
    };
    auto onGrabberDone = [this,grabber](QList<QPixmap*> pixmaps){
        emit toast(QStringLiteral("图片生成完毕。"));
        savePixmapsToFiles(pixmaps);
        grabber->deleteLater();
    };
    connect(grabber,&BbLazyPixmapGrabber::progress,this,onGrabberProgress);
    connect(grabber,&BbLazyPixmapGrabber::done,this,onGrabberDone);
    grabber->start();
}

void BbMenu::onSelectedAllActionTriggered()
{
    _blackboard->selectedAll();
}

void BbMenu::onBlackboardCustomContextMenuRequested(const QPoint &pos)
{
    init();
    move(_blackboard->mapToGlobal(pos));
    show();
}
