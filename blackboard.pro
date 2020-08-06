TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
    blackboard-core \
    blackboard-utils \
    blackboard-demo

include(info.pri)

win32{
    DISTFILES = \
        $$PWD/logo.ico \
        $$PWD/README.md
}
