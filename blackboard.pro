TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
    blackboard-core \
    blackboard-utils \
    blackboard-demo

include(info.pri)

OTHER_FILES = \
    $$PWD/logo.ico \
    $$PWD/README.md
