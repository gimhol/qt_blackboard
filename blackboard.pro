QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(blackboard.pri)

HEADERS += \
    ColorPanel.h \
    BlackboardTestWindow.h \
    SaturationValueZone.h \
    AlphaBar.h \
    HueBar.h \
    AlphaBar.h \
    ColorDisplayer.h

SOURCES += Main.cpp \
    ColorPanel.cpp \
    BlackboardTestWindow.cpp \
    SaturationValueZone.cpp \
    AlphaBar.cpp \
    HueBar.cpp \
    AlphaBar.cpp \
    ColorDisplayer.cpp

FORMS += \
    ColorPanel.ui \
    BlackboardTestWindow.ui

OTHER_FILES += app.rc
RC_FILE += app.rc

win32
{
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01

    DEFINES += _ATL_XP_TARGETING
    QMAKE_CFLAGS += /D _USING_V120_SDK71_
    QMAKE_CXXFLAGS += /D _USING_V120_SDK71_
}
