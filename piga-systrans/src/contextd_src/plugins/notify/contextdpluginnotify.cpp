#include "contextdpluginnotify.h"
#include "../../../context-common.h"

#include <QtDBus/QtDBus>

#include "contextdpluginnotify-impl.h"

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"

ContextdPluginNotify::ContextdPluginNotify() :
		AbstractContextdPlugin("Contextd_Notify"), \
		verbosity(ContextdPluginNotifyEvent::NOTIFY_NORMAL)
{
	initNotify("Contextd");
}

ContextdPluginNotify::~ContextdPluginNotify()
{
	closeNotify();
}

void ContextdPluginNotify::setVerbosityLevel(ContextdPluginNotifyEvent::Priority level)
{
	verbosity=level;
}

ContextdPluginNotifyEvent::Priority ContextdPluginNotify::verbosityLevel()
{
	return verbosity;
}

void ContextdPluginNotify::eventHandler(const ContextdPluginEvent* event)
{
	//Transition Demand
	if(ContextdPluginTransitionDemandEvent().type()==event->type())
	{
		const ContextdPluginTransitionDemandEvent* td_event;
		td_event=static_cast<const ContextdPluginTransitionDemandEvent*>(event);

		if(td_event->transitionDemanded().isValid() && td_event->demandResult()==CONTEXT_ACCEPT)
		{
			Context globalContext=td_event->clientConcerned()->currentGlobalState();
			QString scope=td_event->transitionDemanded().isALocalTransition()?SCOPE_LOCAL:SCOPE_GLOBAL;
			QString app=td_event->clientConcerned()->appName();

			if(td_event->transitionDemanded().notifyUser())
				on_context_changed(scope, app, globalContext);

		}
	}
	else if(ContextdPluginNotifyEvent().type()==event->type())
	{
		const ContextdPluginNotifyEvent* n_event;
		n_event=static_cast<const ContextdPluginNotifyEvent*>(event);

		notifyEvent(n_event);
	}
}

//Functions
bool ContextdPluginNotify::notifyEvent(const ContextdPluginNotifyEvent* event)
{
	if(event->priority() >= verbosity)
	{
		int priority_level=0;

		QString title, icon;
		if(event->priority() == event->NOTIFY_CRITICAL)
		{
			title="Contextd Critical Message";
			icon="/usr/share/icons/gnome/32x32/actions/process-stop.png";
			priority_level=2;
		}
		else if(event->priority() == event->NOTIFY_ERROR)
		{
			title="Contextd Error";
			icon="/usr/share/icons/gnome/32x32/actions/process-stop.png";
			priority_level=2;
		}
		else if(event->priority() >= event->NOTIFY_INFO)
		{
			title="Contextd";
			icon="/usr/share/icons/gnome/32x32/status/dialog-information.png";
			priority_level=1;
		}
		else
			title="Contextd Debug";

		sendNotification(qPrintable(title), qPrintable(event->message()), 0, priority_level, qPrintable(icon));
	}

	return true;
}

bool ContextdPluginNotify::on_context_changed(const QString& scope, const QString& app_name, Context context)
{
	QString title;
	QString msg;
	if(scope==SCOPE_GLOBAL)
	{
		title=QObject::tr("System's Context Transition");
		msg=QObject::tr("Your system has switched to the context '%1' on %2's demand.")
		    .arg(context.displayName()).arg(app_name);
	}
	else
	{
		title=QObject::tr("Local Context Transition");
		msg = QObject::tr("%1 has switched to the context '%2'.")
			 .arg(app_name).arg(context.displayName());
	}

	sendNotification(qPrintable(title), qPrintable(msg), 0, 1, qPrintable(context.iconPath()));

	return true;
}
