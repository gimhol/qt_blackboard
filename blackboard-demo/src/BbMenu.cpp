#include "BbMenu.h"
#include "LazyPixmapGrabber.h"
#include "PixmapSaver.h"

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

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
    connect(addAction(QStringLiteral("保存选中图形为图片（单图）")),
            &QAction::triggered,
            this,
            &BbMenu::onSaveSelectedItemsToPicutreAction);
    connect(addAction(QStringLiteral("保存全部非空页为图片（多图）")),
            &QAction::triggered,
            this,
            &BbMenu::onSaveAllItemsToPicutreAction);
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
    auto time = QDateTime::currentDateTime();
    auto path =
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+
            QStringLiteral("/牛师帮在线课堂/")+
            time.toString("yyyyMMdd");
    QDir d(path);
    if(!d.exists())
        d.mkpath(path);
    pm.save(path+"/"+time.toString("hhMMss")+".png");
}

void BbMenu::saveBlackboardToJsonFile()
{
    QJsonDocument jDoc(_blackboard->toJsonObject());

    auto time = QDateTime::currentDateTime();
    auto dirStr =
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+
            QStringLiteral("/牛师帮在线课堂/")+
            time.toString("yyyyMMdd");
    QDir d(dirStr);
    if(!d.exists())
        d.mkpath(dirStr);
    auto pathStr = dirStr+"/"+time.toString("hhMMss")+".json";
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
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) +
            QStringLiteral("/牛师帮在线课堂/") +
            time.toString("yyyyMMdd");
    auto path = QFileDialog::getOpenFileName(nullptr,u8"打开json文件",dir,"*.json");
    QFile file(path);
    if(!file.exists())
        return;
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    auto jDoc = QJsonDocument::fromJson(file.readAll());
    _blackboard->clearItems();
    _blackboard->fromJsonObject(jDoc.object());
}

void BbMenu::saveBlackboardToFile()
{
    QJsonDocument jDoc(_blackboard->toJsonObject());

    auto time = QDateTime::currentDateTime();
    auto dirStr =
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+
            QStringLiteral("/牛师帮在线课堂/")+
            time.toString("yyyyMMdd");
    QDir d(dirStr);
    if(!d.exists())
        d.mkpath(dirStr);
    auto pathStr = dirStr+"/"+time.toString("hhMMss")+".nsbbb";
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
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) +
            QStringLiteral("/牛师帮在线课堂/") +
            time.toString("yyyyMMdd");
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
    auto time = QDateTime::currentDateTime();
    auto dirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) +
            QStringLiteral("/牛师帮在线课堂/")+
            time.toString("yyyyMMdd");
    QDir d(dirPath);
    if(!d.exists())
        d.mkpath(dirPath);
    QHash<QString,QPixmap*> pms;
    auto i = 0;
    for(auto pixmap : pixmaps){
        ++i;
        auto filePath = QStringLiteral("%1/%2_%3.png")
                .arg(dirPath)
                .arg(time.toString("hhMMss"))
                .arg(i);
        pms[filePath] = pixmap;
    }
    auto total = pixmaps.count();
    auto pixmapSaver = new PixmapSaver(pms);
    auto onPixmapSaverProgress = [this](QString path,int duration,int current,int total){
        auto content = QStringLiteral("图片保存进度：%1/%2，耗时：%3毫秒，路径：%4")
                .arg(current)
                .arg(total)
                .arg(duration)
                .arg(path);
        emit toast(content);
    };
    connect(pixmapSaver,&PixmapSaver::progress,pixmapSaver,onPixmapSaverProgress);
    connect(pixmapSaver,&PixmapSaver::finished,pixmapSaver,[this,total,pixmapSaver](){
        auto content = QStringLiteral("图片保存线程结束，共%1頁，耗时：%2毫秒。")
                .arg(total)
                .arg(pixmapSaver->duration);
        emit toast(content);
    });
    connect(pixmapSaver,&QThread::finished,pixmapSaver,&QObject::deleteLater);
    pixmapSaver->start();
}

void BbMenu::onSaveSelectedItemsToPicutreAction()
{
    auto items = _blackboard->scene()->selectedItems();
    saveItemsToPicture(items);
}

void BbMenu::onSaveAllItemsToPicutreAction()
{
    emit toast(QStringLiteral("开始生成图片..."));
    auto beginTime = QDateTime::currentMSecsSinceEpoch();
    if(!_blackboard){
        emit toast(QStringLiteral("黑板指针已空？！"));
        return;
    }

    auto scene = _blackboard->scene();
    auto itemCount = scene->items().count();
    emit toast(QStringLiteral("正在检测非空页..."));
    auto areas = scene->getNotEmptyPageAreas();
    auto time0 = QDateTime::currentMSecsSinceEpoch();
    if(areas.empty()){
        emit toast(QStringLiteral("没有找到非空页。"));
        return;
    }

    emit toast(QStringLiteral("共有非空页数量：%1").arg(areas.size()));
    auto grabber = new LazyPixmapGrabber();
    grabber->blackboard = _blackboard;
    grabber->areas = areas.values();
    connect(grabber,&LazyPixmapGrabber::progress,this,[this](qlonglong duration,int current, int total){
        auto content = QStringLiteral("图片生成进度，%1/%2。耗时：%3")
                .arg(current)
                .arg(total)
                .arg(duration);
        emit toast(content);
    });

    connect(grabber,&LazyPixmapGrabber::done,this,[this,itemCount,time0,beginTime](QList<QPixmap*> pixmaps){
        auto endTime = QDateTime::currentMSecsSinceEpoch();
        auto content = QStringLiteral("图片生成完毕, 其中图形%1个，"
                                      "共%2頁，"
                                      "检查获取非空页耗时：%3毫秒，"
                                      "图片生成耗时：%4毫秒，"
                                      "总共耗时：%5毫秒")
                .arg(itemCount)
                .arg(pixmaps.count())
                .arg(time0-beginTime)
                .arg(endTime-time0)
                .arg(endTime-beginTime);
        emit toast(content);
        emit toast(QStringLiteral("开启图片保存线程..."));
    });
    connect(grabber,&LazyPixmapGrabber::done,this,&BbMenu::savePixmapsToFiles);
    connect(grabber,&LazyPixmapGrabber::done,grabber,&LazyPixmapGrabber::deleteLater);
    grabber->start();
}

void BbMenu::onSelectedAllActionTriggered()
{
//    _blackboard->setToolType(BBTT_Picker);
    _blackboard->selectedAll();
}

void BbMenu::onBlackboardCustomContextMenuRequested(const QPoint &pos)
{
    init();
    move(_blackboard->mapToGlobal(pos));
    show();
}
