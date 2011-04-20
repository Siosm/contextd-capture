#ifndef CONTEXTDPLUGINSELINUX_H
#define CONTEXTDPLUGINSELINUX_H

#include "../abstractcontextdplugin.h"
#include "../../../xmlsp.h"
#include "selinux_module.h"

#include "../../../program.h"
#include "../../../domain.h"

#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QReadWriteLock>

class ContextdPluginSELinux : public AbstractContextdPlugin, public XMLSP::Parser
{
private:
	static QReadWriteLock rwlock;
	QMap<QPair<QString, QString>, SELinux_Module> modules;
	QMap<QString, SELinux_Module> currentModule;

	//XML Parsing
	bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes);
	void on_error(int errnr, int line, int col, const std::string& message);

	bool readConf();

	SELinux_Module getModule(QString app_name, QString context);

	void loadModule(QString app, QString path);

	void loadTheRightModule(Program app, Domain domain);

	bool exec(QString cmd);

public:
    ContextdPluginSELinux();

    void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINSELINUX_H
