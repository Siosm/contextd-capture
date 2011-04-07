#ifndef CONTEXTDPLUGINEVENT_H
#define CONTEXTDPLUGINEVENT_H

#include <QMap>
#include <QMutex>
#include <QString>

typedef QString EventType;

class ContextdPluginEvent
{
public:
	virtual EventType type() const=0;
};

#endif // CONTEXTDPLUGINEVENT_H
