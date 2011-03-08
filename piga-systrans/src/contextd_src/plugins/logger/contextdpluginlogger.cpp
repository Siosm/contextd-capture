#include "contextdpluginlogger.h"

#include <syslog.h>
#include <stdio.h>

#include "../../../context-common.h"

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"
#include "../contextdpluginnotifyevent.h"

ContextdPluginLogger::ContextdPluginLogger() : AbstractContextdPlugin("Contextd_Logger")
{
	openlog("Contextd_Logger", LOG_NDELAY, LOG_DAEMON);
}

ContextdPluginLogger::~ContextdPluginLogger()
{
	closelog();
}

void ContextdPluginLogger::eventHandler(const ContextdPluginEvent* event)
{
	//Notification
	if(ContextdPluginNotifyEvent().type()==event->type())
	{
		const ContextdPluginNotifyEvent* td_event;
		td_event=static_cast<const ContextdPluginNotifyEvent*>(event);

		int level=LOG_INFO;
		QString s_level;
		switch(td_event->priority())
		{
			case ContextdPluginNotifyEvent::NOTIFY_DEBUG:
				level=LOG_DEBUG;
				s_level="Debug";
				break;
			case ContextdPluginNotifyEvent::NOTIFY_INFO:
				level=LOG_INFO;
				s_level="Info";
				break;
			case ContextdPluginNotifyEvent::NOTIFY_ERROR:
				level=LOG_ERR;
				s_level="Error";
				break;
			case ContextdPluginNotifyEvent::NOTIFY_CRITICAL:
				level=LOG_CRIT;
				s_level="Critical";
				break;
			case ContextdPluginNotifyEvent::NOTIFY_NORMAL:
			default:
				level=LOG_NOTICE;
				s_level="Normal";
				break;
		}

		syslog(level,"%s", qPrintable(td_event->message()));
		printf("%s: %s\n", qPrintable(s_level), qPrintable(td_event->message()));
	}
	//Transition Demand
	else if(ContextdPluginTransitionDemandEvent().type()==event->type())
	{
		const ContextdPluginTransitionDemandEvent* td_event;
		td_event=static_cast<const ContextdPluginTransitionDemandEvent*>(event);

		//Get variables
		pid_t pid=td_event->clientConcerned()->appPid();
		QString app=td_event->clientConcerned()->appName();
		QString localContext=td_event->clientConcerned()->currentLocalState().name();
		QString globalContext=td_event->clientConcerned()->currentGlobalState().name();
		QString scope=td_event->transitionDemanded().isALocalTransition()?"local":"global";
		QMap<QString, QString> currentState=td_event->clientConcerned()->currentState();
		QMap<QString, QString> previousState=td_event->clientConcerned()->previousState();

		//Is the transition valid ? if so, a transition has been found !
		if(td_event->transitionDemanded().isValid())
		{
			//Test ACCEPT or REFUSED
			if(td_event->demandResult()==CONTEXT_ACCEPT)
			{
				syslog(LOG_INFO,"%s (pid=%i): The %s transition from the context '%s' to the context '%s' is accepted.",
					  qPrintable(app),
					  (int)pid,
					  qPrintable(scope),
					  qPrintable(td_event->previousContext().name()),
					  qPrintable(td_event->transitionDemanded().transitTo().name())
					  );
			}
			else
			{
				syslog(LOG_INFO,"%s (pid=%i): The %s transition from the context '%s' to the context '%s' is refused.",
					  qPrintable(app),
					  (int)pid,
					  qPrintable(scope),
					  qPrintable(td_event->previousContext().name()),
					  qPrintable(td_event->transitionDemanded().transitTo().name())
					  );
			}
		}
		else
		{
			syslog(LOG_INFO,"%s (pid=%i): No transitions has been found from the context '%s'.",
				  qPrintable(app),
				  (int)pid,
				  qPrintable(td_event->previousContext().name())
				  );
		}
	}
}
