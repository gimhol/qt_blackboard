INCLUDEPATH += $$PWD
HEADERS += \
    $$PWD/BbMenu.h \
    $$PWD/BlackboardClient.h \
    $$PWD/BlackboardConnector.h \
    $$PWD/BlackboardNetwork.h \
    $$PWD/BlackboardServer.h \
    $$PWD/BlackboardServerWindow.h \
    $$PWD/ColorPanel.h \
    $$PWD/BlackboardTestWindow.h \
    $$PWD/MainWindow.h \
    $$PWD/SaturationValueZone.h \
    $$PWD/AlphaBar.h \
    $$PWD/HueBar.h \
    $$PWD/AlphaBar.h \
    $$PWD/ColorDisplayer.h
SOURCES += \
    $$PWD/BbMenu.cpp \
    $$PWD/BlackboardClient.cpp \
    $$PWD/BlackboardConnector.cpp \
    $$PWD/BlackboardServer.cpp \
    $$PWD/BlackboardServerWindow.cpp \
    $$PWD/Main.cpp \
    $$PWD/ColorPanel.cpp \
    $$PWD/BlackboardTestWindow.cpp \
    $$PWD/MainWindow.cpp \
    $$PWD/SaturationValueZone.cpp \
    $$PWD/AlphaBar.cpp \
    $$PWD/HueBar.cpp \
    $$PWD/AlphaBar.cpp \
    $$PWD/ColorDisplayer.cpp
FORMS += \
    $$PWD/BlackboardServerWindow.ui \
    $$PWD/ColorPanel.ui \
    $$PWD/BlackboardTestWindow.ui \
    $$PWD/MainWindow.ui

OTHER_FILES += $$PWD/app.rc
RC_FILE += $$PWD/app.rc
