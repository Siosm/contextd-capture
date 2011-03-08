#include "contextdpluginiptableblocker.h"
#include "../../../context-common.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"

QReadWriteLock ContextdPluginIPTableBlocker::rwlock;

bool ContextdPluginIPTableBlocker::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if(tag_name=="option")
	{
		std::string name=attributes["name"];
		std::string value=attributes["value"];
		int value_i=atoi(value.c_str());
		QString value_s=QString::fromUtf8(value.c_str());

		if(name=="flushRulesWhenChangingContext" && (value=="true" || value=="false"))
			flushRulesWhenChangingContext=(value=="true");
		else if(name=="maxDomainsPerApplication" && value_i>=-1 && value_i<=1)
			rules.setMaxDomainsPerApplication(value_i);
		else if(name=="GWInterface")
			rules.setGWInterface(value_s);
		else
			std::cerr << "ContextdPluginIPTableBlocker: Config: The argument'" << value << "' is not suitable for the option '" << name << "'" << std::endl;

		return true;
	}

	return false;
}

bool ContextdPluginIPTableBlocker::readConf()
{
	bool res=false;

	const char* path="/etc/context.d/iptables.xml";
	std::ifstream is(path);
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();

		rules.removeAll();

		rwlock.lockForWrite();
		bool result=parse(iss.str());
		rwlock.unlock();

		if(!result)
		{
			std::cerr << "Error while loading the IPTables's rules : parse(iss.str()) returned false !" << std::endl;
		}
		else
			res=true;
	}
	else
	    std::cerr << "Error while loading the IPTables's rules : Cannot read the file '" << path << "' !" << std::endl;

	printf("flushRulesWhenChangingContext=%s\n", flushRulesWhenChangingContext?"true":"false");
	printf("maxDomainsPerApplication=%i\n", rules.maxDomainsPerApplication());

	return res;
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

		QString app_name=td_event->clientConcerned()->appName();
		QString Host=td_event->clientConcerned()->currentState()[CONTEXT_HOST];
		QString port=td_event->clientConcerned()->currentState()[CONTEXT_PORT];
		QString context=td_event->transitionDemanded().transitTo().name();
		QString previousHost=td_event->clientConcerned()->previousState()[CONTEXT_HOST];
		QString previousPort=td_event->clientConcerned()->previousState()[CONTEXT_PORT];
		QString previousContext=td_event->previousContext().name();

		//If a transition happenned
		if(flushRulesWhenChangingContext && context!=previousContext)
			rules.removeAll();

		if(td_event->demandResult()==CONTEXT_ACCEPT)
		{
			if(Host!="" && port!="")
				rules.addDomain(app_name, Host, port.toInt());
		}
	}
}
