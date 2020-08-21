QT += core widgets gui network
CONFIG += c++14
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG(debug, debug|release) {
    TARGET = NsbBlackboardDemod
} else {
    TARGET = NsbBlackboardDemo
}
include(src/src.pri)
include(../info.pri)
win32
{
    DISTFILES = $$PWD/copy_to_shared.cmd
    QMAKE_POST_LINK += $$PWD/copy_to_shared.cmd

    QMAKE_LFLAGS_DEBUG += /MAP
    QMAKE_CFLAGS_DEBUG += /Zi
    QMAKE_LFLAGS_DEBUG += /debug /opt:ref
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    DESTDIR = $$OUT_PWD/../
}
CONFIG(debug, debug|release) {
LIBS += -L$$OUT_PWD/../ -lNsbBlackboardd
LIBS += -L$$OUT_PWD/../ -lNsbBlackboardUtilsd
}else{
LIBS += -L$$OUT_PWD/../ -lNsbBlackboard
LIBS += -L$$OUT_PWD/../ -lNsbBlackboardUtils
}
INCLUDEPATH += $$PWD/../blackboard-core $$PWD/../blackboard-utils
DEPENDPATH += $$PWD/../blackboard-core $$PWD/../blackboard-utils
RESOURCES += resource.qrc
