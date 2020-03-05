TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
    blackboard-shared \
    blackboard-demo
include(info.pri)

win32{
    DISTFILES = \
        $$PWD/logo.ico \
        $$PWD/README.md

    QMAKE_POST_LINK += copy_to_shared.cmd
}
