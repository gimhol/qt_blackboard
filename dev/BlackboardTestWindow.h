#ifndef BLACKBOARDTESTWINDOW_H
#define BLACKBOARDTESTWINDOW_H
#include "BlackboardClient.h"
#include "BlackboardConnector.h"

#include <QWidget>
#include <Blackboard.h>
namespace Ui {
    class BlackboardTestWindow;
}
class Blackboard;
class BlackboardTestWindow : public QWidget
{
    Q_OBJECT
    BbItemPenData *penSettings;
    BbItemStraightData *straightSettings;
    BbItemTextData *textSettings;
    BbItemRectData *rectSettings;
    BbItemEllipseData *ellipseSettings;
    BbItemTriangleData *triangleSettings;

public:
    explicit BlackboardTestWindow(QWidget *parent = nullptr);

    static void bindBlackboard(Blackboard * blackboard0,Blackboard *blackboard1);

    Blackboard * blackboard();

    ~BlackboardTestWindow();

    static void loadImage(BbItemImage *item);

private slots:
    void on_pointer_clicked();

    void on_pen_clicked();

    void on_clear_clicked();

    void on_remove_clicked();

    void on_picker_clicked();

    void on_straight_clicked();

    void on_text_clicked();

    void on_repaint_clicked();

    void on_penWeight_valueChanged(int arg1);

    void on_straightWeight_valueChanged(int arg1);

    void on_textWeight_valueChanged(int arg1);

    void on_rect_clicked();

    void on_ellipse_clicked();

    void on_ellipseWeight_valueChanged(int arg1);

    void on_triangle_clicked();

    void on_triangleWeight_valueChanged(int arg1);

    void on_onlineImage_clicked();

    void on_copy_clicked();

    void on_paste_clicked();

    void on_selectedAll_clicked();

    void on_rectWeight_valueChanged(int arg1);

    void on_imagePick_clicked();

    void on_imageInsert_clicked();

    void on_blackboardHeight_editingFinished();

    void on_btn_set_background_clicked();

    void on_btn_add_background_clicked();

    void on_btn_lay_backgrounds_clicked();

    void on_btn_clear_background_clicked();

    void on_btn_make_sure_show_all_backgrouns_clicked();

    void on_btn_remove_one_background_clicked();

    void on_btnConnectionToggle_clicked();

private:
    Ui::BlackboardTestWindow *ui;
    QPointer<BlackboardConnector> _connector;
};

#endif // BLACKBOARDTESTWINDOW_H
