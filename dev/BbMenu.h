#ifndef BBMENU_H
#define BBMENU_H

#include <Blackboard.h>
#include <QMenu>
#include <QPointer>

class BbMenu: public QMenu
{
    Q_OBJECT
public:
    BbMenu(Blackboard *parent = nullptr);
    void init();
    void initToolMenu();

    void saveItemsToPicture(QList<QGraphicsItem*> items);

private slots:
    void onToolMenuTriggered(QAction *action);
    void onSaveSelectedItemsToPicutreAction();
    void onSaveAllItemsToPicutreAction();
    void onSelectedAllActionTriggered();
private:
    QPointer<Blackboard> _blackboard;
};

#endif // BBMENU_H
