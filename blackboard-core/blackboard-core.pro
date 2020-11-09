QT += widgets
TEMPLATE = lib
CONFIG += c++11 debug_and_release skip_target_version_ext

CONFIG(debug, debug|release):   TARGET = NsbBlackboardd
else:                           TARGET = NsbBlackboard

DEFINES += NSB_BLACKBOARD_SHARED QT_DEPRECATED_WARNINGS
include(src.pri)
include(../info.pri)

win32{
    QMAKE_LFLAGS_RELEASE += /MAP /debug /opt:ref
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_DEBUG += /MAP /debug /opt:ref
    QMAKE_CFLAGS_DEBUG += /Zi

    CONFIG(debug, debug|release)
    {
        DEFINES += $$(NSB_PROJECT_VLD)
        CONFIG += $$(NSB_PROJECT_VLD)
        NSB_PROJECT_VLD_ON
        {
            message(vld on! libs path: \"$$(NSB_PROJECT_VLD_LIBS)\" include path: \"$$(NSB_PROJECT_VLD_INCLUDE)\")
            LIBS += -L"$$(NSB_PROJECT_VLD_LIBS)" -lvld
            INCLUDEPATH += $$(NSB_PROJECT_VLD_INCLUDE)
        }
    }
}


