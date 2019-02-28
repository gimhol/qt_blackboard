#define BLACKBOARD_ITEM_INDEX_SIGNAL
#include <BlackboardTestWindow.h>
#include <QApplication>

#include "BbItemEllipseData.h"
#include "BbItemRectData.h"
#include "BbItemTriangleData.h"
#include "ColorPanel.h"

int main(int argc, char *argv[])
{
    QApplication appCentre(argc,argv);
    BlackboardTestWindow::start();
    return appCentre.exec();
}
