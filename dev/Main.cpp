#include <BlackboardTestWindow.h>
#include <QApplication>

#include "BbItemEllipseData.h"
#include "BbItemRectData.h"
#include "BbItemTriangleData.h"
#include "BlackboardServer.h"
#include "ColorPanel.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication appCentre(argc,argv);
    MainWindow mainWindow;
    mainWindow.show();
    return appCentre.exec();
}
