#include "contextdpluginrestartevent.h"

ContextdPluginRestartEvent::ContextdPluginRestartEvent()
{

}

EventType ContextdPluginRestartEvent::type() const
{
	return "Restart Event";
}
