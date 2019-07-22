#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T14:36:10
#
#-------------------------------------------------

QT += widgets
CONFIG += debug_and_release
CONFIG(debug, debug|release) {
    TARGET = NsbBlackboardd
} else {
    TARGET = NsbBlackboard
}
TEMPLATE = lib
CONFIG += c++11
DEFINES += NSB_BLACKBOARD_SHARED BLACKBOARD_ITEM_INDEX_SIGNAL
DEFINES += QT_DEPRECATED_WARNINGS
include(blackboard.pri)
win32
{
    QMAKE_LFLAGS_DEBUG += /MAP
    QMAKE_CFLAGS_DEBUG += /Zi
    QMAKE_LFLAGS_DEBUG += /debug /opt:ref
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    DISTFILES += $$PWD/dev/copy_to_shared.cmd
    QMAKE_POST_LINK += $$PWD/dev/copy_to_shared.cmd
}
