TEMPLATE = app
TARGET = contextd
DEPENDPATH += .
INCLUDEPATH += .
QT += network xml
QT -= gui
CONFIG += qdbus

CONFIG += debug

# Input
HEADERS += context-common.h \
    contextd_src/config-daemon.h \
    contextd_src/dbus-context.h \
    contextd_src/contextclient.h \
    configuration.h \
    rule.h \
    mrule.h \
    trule.h \
	contextd_src/contextnotify.h \
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
    contextd_src/plugins/iptable-blocker/domaincachethread.h \
    contextd_src/plugins/contextdpluginnotifyevent.h \
    domain.h \
    contextd_src/plugins/killer/killingrule.h \
    contextd_src/contextd_adaptator.h \
    contextd_src/xmlcontext2map.h \
    program.h \
    contextd_src/plugins/contextdpluginreloadevent.h \
    contextd_src/plugins/selinux/contextdpluginselinux.h \
    contextd_src/plugins/selinux/selinux_module.h \
    contextd_src/plugins/contextdpluginrestartevent.h \
    contextd_src/plugins/xorg/contextdpluginxorg.h \
    contextd_src/plugins/domainholder/domainholder.h \
    transition.h \
    contextd_src/pigahandler.h \
    programaction.h \
    loop.h \
    abstractrule.h \
    programvariable.h \
    contextd_src/plugins/iptable-blocker/synchttp.h
SOURCES += contextd_src/contextd.cpp \
    contextd_src/dbus-context.cpp \
    contextd_src/contextclient.cpp \
    configuration.cpp \
    rule.cpp \
    mrule.cpp \
    trule.cpp \
	contextd_src/contextnotify.cpp \
    xmlsp.cpp \
    contextd_src/eventdispatcher.cpp \
    contextd_src/plugins/abstractcontextdplugin.cpp \
    contextd_src/plugins/logger/contextdpluginlogger.cpp \
    contextd_src/plugins/contextdplugintransitiondemandevent.cpp \
    contextd_src/plugins/killer/contextdpluginkiller.cpp \
    contextd_src/plugins/iptable-blocker/rulelist.cpp \
    contextd_src/plugins/iptable-blocker/contextdpluginiptableblocker.cpp \
    contextd_src/plugins/iptable-blocker/domaincachethread.cpp \
    contextd_src/plugins/contextdpluginnotifyevent.cpp \
    domain.cpp \
    contextd_src/plugins/killer/killingrule.cpp \
    contextd_src/contextd_adaptator.cpp \
    contextd_src/xmlcontext2map.cpp \
    program.cpp \
    contextd_src/plugins/contextdpluginreloadevent.cpp \
    contextd_src/plugins/selinux/contextdpluginselinux.cpp \
    contextd_src/plugins/selinux/selinux_module.cpp \
    contextd_src/plugins/contextdpluginrestartevent.cpp \
    contextd_src/plugins/xorg/contextdpluginxorg.cpp \
    contextd_src/plugins/domainholder/domainholder.cpp \
    transition.cpp \
    contextd_src/pigahandler.cpp \
    programaction.cpp \
    loop.cpp \
    abstractrule.cpp \
    programvariable.cpp \
    variabletypes.cpp
DESTDIR = ../bin/

# install
isEmpty( PREFIX ):PREFIX = /usr
target.path = $$PREFIX/bin/
INSTALLS += target
