cmake_minimum_required(VERSION 2.6)

INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(FindPkgConfig)
INCLUDE(FeatureSummary)

SET(SYSTRANS_BIN_PATH "${CMAKE_INSTALL_PREFIX}/bin")
SET(SYSTRANS_LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(SYSTRANS_INCLUDE_PATH "${CMAKE_INSTALL_PREFIX}/include")
SET(SYSTRANS_PKGCFG_PATH "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")

OPTION(BUILD_LIB "Build libcontext" ON)
OPTION(BUILD_DAEMON "Build contextd" ON)
OPTION(BUILD_NOTIFY "Build context-notify" ON)
OPTION(BUILD_EDITOR "Build context-editor" OFF)

OPTION(CONTEXTD_ENABLE_SELINUX "Build contextd with SELinux support (mandatory for PIGA-IPS support)" ON)
OPTION(CONTEXTD_PLUGIN_LOGGER "Build Contextd using the logger plugin" ON)
OPTION(CONTEXTD_PLUGIN_KILLER "Build Contextd using the killer plugin" ON)
OPTION(CONTEXTD_PLUGIN_IPTABLES "Build Contextd using the iptables plugin" ON)
OPTION(CONTEXTD_PLUGIN_SELINUX "Build Contextd using the selinux plugin" ON)
OPTION(CONTEXTD_PLUGIN_XORG "Build Contextd using the xorg plugin" ON)

OPTION(USE_PIGAHANDLER "Build contextd with PIGA's support" ON)

OPTION(CONTEXTNOTIFY_LIBNOTIFY "Build Context-notify with libnotify support" ON)

#Perform some checks
PKG_SEARCH_MODULE(selinux libselinux)
IF(NOT selinux_FOUND)
	SET(CONTEXTD_ENABLE_SELINUX OFF)
	SET(CONTEXTD_PLUGIN_SELINUX OFF)
	SET(USE_PIGAHANDLER OFF)
ELSE(NOT selinux_FOUND)
	INCLUDE_DIRECTORIES(${selinux_INCLUDE_DIRS})
	LINK_DIRECTORIES(${selinux_LIBRARY_DIRS})
ENDIF(NOT selinux_FOUND)

FIND_PACKAGE(Qt4 REQUIRED)

# Set some default values
IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

IF(NOT DEBUG)
	ADD_DEFINITIONS(-DNDEBUG -O2 -DQT_NO_DEBUG)
ELSE(NOT DEBUG)
	ADD_DEFINITIONS(-g)
ENDIF(NOT DEBUG)

SET(COMMON_SOURCE		"${CMAKE_CURRENT_SOURCE_DIR}/abstractrule.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/configuration.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/domain.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/loop.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/misc.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/mrule.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/program.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/programvariable.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/variabletypes.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/programaction.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/programaction.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/rule.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/transition.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/trule.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/xmlsp.cpp"
						)

SET(COMMON_MOC			"${CMAKE_CURRENT_SOURCE_DIR}/configuration.h")
QT4_WRAP_CPP(COMMON_SOURCE ${COMMON_MOC})


IF(BUILD_LIB)
	SET(QT_USE_QTGUI FALSE)
	SET (QT_USE_QTDBUS TRUE)
	INCLUDE(${QT_USE_FILE})

	SET(LIBCONTEXT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libcontext_src")

	INCLUDE_DIRECTORIES(${LIBCONTEXT_SRC_DIR})
	INCLUDE_DIRECTORIES(${QT_INCLUDES})
	
	FILE(GLOB_RECURSE libcontext_source "${LIBCONTEXT_SRC_DIR}/*.cpp")
	SET(libcontext_moc	"${LIBCONTEXT_SRC_DIR}/contextd_communicator.h"
						"${LIBCONTEXT_SRC_DIR}/dbuscallback.h")

	QT4_WRAP_CPP(libcontext_source ${libcontext_moc})

	ADD_LIBRARY(context SHARED ${libcontext_source})
	TARGET_LINK_LIBRARIES(context ${QT_LIBRARIES})
	SET_TARGET_PROPERTIES(context PROPERTIES COMPILE_FLAGS "-pthread" SOVERSION 0.2.0)
	
	INSTALL(TARGETS context DESTINATION ${SYSTRANS_LIB_PATH})
	INSTALL(FILES "${LIBCONTEXT_SRC_DIR}/libcontext.h" DESTINATION ${SYSTRANS_INCLUDE_PATH})
	INSTALL(FILES "${LIBCONTEXT_SRC_DIR}/context.pc" DESTINATION ${SYSTRANS_PKGCFG_PATH})
ENDIF(BUILD_LIB)


IF(BUILD_NOTIFY)
	SET(QT_USE_QTGUI TRUE)
        SET (QT_USE_QTDBUS TRUE)
	INCLUDE(${QT_USE_FILE})

	SET(NOTIFY_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/context-notify_src")

	INCLUDE_DIRECTORIES(${NOTIFY_SRC_DIR})
	INCLUDE_DIRECTORIES(${QT_INCLUDES})
	
	PKG_SEARCH_MODULE(notify libnotify)
	IF(NOT notify_FOUND)
		SET(CONTEXTNOTIFY_LIBNOTIFY OFF)
	ELSE(NOT notify_FOUND)
		INCLUDE_DIRECTORIES(${notify_INCLUDE_DIRS})
		LINK_DIRECTORIES(${notify_LIBRARY_DIRS})
		ADD_DEFINITIONS("-DHAVE_LIBNOTIFY")
	ENDIF(NOT notify_FOUND)

	SET(notify_source	"${NOTIFY_SRC_DIR}/context-notify.cpp"
						"${NOTIFY_SRC_DIR}/changecontextdialog.cpp"
						"${NOTIFY_SRC_DIR}/domainlist.cpp"
						"${NOTIFY_SRC_DIR}/contextListener.cpp"
						"${NOTIFY_SRC_DIR}/notifyprompt.cpp"
						"${NOTIFY_SRC_DIR}/updatedomain.cpp"
						"${NOTIFY_SRC_DIR}/notify.c"
						)

	SET(notify_moc		"${NOTIFY_SRC_DIR}/changecontextdialog.h"
						"${NOTIFY_SRC_DIR}/contextListener.h"
						"${NOTIFY_SRC_DIR}/notifyprompt.h"
						"${NOTIFY_SRC_DIR}/updatedomain.h"
						)

	QT4_WRAP_CPP(notify_source ${notify_moc})

	SET(notify_uis    	"${NOTIFY_SRC_DIR}/changecontextdialog.ui"
						"${NOTIFY_SRC_DIR}/notifyprompt.ui")

	QT4_WRAP_UI(notify_source ${notify_uis})
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

	SET(notify_resources "${NOTIFY_SRC_DIR}/qressources.qrc")
	QT4_ADD_RESOURCES(notify_source ${notify_resources})

	SET(notify_translations "${NOTIFY_SRC_DIR}/context-notify_fr.ts")
	QT4_ADD_TRANSLATION(notify_source ${notify_translations})

	ADD_EXECUTABLE(context-notify ${COMMON_SOURCE} ${notify_source})
	TARGET_LINK_LIBRARIES(context-notify context)
	TARGET_LINK_LIBRARIES(context-notify ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(context-notify ${notify_LIBRARIES})
	TARGET_LINK_LIBRARIES(context-notify ${glib_LIBRARIES})

	INSTALL(TARGETS context-notify DESTINATION ${SYSTRANS_BIN_PATH})
ENDIF(BUILD_NOTIFY)



IF(BUILD_EDITOR)
	SET(QT_USE_QTGUI TRUE)
	SET(QT_USE_QTSVG TRUE)
	SET(QT_USE_QTDBUS FALSE)
	INCLUDE(${QT_USE_FILE})
	SET(EDITOR_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/context-editor_src")

	PKG_SEARCH_MODULE(context context)
	INCLUDE_DIRECTORIES(${context_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${EDITOR_SRC_DIR})
	INCLUDE_DIRECTORIES(${QT_INCLUDES})
	LINK_DIRECTORIES(${context_LIBRARY_DIRS})


	PKG_SEARCH_MODULE(gvc REQUIRED libgvc)
	INCLUDE_DIRECTORIES(${gvc_INCLUDE_DIRS})
	LINK_DIRECTORIES(${gvc_LIBRARY_DIRS})

	UNSET(graph_FOUND)
	PKG_CHECK_MODULES(graph REQUIRED libgraph)
	IF(${graph_VERSION} VERSION_LESS "2.26.0")
		ADD_DEFINITIONS("-D__LEGACY_GRAPHVIZ__")
		MESSAGE("Using legacy GraphViz (${graph_VERSION})")
	ELSE(${graph_VERSION} VERSION_LESS "2.26.0")
		MESSAGE("Using recent GraphViz (${graph_VERSION})")
	ENDIF(${graph_VERSION} VERSION_LESS "2.26.0")
	INCLUDE_DIRECTORIES(${graph_INCLUDE_DIRS})
	LINK_DIRECTORIES(${graph_LIBRARY_DIRS})


	SET(editor_source	"${EDITOR_SRC_DIR}/main.cpp"
						"${EDITOR_SRC_DIR}/abstractruledialog.cpp"
						"${EDITOR_SRC_DIR}/domaininfodock.cpp"
						"${EDITOR_SRC_DIR}/domainloopstreewidget.cpp"
						"${EDITOR_SRC_DIR}/domainselectionwidget.cpp"
						"${EDITOR_SRC_DIR}/sortedrulestreewidget.cpp"
						"${EDITOR_SRC_DIR}/domaininfodock.cpp"
						"${EDITOR_SRC_DIR}/domainloopstreewidget.cpp"
						"${EDITOR_SRC_DIR}/appinfodock.cpp"
						"${EDITOR_SRC_DIR}/abstractruletreewidget.cpp"
						"${EDITOR_SRC_DIR}/browserdock.cpp"
						"${EDITOR_SRC_DIR}/domaindialog.cpp"
						"${EDITOR_SRC_DIR}/configurationstack.cpp"
						"${EDITOR_SRC_DIR}/appexportdialog.cpp"
						"${EDITOR_SRC_DIR}/editorconfig.cpp"
						"${EDITOR_SRC_DIR}/graphicsdomain.cpp"
						"${EDITOR_SRC_DIR}/graphicsview.cpp"
						"${EDITOR_SRC_DIR}/graphicssvgitem.cpp"
						"${EDITOR_SRC_DIR}/graphicsarrow.cpp"
						"${EDITOR_SRC_DIR}/graphicsarrowprogram.cpp"
						"${EDITOR_SRC_DIR}/typedlineedit.cpp"
						"${EDITOR_SRC_DIR}/sessionrestoredialog.cpp"
						"${EDITOR_SRC_DIR}/graphicspopup.cpp"
						"${EDITOR_SRC_DIR}/selectdefaultdomaindialog.cpp"
						"${EDITOR_SRC_DIR}/typeddelegate.cpp"
						"${EDITOR_SRC_DIR}/appdialog.cpp"
						"${EDITOR_SRC_DIR}/actiondialog.cpp"
						"${EDITOR_SRC_DIR}/gvgraph.cpp"
						"${EDITOR_SRC_DIR}/loopdialog.cpp"
						"${EDITOR_SRC_DIR}/loopdialog.cpp"
						"${EDITOR_SRC_DIR}/loopdialog.cpp"
						"${EDITOR_SRC_DIR}/mainwindow.cpp"
						"${EDITOR_SRC_DIR}/ruledialog.cpp"
						"${EDITOR_SRC_DIR}/mruletreewidget.cpp"
						"${EDITOR_SRC_DIR}/mruledialog.cpp"
						"${EDITOR_SRC_DIR}/transitioninfodock.cpp"
						"${EDITOR_SRC_DIR}/settingsdialog.cpp"
						"${EDITOR_SRC_DIR}/abstractruleinfodock.cpp"
						"${EDITOR_SRC_DIR}/messagearea.cpp"
						"${EDITOR_SRC_DIR}/warning.cpp"
						"${EDITOR_SRC_DIR}/editordock.cpp"
						"${EDITOR_SRC_DIR}/colors.cpp"
						"${EDITOR_SRC_DIR}/captiondialog.cpp"
						)
						
	SET(editor_moc		"${EDITOR_SRC_DIR}/transitioninfodock.h"
						"${EDITOR_SRC_DIR}/ruledialog.h"
						"${EDITOR_SRC_DIR}/mruletreewidget.h"
						"${EDITOR_SRC_DIR}/mruledialog.h"
						"${EDITOR_SRC_DIR}/mainwindow.h"
						"${EDITOR_SRC_DIR}/loopdialog.h"
						"${EDITOR_SRC_DIR}/domainselectionwidget.h"
						"${EDITOR_SRC_DIR}/abstractruledialog.h"
						"${EDITOR_SRC_DIR}/domaininfodock.h"
						"${EDITOR_SRC_DIR}/domainloopstreewidget.h"
						"${EDITOR_SRC_DIR}/sortedrulestreewidget.h"
						"${EDITOR_SRC_DIR}/appinfodock.h"
						"${EDITOR_SRC_DIR}/browserdock.h"
						"${EDITOR_SRC_DIR}/abstractruletreewidget.h"
						"${EDITOR_SRC_DIR}/domaindialog.h"
						"${EDITOR_SRC_DIR}/configurationstack.h"
						"${EDITOR_SRC_DIR}/appexportdialog.h"
						"${EDITOR_SRC_DIR}/editorconfig.h"
						"${EDITOR_SRC_DIR}/graphicsdomain.h"
						"${EDITOR_SRC_DIR}/graphicsview.h"
						"${EDITOR_SRC_DIR}/sessionrestoredialog.h"
						"${EDITOR_SRC_DIR}/graphicspopup.h"
						"${EDITOR_SRC_DIR}/selectdefaultdomaindialog.h"
						"${EDITOR_SRC_DIR}/graphicssvgitem.h"
						"${EDITOR_SRC_DIR}/graphicsarrow.h"
						"${EDITOR_SRC_DIR}/graphicsarrowprogram.h"
						"${EDITOR_SRC_DIR}/typedlineedit.h"
						"${EDITOR_SRC_DIR}/typeddelegate.h"
						"${EDITOR_SRC_DIR}/appdialog.h"
						"${EDITOR_SRC_DIR}/actiondialog.h"
						"${EDITOR_SRC_DIR}/settingsdialog.h"
						"${EDITOR_SRC_DIR}/abstractruleinfodock.h"
						"${EDITOR_SRC_DIR}/messagearea.h"
						"${EDITOR_SRC_DIR}/warning.h"
						"${EDITOR_SRC_DIR}/editordock.h"
						"${EDITOR_SRC_DIR}/colors.h"
						"${EDITOR_SRC_DIR}/captiondialog.h"
						)

	QT4_WRAP_CPP(editor_source ${editor_moc})
	
	QT4_ADD_RESOURCES(editor_source "${EDITOR_SRC_DIR}/Icons.qrc")
	
	SET(editor_uis    	"${EDITOR_SRC_DIR}/abstractruledialog.ui"
						"${EDITOR_SRC_DIR}/domaininfodock.ui"
						"${EDITOR_SRC_DIR}/domainloopstreewidget.ui"
						"${EDITOR_SRC_DIR}/domainselectionwidget.ui"
						"${EDITOR_SRC_DIR}/sortedrulestreewidget.ui"
						"${EDITOR_SRC_DIR}/mainwindow.ui"
						"${EDITOR_SRC_DIR}/mruletreewidget.ui"
						"${EDITOR_SRC_DIR}/mruledialog.ui"
						"${EDITOR_SRC_DIR}/transitioninfodock.ui"
						"${EDITOR_SRC_DIR}/appinfodock.ui"
						"${EDITOR_SRC_DIR}/browserdock.ui"
						"${EDITOR_SRC_DIR}/abstractruletreewidget.ui"
						"${EDITOR_SRC_DIR}/domaindialog.ui"
						"${EDITOR_SRC_DIR}/appexportdialog.ui"
						"${EDITOR_SRC_DIR}/appdialog.ui"
						"${EDITOR_SRC_DIR}/actiondialog.ui"
						"${EDITOR_SRC_DIR}/settingsdialog.ui"
						"${EDITOR_SRC_DIR}/selectdefaultdomaindialog.ui"
						"${EDITOR_SRC_DIR}/abstractruleinfodock.ui
						"${EDITOR_SRC_DIR}/messagearea.ui"
						"${EDITOR_SRC_DIR}/captiondialog.ui"
						)

	QT4_WRAP_UI(editor_source ${editor_uis})
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

	ADD_EXECUTABLE(context-editor ${COMMON_SOURCE} ${editor_source})
	TARGET_LINK_LIBRARIES(context-editor ${gvc_LIBRARIES})
	TARGET_LINK_LIBRARIES(context-editor ${graph_LIBRARIES})
	TARGET_LINK_LIBRARIES(context-editor ${QT_LIBRARIES})

	INSTALL(TARGETS context-editor DESTINATION ${SYSTRANS_BIN_PATH})
ENDIF(BUILD_EDITOR)

IF(BUILD_DAEMON)
	SET(QT_USE_QTGUI FALSE)
	SET(QT_USE_QTNETWORK TRUE)
        SET(QT_USE_QTDBUS true)
	INCLUDE(${QT_USE_FILE})	

	SET(DAEMON_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/contextd_src")
	SET(PLUGINS_SRC_DIR "${DAEMON_SRC_DIR}/plugins")

	INCLUDE_DIRECTORIES(${DAEMON_SRC_DIR})
	INCLUDE_DIRECTORIES(${QT_INCLUDES})
	LINK_DIRECTORIES(${DAEMON_SRC_DIR})
	LINK_DIRECTORIES(${PLUGINS_SRC_DIR})

	SET(daemon_source	"${DAEMON_SRC_DIR}/contextclient.cpp"
						"${DAEMON_SRC_DIR}/contextd.cpp"
						"${DAEMON_SRC_DIR}/dbus-context.cpp"
						"${DAEMON_SRC_DIR}/eventdispatcher.cpp"
						"${DAEMON_SRC_DIR}/pigahandler.cpp"
						"${DAEMON_SRC_DIR}/contextnotify.cpp"
						"${DAEMON_SRC_DIR}/xmlcontext2map.cpp"
						"${DAEMON_SRC_DIR}/contextd_adaptator.cpp"
						"${PLUGINS_SRC_DIR}/abstractcontextdplugin.cpp"
						"${PLUGINS_SRC_DIR}/contextdpluginnotifyevent.cpp"
						"${PLUGINS_SRC_DIR}/contextdpluginreloadevent.cpp"
						"${PLUGINS_SRC_DIR}/contextdpluginrestartevent.cpp"
						"${PLUGINS_SRC_DIR}/contextdplugintransitiondemandevent.cpp"
						"${PLUGINS_SRC_DIR}/domainholder/domainholder.cpp"
						"${PLUGINS_SRC_DIR}/iptable-blocker/rulelist.cpp"
						"${PLUGINS_SRC_DIR}/iptable-blocker/contextdpluginiptableblocker.cpp"
						"${PLUGINS_SRC_DIR}/iptable-blocker/domaincachethread.cpp"
						"${PLUGINS_SRC_DIR}/killer/contextdpluginkiller.cpp"
						"${PLUGINS_SRC_DIR}/killer/killingrule.cpp"
						"${PLUGINS_SRC_DIR}/logger/contextdpluginlogger.cpp"
						"${PLUGINS_SRC_DIR}/selinux/contextdpluginselinux.cpp"
						"${PLUGINS_SRC_DIR}/selinux/selinux_module.cpp"
						"${PLUGINS_SRC_DIR}/xorg/contextdpluginxorg.cpp"
						)

	SET(daemon_moc		"${DAEMON_SRC_DIR}/dbus-context.h"
						"${DAEMON_SRC_DIR}/eventdispatcher.h"
						"${DAEMON_SRC_DIR}/pigahandler.h"
						"${DAEMON_SRC_DIR}/contextd_adaptator.h"
						"${PLUGINS_SRC_DIR}/domainholder/domainholder.h"
						"${PLUGINS_SRC_DIR}/iptable-blocker/rulelist.h"
						"${PLUGINS_SRC_DIR}/iptable-blocker/synchttp.h"
						"${PLUGINS_SRC_DIR}/xorg/contextdpluginxorg.h"
						)

	IF(NOT CONTEXTD_PLUGIN_LOGGER)
		ADD_DEFINITIONS("-DPLUGIN_DISABLE_LOGGER")
	ENDIF()

	IF(NOT CONTEXTD_PLUGIN_KILLER)
		ADD_DEFINITIONS("-DPLUGIN_DISABLE_KILLER")
	ENDIF()

	IF(NOT CONTEXTD_PLUGIN_IPTABLES)
		ADD_DEFINITIONS("-DPLUGIN_DISABLE_IPTABLES")
	ENDIF()

	IF(NOT CONTEXTD_PLUGIN_SELINUX)
		ADD_DEFINITIONS("-DPLUGIN_DISABLE_SELINUX")
	ENDIF()

	IF(NOT CONTEXTD_PLUGIN_XORG)
		ADD_DEFINITIONS("-DPLUGIN_DISABLE_XORG")
	ENDIF()

	IF(NOT USE_PIGAHANDLER)
		ADD_DEFINITIONS("-DDISABLE_PIGAHANDLER")
	ENDIF()

	QT4_WRAP_CPP(daemon_source ${daemon_moc})

	ADD_EXECUTABLE(contextd ${COMMON_SOURCE} ${daemon_source})
	TARGET_LINK_LIBRARIES(contextd ${selinux_LIBRARIES})
	TARGET_LINK_LIBRARIES(contextd ${QT_LIBRARIES})
	INSTALL(TARGETS contextd DESTINATION ${SYSTRANS_BIN_PATH})
	IF (CONTEXTD_ENABLE_SELINUX)
		SET_TARGET_PROPERTIES(contextd PROPERTIES COMPILE_FLAGS "-DHAS_SELINUX")
	ENDIF (CONTEXTD_ENABLE_SELINUX)

ENDIF(BUILD_DAEMON)
