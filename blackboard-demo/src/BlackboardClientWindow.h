#ifndef BLACKBOARDCLIENTWINDOW_H
#define BLACKBOARDCLIENTWINDOW_H
#include "BlackboardClient.h"
#include "BlackboardConnector.h"
#include "ColorPanel.h"

#include <QWidget>
#include <Blackboard.h>
namespace Ui {
    class BlackboardClientWindow;
}
class Blackboard;

enum WhichColor{
    WhichColor_Invalid,
    WhichColor_Pen,
    WhichColor_Brush,
    WhichColor_Text,
    WhichColor_Max
};

class BlackboardClientWindow : public QWidget
{
    Q_OBJECT
    WhichColor _whichColor = WhichColor_Invalid;
    QMap<BbToolType,BbItemData*> itemSettings;
public:
    explicit BlackboardClientWindow(QWidget *parent = nullptr);

    Blackboard * blackboard();

    ~BlackboardClientWindow();

    static void loadImage(BbItemImage *item);

private slots:
    void onColorChanged(const QColor &color);

    void onColorDisplayerClicked();

    void on_clear_clicked();

    void on_remove_clicked();

    void on_repaint_clicked();

    void on_penWeight_valueChanged(int arg1);

    void on_textWeight_valueChanged(int arg1);

    void on_onlineImage_clicked();

    void on_copy_clicked();

    void on_paste_clicked();

    void on_selectedAll_clicked();

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

    void on_deselectedAll_clicked();

    void on_cb_pen_join_activated(int index);

    void on_cb_pen_style_activated(int index);

    void on_cb_pen_cap_activated(int index);

    void on_cut_clicked();

    void on_cb_cubic_pen_clicked(bool checked);

private:
    Ui::BlackboardClientWindow *ui;
    QPointer<BlackboardConnector> _connector;
    QPointer<ColorPanel> _colorPanel;
};

#endif // BLACKBOARDCLIENTWINDOW_H
