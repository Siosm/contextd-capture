TEMPLATE = app
TARGET = contextd
DEPENDPATH += .
INCLUDEPATH += .
QT -= gui
CONFIG += qdbus \
    link_pkgconfig
PKGCONFIG = libnotify \
    glib

# Input
HEADERS += context-common.h \
    contextd_src/config-daemon.h \
    contextd_src/dbus-context.h \
    contextd_src/contextclient.h \
    contextd_src/transitionrules.h \
    contextd_src/transition.h \
    contextd_src/promptuser.h \
    xmlsp.h \
    xml_common.h \
    contextd_src/eventdispatcher.h \
    contextd_src/plugins/abstractcontextdplugin.h \
    contextd_src/plugins/logger/contextdpluginlogger.h \
    contextd_src/plugins/contextdpluginevent.h \
    contextd_src/plugins/contextdplugintransitiondemandevent.h \
    contextd_src/plugins/killer/contextdpluginkiller.h \
    contextd_src/plugins/iptable-blocker/contextdpluginiptableblocker.h \
    contextd_src/plugins/iptable-blocker/rulelist.h \
    contextd_src/plugins/notify/contextdpluginnotify.h \
    contextd_src/plugins/contextdpluginnotifyevent.h \
    contextd_src/plugins/notify/contextdpluginnotify-impl.h \
    context.h \
    contextd_src/plugins/killer/killingrule.h \
    contextd_adaptator.h \
    contextd_src/xmlcontext2map.h \
    contextd_src/program.h \
    contextd_src/plugins/contextdpluginreloadevent.h \
    contextd_src/plugins/selinux/contextdpluginselinux.h \
    contextd_src/plugins/selinux/selinux_module.h \
    contextd_src/plugins/contextdpluginrestartevent.h \
    contextd_src/plugins/xorg/contextdpluginxorg.h \
	contextd_src/plugins/contextholder/contextholder.h
SOURCES += contextd_src/contextd.cpp \
    contextd_src/dbus-context.cpp \
    contextd_src/contextclient.cpp \
    contextd_src/transitionrules.cpp \
    contextd_src/transition.cpp \
    contextd_src/promptuser.cpp \
    xmlsp.cpp \
    contextd_src/eventdispatcher.cpp \
    contextd_src/plugins/abstractcontextdplugin.cpp \
    contextd_src/plugins/logger/contextdpluginlogger.cpp \
    contextd_src/plugins/contextdplugintransitiondemandevent.cpp \
    contextd_src/plugins/killer/contextdpluginkiller.cpp \
    contextd_src/plugins/iptable-blocker/rulelist.cpp \
    contextd_src/plugins/iptable-blocker/contextdpluginiptableblocker.cpp \
    contextd_src/plugins/notify/contextdpluginnotify.cpp \
    contextd_src/plugins/contextdpluginnotifyevent.cpp \
    contextd_src/plugins/notify/contextdpluginnotify-impl.c \
    context.cpp \
    contextd_src/plugins/killer/killingrule.cpp \
    contextd_adaptator.cpp \
    contextd_src/xmlcontext2map.cpp \
    contextd_src/program.cpp \
    contextd_src/plugins/contextdpluginreloadevent.cpp \
    contextd_src/plugins/selinux/contextdpluginselinux.cpp \
    contextd_src/plugins/selinux/selinux_module.cpp \
    contextd_src/plugins/contextdpluginrestartevent.cpp \
    contextd_src/plugins/xorg/contextdpluginxorg.cpp \
	contextd_src/plugins/contextholder/contextholder.cpp
DESTDIR = ../bin/

# install
isEmpty( PREFIX ):PREFIX = /usr
target.path = $$PREFIX/bin/
INSTALLS += target
