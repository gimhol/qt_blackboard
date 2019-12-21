INCLUDEPATH += $$PWD
HEADERS += \
    $$PWD/BlackboardClient.h \
    $$PWD/BlackboardNetwork.h \
    $$PWD/BlackboardServer.h \
    $$PWD/ColorPanel.h \
    $$PWD/BlackboardTestWindow.h \
    $$PWD/SaturationValueZone.h \
    $$PWD/AlphaBar.h \
    $$PWD/HueBar.h \
    $$PWD/AlphaBar.h \
    $$PWD/ColorDisplayer.h
SOURCES += \
    $$PWD/BlackboardClient.cpp \
    $$PWD/BlackboardServer.cpp \
    $$PWD/Main.cpp \
    $$PWD/ColorPanel.cpp \
    $$PWD/BlackboardTestWindow.cpp \
    $$PWD/SaturationValueZone.cpp \
    $$PWD/AlphaBar.cpp \
    $$PWD/HueBar.cpp \
    $$PWD/AlphaBar.cpp \
    $$PWD/ColorDisplayer.cpp
FORMS += \
    $$PWD/ColorPanel.ui \
    $$PWD/BlackboardTestWindow.ui

OTHER_FILES += $$PWD/app.rc
RC_FILE += $$PWD/app.rc
