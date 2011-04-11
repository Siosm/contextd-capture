#include "contextdpluginlogger.h"

#include <syslog.h>
#include <stdio.h>

#include "../../../context-common.h"

#include "../../contextnotify.h"

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"
#include "../contextdpluginrestartevent.h"

ContextdPluginLogger::ContextdPluginLogger() : AbstractContextdPlugin("Contextd_Logger"), minP(ContextdPluginNotifyEvent::NOTIFY_INFO)
{
	openlog("Contextd_Logger", LOG_NDELAY, LOG_DAEMON);

#ifndef NDEBUG
	setVerbosity(ContextdPluginNotifyEvent::NOTIFY_DEBUG);
#endif
}

ContextdPluginLogger::~ContextdPluginLogger()
{
	closelog();
}

void ContextdPluginLogger::setVerbosity(ContextdPluginNotifyEvent::Priority minPriority)
{
	minP=minPriority;
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

		// Only log events that have a priority superior to the minimum priority
		if(td_event->priority()>=minP)
		{
			syslog(level,"%s", qPrintable(td_event->message()));
			qDebug("%s: %s", qPrintable(s_level), qPrintable(td_event->message()));
		}

		// Log critical events to context-notify
		if(td_event->priority()>=LOG_CRIT)
			ContextNotify::notify(td_event->message(), 2);
	}
	//Transition Demand
	else if(ContextdPluginTransitionDemandEvent().type()==event->type())
	{
		const ContextdPluginTransitionDemandEvent* td_event;
		td_event=static_cast<const ContextdPluginTransitionDemandEvent*>(event);

		//Get variables
		pid_t pid=td_event->clientConcerned()->appPid();
		QString app=td_event->clientConcerned()->program().name();
		QString globalContext=td_event->clientConcerned()->currentDomain().name();
		QMap<QString, QString> currentState=td_event->clientConcerned()->currentState();
		QMap<QString, QString> previousState=td_event->clientConcerned()->previousState();

		//Create a string from the current context
		QString context;
		QMapIterator<QString, QString> i=td_event->clientConcerned()->currentState();
		while(i.hasNext())
		{
			i.next();
			if(context!=QString())
				context+=", ";
			context+=QString("\"%1\"=\"%2\"").arg(i.key(), i.value());
		}

		//Is the transition valid ? if so, a transition has been found !
		if(td_event->transitionDemanded().isValid())
		{
			//Test ACCEPT or REFUSED
			if(td_event->demandResult()==CONTEXT_ACCEPT)
			{
				syslog(LOG_INFO,"%s (pid=%i): The transition from the domain '%s' to the domain '%s' is accepted. Context (%s).",
					  qPrintable(app),
					  (int)pid,
					  qPrintable(td_event->previousContext().name()),
					  qPrintable(td_event->transitionDemanded().transitTo().name()),
					  qPrintable(context)
					  );
			}
			else
			{
				syslog(LOG_INFO,"%s (pid=%i): The transition from the domain '%s' to the domain '%s' is refused. Context(%s).",
					  qPrintable(app),
					  (int)pid,
					  qPrintable(td_event->previousContext().name()),
					  qPrintable(td_event->transitionDemanded().transitTo().name()),
					  qPrintable(context)
					  );
			}
		}
		else
		{
			syslog(LOG_INFO,"%s (pid=%i): No transitions has been found from the domain '%s'. Context(%s).",
				  qPrintable(app),
				  (int)pid,
				  qPrintable(td_event->previousContext().name()),
				  qPrintable(context)
				  );
		}
	}
	else if(ContextdPluginRestartEvent().type()==event->type())
	{
		syslog(LOG_INFO,"Got a restart event.");
	}
}
