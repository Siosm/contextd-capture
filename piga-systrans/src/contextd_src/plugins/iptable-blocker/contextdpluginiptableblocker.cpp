#include "contextdpluginiptableblocker.h"
#include "../../../context-common.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include <QDomDocument>
#include <QFile>
#include <QDomNode>

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"
#include "../contextdpluginreloadevent.h"
#include "../contextdpluginrestartevent.h"

QReadWriteLock ContextdPluginIPTableBlocker::rwlock;

bool ContextdPluginIPTableBlocker::readConf()
{
	// parse the configuration
	QDomDocument doc("iptables.xml");
	QFile file("/etc/context.d/iptables.xml");
	if (!file.open(QIODevice::ReadOnly))
		return false;
	if (!doc.setContent(&file))
	{
		file.close();
		return false;
	}
	file.close();

	// Read the options
	QDomNodeList options=doc.elementsByTagName("option");
	for(int i=0; i<options.size(); i++)
	{
		QDomElement element=options.at(i).toElement();
		QString name=element.attribute("name");
		QString value=element.attribute("value");

		bool ok;
		int value_i=value.toInt(&ok);
		if(!ok)
			value_i=-123456;

		if(name=="flushRulesWhenChangingContext" && (value=="true" || value=="false"))
			flushRulesWhenChangingContext=(value=="true");
		else if(name=="maxDomainsPerApplication" && value_i>=-1 && value_i<=1)
			rules.setMaxDomainsPerApplication(value_i);
		else if(name=="GWInterface")
			rules.setGWInterface(value);
		else if(name=="IPTablesPath")
			rules.setIPTablesPath(value);
	}

	// Read the static allow
	QDomNodeList domains=doc.elementsByTagName("domain");
	for(int i=0; i<domains.size(); i++)
	{
		QDomElement element=domains.at(i).toElement();
		QString domain_name=element.attribute("name");

		// Get the IPs
		QDomNodeList ips=element.elementsByTagName("allow");
		for(int i=0; i<ips.size(); i++)
		{
			QDomElement element=ips.at(i).toElement();

			QString ip=element.attribute("ip");
			QString port_s=element.attribute("port");

			bool port_ok;
			unsigned int port=port_s.toInt(&port_ok);

			if(ip!=QString() && port_ok && port<65536)
			{
				static_allow[domain_name].push_back(qMakePair(ip, (unsigned short)port));
				//fprintf(stderr, "Add '%s:%i' for domain '%s'\n", qPrintable(ip), port, qPrintable(domain_name));
			}
		}
	}

	return true;
}

bool ContextdPluginIPTableBlocker::addStaticAllows(const QString& domain)
{
	QList<QPair<QString, unsigned short> > toAllow=static_allow[domain];

	for(int i=0; i<toAllow.size(); i++)
		rules.addRule("static_allow", toAllow[i].first, toAllow[i].second);

	return true;
}

ContextdPluginIPTableBlocker::ContextdPluginIPTableBlocker() :
		AbstractContextdPlugin("Contextd_IPTablesBlocker"),
		flushRulesWhenChangingContext(true)
{
	readConf();
}

ContextdPluginIPTableBlocker::~ContextdPluginIPTableBlocker()
{
	//On exit, flush all the added rules
	rules.removeAll();
}

void ContextdPluginIPTableBlocker::eventHandler(const ContextdPluginEvent* event)
{
	//Transition Demand
	if(ContextdPluginTransitionDemandEvent().type()==event->type())
	{
		const ContextdPluginTransitionDemandEvent* td_event;
		td_event=static_cast<const ContextdPluginTransitionDemandEvent*>(event);

		QString app_name=td_event->clientConcerned()->program().name();
		QString url_s=td_event->clientConcerned()->currentState()[CONTEXT_URL];
		QString host=td_event->clientConcerned()->currentState()[CONTEXT_HOST];
		QString port=td_event->clientConcerned()->currentState()[CONTEXT_PORT];
		QString domain=td_event->transitionDemanded().transitTo().name();
		/*QString previousHost=td_event->clientConcerned()->previousState()[CONTEXT_HOST];
		QString previousPort=td_event->clientConcerned()->previousState()[CONTEXT_PORT];
		QString previousDomain=td_event->previousContext().name();*/

		//If a transition happenned and we want to flush
		if(flushRulesWhenChangingContext) // && domain!=previousDomain
			rules.removeAll();

		//Add static allows
		addStaticAllows(domain);

		if(td_event->demandResult()==CONTEXT_ACCEPT)
		{
			if(url_s!="" && host!="" && port!="")
			{
				QUrl url(url_s);
				url.setPort(port.toInt());
				rules.addURL(app_name, url);
			}
			else if(host!="" && port!="")
				rules.addDomain(app_name, host, port.toInt());
		}
	}
	else if(ContextdPluginRestartEvent().type()==event->type())
	{
		// On reset, remove everything
		rules.removeAll();
	}
	else if(ContextdPluginReloadEvent().type()==event->type())
	{
		readConf();
	}
}
