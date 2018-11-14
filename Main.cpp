#include <BlackboardTestWindow.h>
#include <QApplication>

#include "BbItemEllipseData.h"
#include "BbItemRectData.h"
#include "BbItemTriangleData.h"
#include "ColorPanel.h"

int main(int argc, char *argv[])
{
    BbItemRectData::defaultBrush.setColor(QColor(255,255,255,255));
    BbItemEllipseData::defaultBrush.setColor(QColor(255,255,255,255));
    BbItemTriangleData::defaultBrush.setColor(QColor(255,255,255,255));
    QApplication appCentre(argc,argv);
    BlackboardTestWindow::start();
    return appCentre.exec();
}
