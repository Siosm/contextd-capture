#include "dbus-context.h"
#include "../context-common.h"
#include "eventdispatcher.h"
#include "plugins/contextdpluginrestartevent.h"
#include "plugins/domainholder/domainholder.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusMessage>
#include <QCoreApplication>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "contextd_adaptator.h"

DBusContext::DBusContext() /*: dbusInterface(new QDBusConnectionInterface(QDBusConnection::systemBus(), this))*/
{
	if (!QDBusConnection::systemBus().isConnected())
	{
		EventDispatcher::instance().sendError("Cannot connect to the D-Bus system bus.\n"
			 "To start it, run:\n"
			 "\teval `dbus-launch --auto-syntax`\n");

		QCoreApplication::instance()->exit(-1);
	}

	if (!QDBusConnection::systemBus().registerService(SERVICE_NAME))
	{
		EventDispatcher::instance().sendError(qPrintable(QDBusConnection::systemBus().lastError().message()));
		QCoreApplication::instance()->exit(1);
	}

	ContextdAdaptor* adaptor=new ContextdAdaptor(this);

	if (!QDBusConnection::systemBus().registerObject(LISTENER_PATH, adaptor, QDBusConnection::ExportAllSlots))
	{
		EventDispatcher::instance().sendError(qPrintable(QDBusConnection::systemBus().lastError().message()));
		QCoreApplication::instance()->exit(1);
	}
}

QString DBusContext::getFullPathFromPID(pid_t pid)
{
	QString proc_path="/proc/%1/exe";
	proc_path=proc_path.arg(pid);

	return QFile::readLink(proc_path);
}

QString DBusContext::register_application(const QString &app_name, uint /*app_pid*/)
{
	QString dbus_id=message().service();

	//You can't register two applications using the same dbus_id
	if(!clients.contains(dbus_id))
	{
		pid_t pid=0;

		//Get the pid
		QDBusReply<uint> res=QDBusConnection::systemBus().interface()->servicePid(dbus_id);
		if(res.isValid())
			pid=res.value();

		//Get the exe full path
		QString full_path=getFullPathFromPID(pid);

		const Configuration& transRules=Configuration::instance();

		//If the program is white listed
		if(transRules.isProgramAllowed(app_name, full_path))
		{
			Program program=transRules.getProgramByName(app_name);
			clients[dbus_id]=ContextClient(dbus_id, program, pid);

			EventDispatcher::instance().sendNotification("the application "+app_name+" has been successfuly registered.");

			return DBUS_SUCCESS;
		}
		else
		{
			EventDispatcher::instance().sendError("The program "+full_path+" cannot be identified as "+ app_name +".");

			return DBUS_ERROR;
		}
	}
	else
		return DBUS_ERROR;
}

QString DBusContext::is_registered()
{
	QString dbus_id=message().service();
	if(clients.contains(dbus_id))
		return DBUS_SUCCESS;
	else
		return DBUS_ERROR_NOT_REGISTERED;
}

QString DBusContext::domain_changed(const QString &xml_context)
{
	QString dbus_id=message().service();

	//You have to be registered to use this function !
	if(clients.contains(dbus_id))
	{
		return clients[dbus_id].updateState(xml_context);
	}
	else
		return DBUS_ERROR_NOT_REGISTERED;
}

QString DBusContext::required_domain(const QString &xml_context)
{
	QString dbus_id=message().service();

	//You have to be registered to use this function !
	if(clients.contains(dbus_id))
	{
		return clients[dbus_id].required_domain(xml_context);
	}
	else
		return DBUS_ERROR_NOT_REGISTERED;
}

QString DBusContext::current_domain()
{
	QString dbus_id=message().service();

	//You have to be registered to use this function !
	if(clients.contains(dbus_id))
		return DomainHolder::instance().currentDomain().name();
	else
		return DBUS_ERROR_NOT_REGISTERED;
}

QString DBusContext::register_for_domain_changes_updates()
{
	QString dbus_id=message().service();

	//You have to be registered to use this function !
	if(clients.contains(dbus_id))
		return clients[dbus_id].registerForContextChangesUpdates();
	else
		return DBUS_ERROR_NOT_REGISTERED;
}

void DBusContext::onGlobalContextChanged(Domain previousGlobalContext, Domain globalContext)
{
	QDBusMessage msg = QDBusMessage::createSignal(LISTENER_PATH, SERVICE_NAME, "globalContextChanged");
	msg << previousGlobalContext.name() << globalContext.name();
	if(!QDBusConnection::systemBus().send(msg))
		EventDispatcher::instance().sendError("Sending Error: "+ QDBusConnection::systemBus().lastError().message());

	emit globalContextChanged(previousGlobalContext.name(), globalContext.name());
}

void DBusContext::onEvent(ContextdPluginEvent* event)
{
	if(event->type()==ContextdPluginRestartEvent().type())
	{
		DomainHolder::instance().resetToDefaultDomain();
	}
}
