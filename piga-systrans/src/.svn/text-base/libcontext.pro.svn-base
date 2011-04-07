TEMPLATE = lib
PKGCONFIG = dbus-glib-1
CXXFLAGS += -pthread
TARGET = context
VERSION += 0.2.0
DEPENDPATH += .
INCLUDEPATH += . \
    libcontext_src
QT -= gui
CONFIG += qdbus \
    dll \
    link_pkgconfig

# Input
HEADERS += context-common.h \
    libcontext_src/libcontext.h \
    libcontext_src/dbuscallback.h \
    libcontext_src/contextd_communicator.h
SOURCES += libcontext_src/libcontext.cpp \
    libcontext_src/dbuscallback.cpp \
    libcontext_src/contextd_communicator.cpp
DESTDIR = ../bin/lib/

# DBUS_ADAPTORS += com.piga.contextd.xml
# DBUS_INTERFACES += com.piga.contextd.xml
# install
isEmpty( PREFIX ):PREFIX = /usr
target.path = $$PREFIX/lib/
header.files = libcontext_src/libcontext.h
header.path = $$PREFIX/include/
pkgconfig.files = libcontext_src/context.pc
pkgconfig.path = $$PREFIX/lib/pkgconfig/
INSTALLS += target \
    header \
    pkgconfig
