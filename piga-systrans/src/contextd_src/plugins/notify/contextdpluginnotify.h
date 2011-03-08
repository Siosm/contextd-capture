#ifndef CONTEXTDPLUGINNOTIFY_H
#define CONTEXTDPLUGINNOTIFY_H

#include "../abstractcontextdplugin.h"
#include "../contextdpluginnotifyevent.h"

class ContextdPluginNotify : public AbstractContextdPlugin
{
private:
	bool on_context_changed(const QString& scope, const QString& app_name, Context context);
	bool notifyEvent(const ContextdPluginNotifyEvent* event);

	ContextdPluginNotifyEvent::Priority verbosity;

public:
    ContextdPluginNotify();
    ~ContextdPluginNotify();

    void setVerbosityLevel(ContextdPluginNotifyEvent::Priority);
    ContextdPluginNotifyEvent::Priority verbosityLevel();

    void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINKILLER_H
