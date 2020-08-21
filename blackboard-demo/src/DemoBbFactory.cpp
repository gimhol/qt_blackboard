#include "DemoBbFactory.h"

DemoBbFactory::DemoBbFactory(QWidget *window):_window(window){

}

QString DemoBbFactory::makeItemId(BbToolType bbtt)
{
    auto ptr = int(_window);
    auto org = BbFactory::makeItemId(bbtt);
    return QString("%1_%2").arg(ptr).arg(org);
}
