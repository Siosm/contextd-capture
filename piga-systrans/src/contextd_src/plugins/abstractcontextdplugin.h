#ifndef ABSTRACTCONTEXTDPLUGIN_H
#define ABSTRACTCONTEXTDPLUGIN_H

#include "contextdpluginevent.h"

#include <QString>

class AbstractContextdPlugin
{
private:
	QString plugin_name;

public:
	AbstractContextdPlugin(QString plugin_name);

	QString name();

	virtual void eventHandler(const ContextdPluginEvent* event)=0;
};

#endif // ABSTRACTCONTEXTDPLUGIN_H
