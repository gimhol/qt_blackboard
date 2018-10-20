#ifndef BLACKBOARDTESTWINDOW_H
#define BLACKBOARDTESTWINDOW_H

#include <QWidget>

namespace Ui {
    class BlackboardTestWindow;
}
class Blackboard;
class BlackboardTestWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BlackboardTestWindow(QWidget *parent = nullptr);
    ~BlackboardTestWindow();

    void bindBlackboard(Blackboard * blackboard0,Blackboard *blackboard1);
private slots:
    void on_pointer_clicked();

    void on_pen_clicked();

    void on_clear_clicked();

    void on_remove_clicked();

    void on_picker_clicked();

    void on_straight_clicked();

    void on_text_clicked();

    void on_repaint_clicked();

private:
    Ui::BlackboardTestWindow *ui;
};

#endif // BLACKBOARDTESTWINDOW_H
