#ifndef CONTEXTDPLUGINEVENT_H
#define CONTEXTDPLUGINEVENT_H

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QString>

typedef QString EventType;

class ContextdPluginEvent
{
public:
	virtual EventType type() const=0;
};

#endif // CONTEXTDPLUGINEVENT_H
