#ifndef CONTEXTDPLUGINRELOADEVENT_H
#define CONTEXTDPLUGINRELOADEVENT_H

#include "contextdpluginevent.h"

class ContextdPluginReloadEvent : public ContextdPluginEvent
{
public:
    ContextdPluginReloadEvent();

    EventType type() const;
};

#endif // CONTEXTDPLUGINRELOADEVENT_H
