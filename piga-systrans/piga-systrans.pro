TEMPLATE = subdirs
CONFIG += ordered
win32:CONFIG -= console
SUBDIRS = src/libcontext.pro src/contextd.pro src/context-notify.pro
