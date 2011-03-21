#include "eventdispatcher.h"
#include "misc.h"

//Static
QVector<AbstractContextdPlugin*> EventDispatcher::plugins;
QReadWriteLock EventDispatcher::readWriteLock;

//Plugins
#include "plugins/domainholder/domainholder.h"
#include "plugins/logger/contextdpluginlogger.h"
#include "plugins/killer/contextdpluginkiller.h"
#include "plugins/iptable-blocker/contextdpluginiptableblocker.h"
#include "plugins/selinux/contextdpluginselinux.h"
#include "plugins/xorg/contextdpluginxorg.h"

//Events
#include "plugins/contextdpluginnotifyevent.h"

#include <stdio.h>
const EventDispatcher& EventDispatcher::instance()
{
	static EventDispatcher e;

	/*
	Avoid the case where plugins would make a call to EventDispatcher::instance()
	while the instanciation of the static variable e.
	Indeed, EventDispatcher's constructor used to call initPlugins() by itself.
	This led EventDispatcher being a dependency of EventDispatcher --> bad !!
	*/
	e.initPlugins();

	return e;
}

EventDispatcher::EventDispatcher()
{

}

void EventDispatcher::initPlugins()
{
	if(plugins.size()>0)
		return;

	//Plugins
	if(!addPlugin(&DomainHolder::instance()))
		sendError("EventDispatcher::EventDispatcher() : Failed to add the plugin ContextHolder! Already present ?");
	else
		sendNotification("EventDispatcher::EventDispatcher() : Plugin ContextHolder added !");

	#ifndef PLUGIN_DISABLE_LOGGER
		static ContextdPluginLogger logger;

		if(!addPlugin(&logger))
			sendError("EventDispatcher::EventDispatcher() : Failed to add the plugin logger ! Already present ?");
		else
			sendNotification("EventDispatcher::EventDispatcher() : Plugin logger added !");
	#endif

	#ifndef PLUGIN_DISABLE_KILLER
		static ContextdPluginKiller killer;

		if(!addPlugin(&killer))
			sendError("EventDispatcher::EventDispatcher() : Failed to add the plugin killer! Already present ?");
		else
			sendNotification("EventDispatcher::EventDispatcher() : Plugin killer added !");
	#endif

	#ifndef PLUGIN_DISABLE_IPTABLE_BLOCKER
		static ContextdPluginIPTableBlocker IPTblocker;

		if(!addPlugin(&IPTblocker))
			sendError("EventDispatcher::EventDispatcher() : Failed to add the plugin IPTable-Blocker! Already present ?");
		else
			sendNotification("EventDispatcher::EventDispatcher() : Plugin IPTable-Blocker added !");
	#endif

	#ifndef PLUGIN_DISABLE_SELINUX
		static ContextdPluginSELinux SELinux;

		if(!addPlugin(&SELinux))
			sendError("EventDispatcher::EventDispatcher() : Failed to add the plugin SELinux! Already present ?");
		else
			sendNotification("EventDispatcher::EventDispatcher() : Plugin SELinux added !");
	#endif

	#ifndef PLUGIN_DISABLE_XORG
		static ContextdPluginXorg Xorg;

		if(!addPlugin(&Xorg))
			sendError("EventDispatcher::EventDispatcher() : Failed to add the plugin Xorg! Already present ?");
		else
			sendNotification("EventDispatcher::EventDispatcher() : Plugin Xorg added !");
	#endif
}

bool EventDispatcher::addPlugin(AbstractContextdPlugin* plugin)
{
	AbstractContextdPlugin* ret=getPluginByName(plugin->name());

	//If no plugins have the same name, add it to the plugins list
	if(ret==NULL)
	{
		readWriteLock.lockForWrite();
		plugins.push_back(plugin);
		readWriteLock.unlock();
	}

	return ret==NULL;
}

QList<QString> EventDispatcher::getPluginNames() const
{
	QList<QString> plist;

	readWriteLock.lockForRead();

	for(int i=0; i<plugins.size(); i++)
		plist.push_back(plugins[i]->name());

	readWriteLock.unlock();

	return plist;
}

AbstractContextdPlugin* EventDispatcher::getPluginByName(QString plugin_name) const
{
	AbstractContextdPlugin* ret=NULL;

	readWriteLock.lockForRead();

	for(int i=0; i<plugins.size() && ret; i++)
	{
		if(plugins[i]->name()==plugin_name)
			ret=plugins[i];
	}

	readWriteLock.unlock();

	return ret;
}

void EventDispatcher::sendEvent(ContextdPluginEvent* event) const
{
	readWriteLock.lockForRead();

	for(int i=0;i<plugins.size();i++)
		plugins[i]->eventHandler(event);

	emit this->event(event);

	readWriteLock.unlock();
}

void EventDispatcher::sendCritical(QString critical_msg) const
{
	ContextdPluginNotifyEvent test_msg(ContextdPluginNotifyEvent::NOTIFY_CRITICAL, critical_msg);
	sendEvent(&test_msg);
}

void EventDispatcher::sendError(QString error) const
{
	ContextdPluginNotifyEvent test_msg(ContextdPluginNotifyEvent::NOTIFY_ERROR, error);
	sendEvent(&test_msg);
}

void EventDispatcher::sendNotification(QString msg) const
{
	ContextdPluginNotifyEvent test_msg(ContextdPluginNotifyEvent::NOTIFY_INFO, msg);
	sendEvent(&test_msg);
}

void EventDispatcher::sendDebug(QString msg) const
{
	ContextdPluginNotifyEvent test_msg(ContextdPluginNotifyEvent::NOTIFY_DEBUG, msg);
	sendEvent(&test_msg);
}
