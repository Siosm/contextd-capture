#ifndef CONTEXTDPLUGINLOGGER_H
#define CONTEXTDPLUGINLOGGER_H

#include "../abstractcontextdplugin.h"

class ContextdPluginLogger : public AbstractContextdPlugin
{
public:
	ContextdPluginLogger();
	~ContextdPluginLogger();

	void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINLOGGER_H
