#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T14:36:10
#
#-------------------------------------------------

QT       += widgets
CONFIG+=debug_and_release
CONFIG(debug, debug|release) {
    TARGET = NsbBlackboardd
} else {
    TARGET = NsbBlackboard
}
TEMPLATE = lib

DEFINES += NSB_BLACKBOARD_SHARED
CONFIG += c++11
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=

HEADERS +=
include(blackboard.pri)
unix
{
    target.path = /usr/lib
    INSTALLS += target
}
win32
{
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01

    DEFINES += _ATL_XP_TARGETING
    QMAKE_CFLAGS += /D _USING_V120_SDK71_
    QMAKE_CXXFLAGS += /D _USING_V120_SDK71_


}
