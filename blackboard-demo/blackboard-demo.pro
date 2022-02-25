QT += core widgets gui network
CONFIG += c++14
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG(debug, debug|release):   TARGET = NsbBlackboardDemod
else:                           TARGET = NsbBlackboardDemo

include(src/src.pri)
include(../info.pri)
include(../get-version-from-git.pri)
include(../vld.pri)
RESOURCES += resource.qrc
win32
{
    DISTFILES = $$PWD/copy_to_shared.cmd
    QMAKE_POST_LINK += $$PWD/copy_to_shared.cmd

    QMAKE_CFLAGS_DEBUG += /Zi
    QMAKE_LFLAGS_DEBUG += /MAP /debug /opt:ref
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /MAP /debug /opt:ref
}

CONFIG(debug, debug|release):   LIBS += -L$$OUT_PWD/../blackboard-core/debug -lNsbBlackboardd
else:                           LIBS += -L$$OUT_PWD/../blackboard-core/release -lNsbBlackboard
INCLUDEPATH += $$PWD/../blackboard-core

CONFIG(debug, debug|release):   LIBS += -L$$OUT_PWD/../blackboard-utils/debug -lNsbBlackboardUtilsd
else:                           LIBS += -L$$OUT_PWD/../blackboard-utils/release -lNsbBlackboardUtils
INCLUDEPATH += $$PWD/../blackboard-utils

