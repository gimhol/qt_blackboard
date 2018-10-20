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

    explicit BlackboardTestWindow(QWidget *parent = nullptr);

    static void bindBlackboard(Blackboard * blackboard0,Blackboard *blackboard1);

    Blackboard * blackboard();
public:

    ~BlackboardTestWindow();

    static void start();



private slots:
    void on_pointer_clicked();

    void on_pen_clicked();

    void on_clear_clicked();

    void on_remove_clicked();

    void on_picker_clicked();

    void on_straight_clicked();

    void on_text_clicked();

    void on_repaint_clicked();

    void on_penSize_sliderMoved(int position);

private:
    Ui::BlackboardTestWindow *ui;
};

#endif // BLACKBOARDTESTWINDOW_H
