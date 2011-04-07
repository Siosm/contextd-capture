#ifndef CONTEXTDPLUGINLOGGER_H
#define CONTEXTDPLUGINLOGGER_H

#include "../abstractcontextdplugin.h"

#include "../contextdpluginnotifyevent.h"

class ContextdPluginLogger : public AbstractContextdPlugin
{
private:
	ContextdPluginNotifyEvent::Priority minP;
public:
	ContextdPluginLogger();
	~ContextdPluginLogger();

	void setVerbosity(ContextdPluginNotifyEvent::Priority minPriority);

	void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINLOGGER_H
