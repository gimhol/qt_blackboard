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
    void saveAreaToPicture(QRectF srcArea);

    void saveBlackboardToJsonFile();
    void readBlackboardFromJsonFile();

    void saveBlackboardToFile();
    void readBlackboardFromFile();

    QPixmap areaToPixmap(QRectF srcArea);
    void savePixmapsToFiles(QList<QPixmap*> pixmaps);
signals:
    void toast(QString);
private slots:
    void onToolMenuTriggered(QAction *action);
    void onSaveSelectedItemsToPicutreAction();

    void onSaveAllItemsToPicturePageByPage();

    void onSelectedAllActionTriggered();

    void onBlackboardCustomContextMenuRequested(const QPoint &pos);
private:
    QPointer<Blackboard> _blackboard;
};

#endif // BBMENU_H
