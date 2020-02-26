QT += core widgets gui network
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
TARGET = NsbBlackboardDemo
include(src/src.pri)
include(../info.pri)
win32
{
    QMAKE_LFLAGS_DEBUG += /MAP
    QMAKE_CFLAGS_DEBUG += /Zi
    QMAKE_LFLAGS_DEBUG += /debug /opt:ref
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref

#    DISTFILES += $$PWD/dev/copy_to_shared.cmd
#    QMAKE_POST_LINK += $$PWD/dev/copy_to_shared.cmd

    DESTDIR = $$OUT_PWD/../
}
LIBS += -L$$OUT_PWD/../ -lNsbBlackboardd
INCLUDEPATH += $$PWD/../blackboard-shared
DEPENDPATH += $$PWD/../blackboard-shared

DISTFILES += \
    logo.ico
