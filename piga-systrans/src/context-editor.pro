#-------------------------------------------------
#
# Project created by QtCreator 2010-06-07T11:39:49
#
#-------------------------------------------------

QT       += core gui svg

CONFIG += qdbus debug link_pkgconfig

TARGET = context-editor
TEMPLATE = app

PKGCONFIG = libgraph libgvc

INCLUDEPATH += librecompare_src

SOURCES += \
    trule.cpp \
    transition.cpp \
    rule.cpp \
    programvariable.cpp \
    programaction.cpp \
    program.cpp \
    mrule.cpp \
    abstractrule.cpp \
    configuration.cpp \
    contextd_interface.cpp \
    domain.cpp \
    loop.cpp \
	misc.cpp \
	context-editor_src/transitioninfodock.cpp \
    context-editor_src/ruledialog.cpp \
    context-editor_src/mruletreewidget.cpp \
    context-editor_src/mruledialog.cpp \
    context-editor_src/mainwindow.cpp \
    context-editor_src/main.cpp \
    context-editor_src/loopdialog.cpp \
    context-editor_src/domainselectionwidget.cpp \
    context-editor_src/abstractruledialog.cpp \
    context-editor_src/domaininfodock.cpp \
    context-editor_src/domainloopstreewidget.cpp \
    context-editor_src/sortedrulestreewidget.cpp \
    context-editor_src/appinfodock.cpp \
    context-editor_src/browserdock.cpp \
    context-editor_src/abstractruletreewidget.cpp \
	context-editor_src/domaindialog.cpp \
    context-editor_src/configurationstack.cpp \
    context-editor_src/appexportdialog.cpp \
    context-editor_src/editorconfig.cpp \
    context-editor_src/graphicsdomain.cpp \
    context-editor_src/graphicsview.cpp \
    context-editor_src/graphicssvgitem.cpp \
    context-editor_src/graphicsarrow.cpp \
    context-editor_src/graphicsarrowprogram.cpp \
    context-editor_src/typedlineedit.cpp \
    context-editor_src/typeddelegate.cpp \
    variabletypes.cpp \
	context-editor_src/appdialog.cpp \
    context-editor_src/actiondialog.cpp \
    context-editor_src/gvgraph.cpp \
    context-editor_src/settingsdialog.cpp \
    context-editor_src/sessionrestoredialog.cpp \
    context-editor_src/graphicspopup.cpp \
    context-editor_src/selectdefaultdomaindialog.cpp \
    context-editor_src/abstractruleinfodock.cpp \
    context-editor_src/messagearea.cpp \
    context-editor_src/warning.cpp \
    context-editor_src/editordock.cpp \
	context-editor_src/colors.cpp \
	context-editor_src/captiondialog.cpp

HEADERS  += \
    xml_common.h \
    trule.h \
    transition.h \
    rule.h \
    programvariable.h \
    programaction.h \
    program.h \
    mrule.h \
    misc.h \
    loop.h \
    domain.h \
    contextd_interface.h \
    context-common.h \
    configuration.h \
	abstractrule.h \
	context-editor_src/transitioninfodock.h \
    context-editor_src/ruledialog.h \
    context-editor_src/mruletreewidget.h \
    context-editor_src/mruledialog.h \
    context-editor_src/mainwindow.h \
    context-editor_src/loopdialog.h \
    context-editor_src/domainselectionwidget.h \
    context-editor_src/abstractruledialog.h \
    context-editor_src/domaininfodock.h \
    context-editor_src/domainloopstreewidget.h \
    context-editor_src/sortedrulestreewidget.h \
    context-editor_src/appinfodock.h \
    context-editor_src/browserdock.h \
    context-editor_src/abstractruletreewidget.h \
	context-editor_src/domaindialog.h \
    context-editor_src/configurationstack.h \
    context-editor_src/appexportdialog.h \
	context-editor_src/editorconfig.h \
	context-editor_src/graphicsdomain.h \
	context-editor_src/graphicsview.h \
	context-editor_src/graphicssvgitem.h \
	context-editor_src/graphicsarrow.h \
	context-editor_src/graphicsarrowprogram.h \
	context-editor_src/typedlineedit.h \
    context-editor_src/typeddelegate.h \
    context-editor_src/appdialog.h \
    context-editor_src/actiondialog.h \
    context-editor_src/gvgraph.h \
    context-editor_src/settingsdialog.h \
    context-editor_src/sessionrestoredialog.h \
    context-editor_src/graphicspopup.h \
    context-editor_src/selectdefaultdomaindialog.h \
    context-editor_src/abstractruleinfodock.h \
    context-editor_src/messagearea.h \
    context-editor_src/warning.h \
    context-editor_src/editordock.h \
	context-editor_src/colors.h \
	context-editor_src/captiondialog.h

FORMS    += \
	context-editor_src/transitioninfodock.ui \
    context-editor_src/mruletreewidget.ui \
    context-editor_src/mruledialog.ui \
    context-editor_src/mainwindow.ui \
    context-editor_src/domainselectionwidget.ui \
    context-editor_src/abstractruledialog.ui \
    context-editor_src/domaininfodock.ui \
    context-editor_src/domainloopstreewidget.ui \
    context-editor_src/sortedrulestreewidget.ui \
    context-editor_src/appinfodock.ui \
    context-editor_src/browserdock.ui \
    context-editor_src/abstractruletreewidget.ui \
    context-editor_src/domaindialog.ui \
    context-editor_src/appexportdialog.ui \
    context-editor_src/appdialog.ui \
    context-editor_src/actiondialog.ui \
    context-editor_src/settingsdialog.ui \
    context-editor_src/selectdefaultdomaindialog.ui \
    context-editor_src/abstractruleinfodock.ui \
    context-editor_src/messagearea.ui \
    context-editor_src/captiondialog.ui

RESOURCES += \
    context-editor_src/Icons.qrc
