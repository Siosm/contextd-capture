#include "domainholder.h"
#include "../contextdpluginrestartevent.h"
#include "../../eventdispatcher.h"

//Static variables
Domain DomainHolder::c_domain;
Domain DomainHolder::default_domain;
QReadWriteLock DomainHolder::readwritelock;

//Methods
DomainHolder& DomainHolder::instance()
{
	static DomainHolder ch;

	return ch;
}

DomainHolder::DomainHolder(QObject *parent) : QObject(parent), AbstractContextdPlugin("domainHolder")
{
}

void DomainHolder::eventHandler(const ContextdPluginEvent* event)
{
	if(event->type()==ContextdPluginRestartEvent().type())
	{
		//const domaindPluginRestartEvent* restart=static_cast<const domaindPluginRestartEvent*>(event);

		//reset the current global state
		setCurrentDomain(default_domain);

		EventDispatcher::instance().sendNotification("The global domain has been reset");
	}
}

Domain DomainHolder::currentDomain()
{
	//Avoid other processes to change c_global_state while I return it
	QReadLocker lock(&readwritelock);

	return c_domain;
}

Domain DomainHolder::setCurrentDomain(Domain domain)
{
	QWriteLocker lock(&readwritelock);

	//Copy the current global state to return it after
	Domain ret=c_domain;

	//Update the domain
	c_domain=domain;

	//Send a signal to everyone
	emit instance().domainChanged(ret, c_domain);

	return ret;
}

Domain DomainHolder::setDefaultDomain(Domain default_domain)
{
	QWriteLocker lock(&readwritelock);

	if(DomainHolder::default_domain!=Domain())
	{
		EventDispatcher::instance().sendError("Domain holder cannot set the default domain as it has already been set");
		return default_domain;
	}

	//Copy the current default global state to return it after
	Domain ret=default_domain;

	//Update the default domain
	DomainHolder::default_domain=default_domain;

	return ret;
}

Domain DomainHolder::defaultDomain()
{
	QReadLocker lock(&readwritelock);

	return default_domain;
}

Domain DomainHolder::resetToDefaultDomain()
{
	return setCurrentDomain(defaultDomain());
}
