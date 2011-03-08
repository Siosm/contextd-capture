#ifndef CONTEXTDPLUGINRESTARTEVENT_H
#define CONTEXTDPLUGINRESTARTEVENT_H

#include "contextdpluginevent.h"

class ContextdPluginRestartEvent : public ContextdPluginEvent
{
public:
    ContextdPluginRestartEvent();

    EventType type() const;
};

#endif // CONTEXTDPLUGINRESTARTEVENT_H
