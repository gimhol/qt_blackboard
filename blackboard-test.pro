QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(dev/dev.pri)
win32
{
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01

    DEFINES += _ATL_XP_TARGETING
    QMAKE_CFLAGS += /D _USING_V120_SDK71_
    QMAKE_CXXFLAGS += /D _USING_V120_SDK71_
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/shared/lib/ -lNsbBlackboard
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/shared/lib/ -lNsbBlackboard
INCLUDEPATH += $$PWD/shared/include
DEPENDPATH += $$PWD/shared/include
