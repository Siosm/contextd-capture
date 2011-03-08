#include "contextdpluginkiller.h"
#include "../../../context-common.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <signal.h>
#include <QRegExp>

#include <syslog.h>

//Events
#include "../../eventdispatcher.h"
#include "../contextdpluginnotifyevent.h"
#include "../contextdplugintransitiondemandevent.h"
#include "../contextdpluginreloadevent.h"

//Static variables
QReadWriteLock ContextdPluginKiller::rwlock;

//XML Parsing
#define KILLER_ROOT_TAG "PIGA_Killer"
#define KILLER_RULE_TAG "rule"
#define KILLER_APP_TAG "app_name"
#define KILLER_CONTEXT_TAG "context"
#define KILLER_SIGNUM_TAG "sig_num"
#define KILLER_MSG_TAG "msg_kill"
KillingRule rule=KillingRule();
QString paramName;
bool ContextdPluginKiller::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if(tag_name==KILLER_ROOT_TAG) {}
	else if(tag_name==KILLER_RULE_TAG /*&& rule==KillingRule()*/)
	{
		QString app=QString::fromUtf8(attributes[KILLER_APP_TAG].c_str());
		QString context=QString::fromUtf8(attributes[KILLER_CONTEXT_TAG].c_str());
		QString msg=QString::fromUtf8(attributes[KILLER_MSG_TAG].c_str());
		QString str_sig_num=QString::fromUtf8(attributes[KILLER_SIGNUM_TAG].c_str());

		rule=KillingRule(app, context, str_sig_num.toInt(), msg);
	}
	else if(paramName==QString() && rule!=KillingRule())
		paramName=QString::fromUtf8(tag_name.c_str());
	else
		return false;

	return true;
}

bool ContextdPluginKiller::on_cdata(const std::string& cdata)
{
	if(rule!=KillingRule() && paramName!=QString())
		rule.addParameter(paramName, QString::fromUtf8(cdata.c_str()));

	return true;
}

bool ContextdPluginKiller::on_tag_close(const std::string& tag_name)
{
	if(tag_name==paramName.toStdString())
		paramName=QString();
	else if(tag_name==KILLER_RULE_TAG)
	{
		rules.push_back(rule);
		rule=KillingRule();
	}
	else
		return false;

	return true;
}

void ContextdPluginKiller::on_error(int errnr, int line, int col, const std::string& message)
{
	printf("Killer.xml: Error %i at the line %i and column %i: %s", errnr, line, col, message.c_str());
}

bool ContextdPluginKiller::readConf()
{
	bool res=false;

	const char* path="/etc/context.d/killer.xml";
	std::ifstream is(path);
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();

		rules.clear();

		rwlock.lockForWrite();
		bool result=parse(iss.str());
		rwlock.unlock();

		if(!result)
		{
			std::cerr << "Error while loading the killer's rules : parse(iss.str()) returned false !" << std::endl;
		}
		else
			res=true;
	}
	else
	    std::cerr << "Error while loading the killer's rules : Cannot read the file '" << path << "' !" << std::endl;

	return res;
}

ContextdPluginKiller::ContextdPluginKiller() : AbstractContextdPlugin("Contextd_Killer")
{
	readConf();
}

ContextdPluginKiller::~ContextdPluginKiller()
{

}

void ContextdPluginKiller::eventHandler(const ContextdPluginEvent* event)
{
	//Transition Demand
	if(ContextdPluginTransitionDemandEvent().type()==event->type())
	{
		const ContextdPluginTransitionDemandEvent* td_event;
		td_event=static_cast<const ContextdPluginTransitionDemandEvent*>(event);

		if(td_event->demandResult()==CONTEXT_ACCEPT)
		{
			QString app=td_event->clientConcerned()->appName();
			QString context=td_event->clientConcerned()->currentGlobalState().name();

			rwlock.lockForRead();

			for(int i=0;i<rules.size();i++)
			{
				if(rules[i].isMatched(app, context, td_event->clientConcerned()->currentState()))
					killClient(td_event->clientConcerned(), rules[i].messageKill());
			}

			rwlock.unlock();
		}
	}
	else if(ContextdPluginReloadEvent().type()==event->type())
	{
		readConf();
	}
}

void ContextdPluginKiller::killClient(const ContextClient* client, QString msg)
{
	//Kill the software
	kill(client->appPid(), SIGKILL); //SIGTERM

	QString message=QObject::tr("%1 has been interrupted because it went into an illegal state : %2").arg(client->appName(), msg);
	ContextdPluginNotifyEvent event(ContextdPluginNotifyEvent::NOTIFY_CRITICAL, message);
	EventDispatcher::instance().sendEvent(&event);
}
