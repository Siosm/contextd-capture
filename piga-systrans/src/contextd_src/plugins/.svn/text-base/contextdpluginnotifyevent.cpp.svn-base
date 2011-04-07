#include "contextdpluginnotifyevent.h"

ContextdPluginNotifyEvent::ContextdPluginNotifyEvent()
{

}

ContextdPluginNotifyEvent::ContextdPluginNotifyEvent(Priority priority, QString msg) :
		_priority(priority), _msg(msg)
{

}

EventType ContextdPluginNotifyEvent::type() const
{
	return "Notify";
}

ContextdPluginNotifyEvent::Priority ContextdPluginNotifyEvent::priority() const
{
	return _priority;
}

QString ContextdPluginNotifyEvent::message() const
{
	return _msg;
}
