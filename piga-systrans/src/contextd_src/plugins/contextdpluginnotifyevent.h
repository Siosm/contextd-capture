#ifndef CONTEXTDPLUGINNOTIFYEVENT_H
#define CONTEXTDPLUGINNOTIFYEVENT_H

#include "contextdpluginevent.h"
#include "../contextclient.h"

class ContextdPluginNotifyEvent : public ContextdPluginEvent
{
public:
	enum Priority {NOTIFY_DEBUG=0, NOTIFY_INFO=1, NOTIFY_NORMAL=2, NOTIFY_ERROR=3, NOTIFY_CRITICAL=4 };

	ContextdPluginNotifyEvent();
	ContextdPluginNotifyEvent(Priority priority, QString msg);

	EventType type() const;

	Priority priority() const;
	QString message() const;

private:
	Priority _priority;
	QString _msg;
};

#endif // CONTEXTDPLUGINGLOBALCONTEXTCHANGEDEVENT_H
