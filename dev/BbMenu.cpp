#include "BbMenu.h"

#include <QDateTime>
#include <QDir>
#include <QStandardPaths>

BbMenu::BbMenu(Blackboard *parent):
    QMenu(parent),
    _blackboard(parent)
{
    setStyleSheet("QMenu{background-color:#333333;border:1px solid black;border-radius:2px;}"
                  "QMenu::item:selected{background-color:#555555;}"
                  "QMenu::item:disabled{background-color:#333333;color:#888888;}");
    init();
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
    connect(addAction(QStringLiteral("删除")),
            &QAction::triggered,
            _blackboard,
            &Blackboard::removeSelectedItems);
    addSeparator();
    connect(addAction(QStringLiteral("保存选中图形为图片")),
            &QAction::triggered,
            this,
            &BbMenu::onSaveSelectedItemsToPicutreAction);
    connect(addAction(QStringLiteral("保存全部图形为图片")),
            &QAction::triggered,
            this,
            &BbMenu::onSaveAllItemsToPicutreAction);
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
        QPixmap pm(int(r-l),int(b-t));
        pm.fill(_blackboard->palette().window().color());
        QPainter painter(&pm);
        _blackboard->scene()->render(&painter,
                                    QRectF(0,0,r-l,b-t),
                                    QRectF(l,t,r-l,b-t));
        auto time = QDateTime::currentDateTime();
        auto path =
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+
                QStringLiteral("/牛师帮在线课堂/")+
                time.toString("yyyyMMdd");
        QDir d(path);
        if(!d.exists())
            d.mkpath(path);
        pm.save(path+"/"+time.toString("hhmmss")+".png");
    }
}

void BbMenu::onSaveSelectedItemsToPicutreAction()
{
    auto items = _blackboard->scene()->selectedItems();
    saveItemsToPicture(items);
}

void BbMenu::onSaveAllItemsToPicutreAction()
{
    auto items = _blackboard->scene()->items();
    saveItemsToPicture(items);
}

void BbMenu::onSelectedAllActionTriggered()
{
    _blackboard->setToolType(BBTT_Picker);
    _blackboard->selectedAll();
}
