# -------------------------------------------------
# Project created by QtCreator 2009-04-29T19:45:45
# -------------------------------------------------
TEMPLATE = app
TARGET = context-notify
CONFIG += qdbus \
    gui \
    link_pkgconfig
CONFIG += debug
PKGCONFIG = context libnotify glib

SOURCES += context-notify_src/context-notify.cpp \
    context-notify_src/notifyprompt.cpp \
    context-notify_src/contextListener.cpp \
    misc.cpp \
    xmlsp.cpp \
    configuration.cpp \
    domain.cpp \
    transition.cpp \
    program.cpp \
    rule.cpp \
    mrule.cpp \
    trule.cpp \
    context-notify_src/changecontextdialog.cpp \
    context-notify_src/domainlist.cpp \
    context-notify_src/updatedomain.cpp \
    programaction.cpp \
    loop.cpp \
    programvariable.cpp \
    abstractrule.cpp \
    context-notify_src/notify.c \
    variabletypes.cpp
HEADERS += context-notify_src/notifyprompt.h \
    context-notify_src/contextListener.h \
    context-common.h \
    misc.h \
    xmlsp.h \
    configuration.h \
    domain.h \
    transition.h \
    program.h \
    rule.h \
    mrule.h \
    trule.h \
    context-notify_src/changecontextdialog.h \
    context-notify_src/domainlist.h \
    xml_common.h \
    context-notify_src/updatedomain.h \
    programaction.h \
    loop.h \
    programvariable.h \
    abstractrule.h \
    contextd_src/plugins/iptable-blocker/domaincachethread.h \
    context-notify_src/notify.h
FORMS += context-notify_src/notifyprompt.ui \
    context-notify_src/changecontextdialog.ui
DESTDIR = ../bin/
RESOURCES += context-notify_src/qressources.qrc
TRANSLATIONS = context-notify_src/context-notify_fr.ts
unix:LIBS += -L../bin/lib \
    -lcontext

# install
isEmpty( PREFIX ):PREFIX = /usr
target.path = $$PREFIX/bin/
INSTALLS += target
