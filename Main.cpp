#include <BlackboardTestWindow.h>
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication appCentre(argc,argv);

    BlackboardTestWindow::start();
	
    return appCentre->exec();
}
