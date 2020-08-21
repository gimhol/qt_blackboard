QT += widgets
CONFIG += debug_and_release skip_target_version_ext
CONFIG(debug, debug|release) {
    TARGET = NsbBlackboardUtilsd
} else {
    TARGET = NsbBlackboardUtils
}
TEMPLATE = lib
CONFIG += c++14
DEFINES += NSB_BLACKBOARD_UTILS_SHARED
DEFINES += QT_DEPRECATED_WARNINGS
include(src.pri)
include(../info.pri)
win32
{
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
}else{
LIBS += -L$$OUT_PWD/../ -lNsbBlackboard
}
INCLUDEPATH += $$PWD/../blackboard-core
DEPENDPATH += $$PWD/../blackboard-core
