#ifndef CONTEXTDPLUGINXORG_H
#define CONTEXTDPLUGINXORG_H

#include "../abstractcontextdplugin.h"

#include <QtCore/QThread>

class ContextdPluginXorg : public QThread, public AbstractContextdPlugin
{
Q_OBJECT
private:
	unsigned int pid_X;

	unsigned int getXPID();

	bool stop_running;

	void run();

public:
    ContextdPluginXorg();
    ~ContextdPluginXorg();

    void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINXORG_H
