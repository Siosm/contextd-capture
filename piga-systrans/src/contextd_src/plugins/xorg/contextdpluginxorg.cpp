#include "contextdpluginxorg.h"
#include "../../../context-common.h"

//Events
#include "../../eventdispatcher.h"
#include "../contextdpluginnotifyevent.h"
#include "../contextdplugintransitiondemandevent.h"
#include "../contextdpluginrestartevent.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QFile>

#define XSERVER_NAME "/usr/bin/Xorg"
#define REFRESH_RATE 500

unsigned int ContextdPluginXorg::getXPID()
{
	QRegExp pid_e("^\\d+$");

	QStringList pids=QDir("/proc/").entryList(QStringList("*"), QDir::Dirs);
	for(int i=0; i<pids.size(); i++)
	{
		if(pid_e.exactMatch(pids[i]))
		{
			QString path="/proc/"+pids[i]+"/exe";

			//We want to resolve the link /proc/$pid/exe and see if it is /usr/bin/Xorg
			QString exePath=QFile::symLinkTarget(path);

			if(exePath==XSERVER_NAME)
			{
				//qDebug("PID Xserver=%s", qPrintable(pids[i]));
				return pids[i].toInt();
			}
		}
	}

	//qDebug("ContextdPluginXorg: Xorg has not been found!");

	return 0;
}

void ContextdPluginXorg::run()
{
	pid_X=getXPID();

	while(!stop_running)
	{
		sleep(1);

		unsigned int newPID=getXPID();
		if(newPID!=pid_X)
		{
			ContextdPluginRestartEvent event;

			EventDispatcher::instance().sendEvent(&event);

			pid_X=newPID;
		}
	}
}

ContextdPluginXorg::ContextdPluginXorg() : AbstractContextdPlugin("Contextd_Xorg"), pid_X(0), stop_running(false)
{
	//Start the update thread
	start();
}

ContextdPluginXorg::~ContextdPluginXorg()
{
	stop_running=true;
	wait(1500);
	terminate();
}

void ContextdPluginXorg::eventHandler(const ContextdPluginEvent* /*event*/)
{

}
