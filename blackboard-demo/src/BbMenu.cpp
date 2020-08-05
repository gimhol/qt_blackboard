#include "BbMenu.h"

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
    connect(addAction(QStringLiteral("保存黑板内容为文件")),
            &QAction::triggered,
            this,
            &BbMenu::saveBlackboardToJsonFile);
    connect(addAction(QStringLiteral("从文件读取黑板内容")),
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
    auto path =
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+
            QStringLiteral("/牛师帮在线课堂/")+
            time.toString("yyyyMMdd");
    QDir d(path);
    if(!d.exists())
        d.mkpath(path);

    QFile file(path+"/"+time.toString("hhMMss")+".json");
    file.open(QIODevice::WriteOnly);
    file.write(jDoc.toJson());
    file.close();
}

void BbMenu::readBlackboardFromFile()
{
    auto time = QDateTime::currentDateTime();
    auto dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) +
            QStringLiteral("/牛师帮在线课堂/") +
            time.toString("yyyyMMdd");
    auto path = QFileDialog::getOpenFileName(nullptr,u8"打开nsbbb文件",dir,"*.json");
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

void BbMenu::onSaveSelectedItemsToPicutreAction()
{
    auto items = _blackboard->scene()->selectedItems();
    saveItemsToPicture(items);
}

void BbMenu::onSaveAllItemsToPicutreAction()
{
    auto beginTime = QDateTime::currentMSecsSinceEpoch();
    if(!_blackboard){
        // TODO: make some noise here;
        return;
    }

    auto scene = _blackboard->scene();
    qreal maxH = scene->height();
    qreal w = scene->width();
    qreal h = w * scene->pageAspectRatio();
    qreal x = 0;
    qreal y = 0;
    QHash<int,QRectF> dirtyPages;

    auto items = _blackboard->items();
    if(items.empty()){
        // TODO: make some noise here;
        return;
    }

    auto itemCount = items.count();
    while(!items.empty()){
        auto item = items.takeFirst();
        if(!item->isVisible())
            continue;
        item->setSelected(false);
        auto itemRect = item->sceneBoundingRect();
        if(itemRect.right() < 0 ||
                itemRect.bottom() < 0 ||
                itemRect.left() > w ||
                itemRect.bottom() > maxH)
            continue;

        auto beginPage = int(itemRect.top()/h);
        auto endPage = int(itemRect.bottom()/h);
        for(auto page = beginPage; page <= endPage; ++page)
            dirtyPages[page] = QRectF(0,h * page, w, h);
    }
    auto time0 = QDateTime::currentMSecsSinceEpoch();
    if(dirtyPages.empty()){
        // TODO: make some noise here;
        return;
    }
    auto time = QDateTime::currentDateTime();
    auto dirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QStringLiteral("/牛师帮在线课堂/")+ time.toString("yyyyMMdd");
    QDir d(dirPath);
    if(!d.exists())
        d.mkpath(dirPath);

    for(auto itr = dirtyPages.begin(); itr != dirtyPages.end(); ++itr){
        auto filePath = QString("%1/%2_%3.png")
                .arg(dirPath)
                .arg(time.toString("hhMMss"))
                .arg(itr.key());

        auto pm = areaToPixmap(itr.value());
        pm.save(filePath);
    }
    auto time1 = QDateTime::currentMSecsSinceEpoch();

    auto endTime = QDateTime::currentMSecsSinceEpoch();
    auto title = u8"保存完成";
    auto content = QString(u8"其中图形%1个，共%2頁，检查获取非空页耗时：%3毫秒，图片生成与保存耗时：%4毫秒，总共耗时：%5毫秒")
            .arg(itemCount)
            .arg(dirtyPages.count())
            .arg(time0-beginTime)
            .arg(time1-time0)
            .arg(endTime-beginTime);
    QMessageBox::information(nullptr, title,content);
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
