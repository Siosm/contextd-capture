#include "contextnotify.h"
#include "eventdispatcher.h"
#include "context-common.h"

#include <QtDBus/QtDBus>

bool ContextNotify::promptUserForAutorisation(const QString& current_state, const QString& new_state, const QString& app_name)
{
	QDBusInterface promptUser(CONTEXTLISTENER_NAME, LISTENER_PATH, "", QDBusConnection::systemBus());

	if (!QDBusConnection::systemBus().isConnected())
	{
		qWarning("Contextd : Cannot connect to the D-Bus systemBus bus.\n"
			 "To start it, run:\n"
			 "\teval `dbus-launch --auto-syntax`\n");

		qWarning( "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));

		return false;
	}

	if (promptUser.isValid())
	{
		QDBusReply<QString> res = promptUser.call("accept_changing_domain", current_state, new_state, app_name);
		if (!res.isValid())
		{
			qWarning("PromptUser : The call to accept_changing_domain failed: %s\n", qPrintable(res.error().message()));
			return false;
		}
		else
		{
			return res.value()==CONTEXT_ACCEPT;
		}
	}
	else
	{
		qWarning("Context-Notify is unreachable. Please launch it.");
		return false;
	}

	return true;
}

bool ContextNotify::notifyUserOfDomainChanged(const QString& app_name, const QString& new_domain)
{
	QDBusInterface notifyUser(CONTEXTLISTENER_NAME, LISTENER_PATH, "", QDBusConnection::systemBus());

	if (!QDBusConnection::systemBus().isConnected())
	{
		qWarning("Contextd : Cannot connect to the D-Bus systemBus bus.\n"
			 "To start it, run:\n"
			 "\teval `dbus-launch --auto-syntax`\n");

		qWarning( "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));

		return false;
	}

	if (notifyUser.isValid())
	{
		QDBusReply<QString> res = notifyUser.call("domain_changed", app_name, new_domain);
		if (!res.isValid())
		{
			qWarning("PromptUser : The call to domain_changed failed: %s\n", qPrintable(res.error().message()));
			return false;
		}
		else
		{
			return res.value()==CONTEXT_ACCEPT;
		}
	}
	else
	{
		qWarning("Context-Notify is unreachable. Please launch it.");
		return false;
	}

	return true;
}

bool ContextNotify::notify(const QString& msg, int severity)
{
	QDBusInterface notifyUser(CONTEXTLISTENER_NAME, LISTENER_PATH, "", QDBusConnection::systemBus());

	if (!QDBusConnection::systemBus().isConnected())
	{
		EventDispatcher::instance().sendError("Contextd : Cannot connect to the D-Bus systemBus bus.\n"
			 "To start it, run:\n"
			 "\teval `dbus-launch --auto-syntax`\n");

		qWarning( "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));

		return false;
	}

	if (notifyUser.isValid())
	{
		QDBusReply<QString> res = notifyUser.call("notify", msg, severity);
		if (!res.isValid())
		{
			EventDispatcher::instance().sendError("PromptUser : The call to notify failed: " + res.error().message() + "\n.");
			return false;
		}
		else
		{
			return res.value()==CONTEXT_ACCEPT;
		}
	}
	else
	{
		qWarning("Context-Notify is unreachable. Please launch it.");
		return false;
	}

	return true;
}
