# -------------------------------------------------
# Project created by QtCreator 2009-04-29T19:45:45
# -------------------------------------------------
TEMPLATE = app
TARGET = context-notify
CONFIG += qdbus \
    gui \
    link_pkgconfig
PKGCONFIG = context
SOURCES += context-notify_src/context-notify.cpp \
    context-notify_src/notifyprompt.cpp \
    context-notify_src/contextListener.cpp \
    misc.cpp \
    xmlsp.cpp \
    context-notify_src/changecontextdialog.cpp \
    context-notify_src/contextlist.cpp \
    context-notify_src/updatecontext.cpp
HEADERS += context-notify_src/notifyprompt.h \
    context-notify_src/contextListener.h \
    context-common.h \
    misc.h \
    xmlsp.h \
    context-notify_src/changecontextdialog.h \
    context-notify_src/contextlist.h \
    xml_common.h \
    context-notify_src/updatecontext.h
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
