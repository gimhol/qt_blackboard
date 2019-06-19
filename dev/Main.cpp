#include <BlackboardTestWindow.h>
#include <QApplication>

#include "BbItemEllipseData.h"
#include "BbItemRectData.h"
#include "BbItemTriangleData.h"
#include "ColorPanel.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication appCentre(argc,argv);
    BlackboardTestWindow::start();
    return appCentre.exec();
}
