#include "promptuser.h"
#include "eventdispatcher.h"
#include "context-common.h"

#include <QtDBus/QtDBus>

bool PromptUser::promptUserForAutorisation(const QString& scope, const QString& current_state, const QString& new_state, const QString& app_name)
{
	QDBusInterface promptUser(CONTEXTLISTENER_NAME, "/", "", QDBusConnection::systemBus());

	if (!QDBusConnection::systemBus().isConnected())
	{
		EventDispatcher::instance().sendError("Contextd : Cannot connect to the D-Bus systemBus bus.\n"
			 "To start it, run:\n"
			 "\teval `dbus-launch --auto-syntax`\n");

		fprintf(stderr, "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));

		return false;
	}

	if (promptUser.isValid())
	{
		QDBusReply<QString> res = promptUser.call("accept_changing_state", scope, current_state, new_state, app_name);
		if (!res.isValid())
		{
			EventDispatcher::instance().sendError("PromptUser : The call to accept_changing_state failed: " + res.error().message() + "\n.");
			return false;
		}
		else
		{
			return res.value()==CONTEXT_ACCEPT;
		}
	}
	else
	{
		EventDispatcher::instance().sendError("Context-Notify is unreachable. Please launch it.");
		return false;
	}

	return true;
}
