#ifndef DEMOBBFACTORY_H
#define DEMOBBFACTORY_H

#include <BbFactory.h>

class DemoBbFactory: public BbFactory
{
public:
    DemoBbFactory(QWidget *window);

    QString makeItemId(BbToolType bbtt = BBTT_None) override;
private:
    QWidget *_window;
};

#endif // DEMOBBFACTORY_H
