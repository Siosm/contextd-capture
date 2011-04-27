#include "kernel-context.h"
#include "../context-common.h"
#include "eventdispatcher.h"
#include "plugins/contextdpluginrestartevent.h"
#include "plugins/domainholder/domainholder.h"

#include <QCoreApplication>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "contextd_adaptator.h"

KernelContext::KernelContext() /*: dbusInterface(new QDBusConnectionInterface(QDBusConnection::systemBus(), this))*/
{
}

QString KernelContext::getFullPathFromPID(pid_t pid)
{
	QString proc_path="/proc/%1/exe";
	proc_path=proc_path.arg(pid);

	return QFile::readLink(proc_path);
}

QString KernelContext::register_application(const QString &app_name, uint /*app_pid*/)
{
}

QString KernelContext::is_registered()
{
}

QString KernelContext::domain_changed(const QString &xml_context)
{
}

QString KernelContext::required_domain(const QString &xml_context)
{
}

QString KernelContext::current_domain()
{
}

QString KernelContext::register_for_domain_changes_updates()
{
}

void KernelContext::onGlobalContextChanged(Domain previousGlobalContext, Domain globalContext)
{
}

void KernelContext::onEvent(ContextdPluginEvent* event)
{
}
