#include "contextdpluginreloadevent.h"

ContextdPluginReloadEvent::ContextdPluginReloadEvent()
{

}

EventType ContextdPluginReloadEvent::type() const
{
	return QString("Reload Event");
}
