QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(dev/dev.pri)
win32
{
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/shared/lib/ -lNsbBlackboard
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/shared/lib/ -lNsbBlackboard
INCLUDEPATH += $$PWD/shared/include
DEPENDPATH += $$PWD/shared/include
