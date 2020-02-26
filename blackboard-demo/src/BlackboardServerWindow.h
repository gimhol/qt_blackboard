#ifndef BLACKBOARDSERVERWINDOW_H
#define BLACKBOARDSERVERWINDOW_H

#include "BlackboardServer.h"

#include <QWidget>

namespace Ui {
class BlackboardServerWindow;
}

class BlackboardServerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BlackboardServerWindow(QWidget *parent = nullptr);
    ~BlackboardServerWindow();

private slots:
    void on_btnToggle_clicked();

private:
    Ui::BlackboardServerWindow *ui;
    QPointer<BlackboardServer> _server;
};

#endif // BLACKBOARDSERVERWINDOW_H
