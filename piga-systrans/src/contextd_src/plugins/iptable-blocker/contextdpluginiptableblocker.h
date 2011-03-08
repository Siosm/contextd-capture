#ifndef CONTEXTDPLUGINIPTABLEBLOCKER_H
#define CONTEXTDPLUGINIPTABLEBLOCKER_H

#include "../abstractcontextdplugin.h"
#include "../../../xmlsp.h"
#include "rulelist.h"
#include <QReadWriteLock>

class ContextdPluginIPTableBlocker : public AbstractContextdPlugin, public XMLSP::Parser
{
private:
	static QReadWriteLock rwlock;
	RuleList rules;
	bool flushRulesWhenChangingContext;

	bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes);
	bool readConf();

public:
    ContextdPluginIPTableBlocker();
    ~ContextdPluginIPTableBlocker();

    void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINKILLER_H
