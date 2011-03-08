#include "rulelist.h"

#include <syslog.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sstream>

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"

QReadWriteLock RuleList::rwlock;

QList<QString> RuleList::dnsRequest(QString host)
{
	QList<QString> ips;

	struct hostent *s_host;
	if((s_host = gethostbyname(qPrintable(host)))!=NULL)
	{
		struct in_addr **a;
		for (a=(struct in_addr **)s_host->h_addr_list; *a; a++) {
			ips.push_back(inet_ntoa(**a));
		}
	}
	else
		EventDispatcher::instance().sendCritical(QObject::tr("Error while querying the DNS server(Host='%1')").arg(host));

	return ips;
}

QString RuleList::createIPTableAcceptRule(QString IPDestination, unsigned int port)
{
	QString accept_cmd("iptables -I OUTPUT -d %1 -p tcp --dport %2 -j %3 -o %4");
	accept_cmd=accept_cmd.arg(IPDestination).arg(port).arg("ACCEPT").arg(gwInterface);

	return accept_cmd;
}

QString RuleList::createIPTableRejectRule(QString IPDestination, unsigned int port)
{
	QString accept_cmd("iptables -D OUTPUT -d %1 -p tcp --dport %2 -j %3 -o %4");
	accept_cmd=accept_cmd.arg(IPDestination).arg(port).arg("DROP").arg(gwInterface);

	return accept_cmd;
}

bool RuleList::IPTableRuleExists(QString rule)
{
	bool ret=false;

	rwlock.lockForRead();

	QList< QList<QString> > lists=rules.values();
	for(int i=0; i<lists.size(); i++)
	{
		if(lists[i].contains(rule))
			ret=true;
	}

	rwlock.unlock();

	return ret;
}

#include <QStringList>
bool RuleList::exec(QString cmd)
{
	int pid=fork();
	if(pid==0)
	{
		const QStringList cmd_l=cmd.split(' ', QString::SkipEmptyParts);

		QString appName=cmd_l[0];
		char* app=new char[appName.size()+1];
		strcpy(app, qPrintable(appName));

		//Create the args buffer
		char** params=new char*[cmd_l.size()];
		for(int i=0;i<cmd_l.size();i++)
		{
			params[i]=new char[cmd_l.at(i).size()];
			strcpy(params[i],qPrintable(cmd_l.at(i)));
		}

		//Execute the command
		execv(app, params);

		return false;
	}
	else if(pid<0)
		return false;

	return true;
}

bool RuleList::reject(QString rule)
{
	if(IPTableRuleExists(rule))
	{
		rule=rule.replace(" -I", " -D");

		printf("ContextdPluginIPTableBlocker: Remove a rule : %s\n", qPrintable(rule));

		return exec(rule);
	}
	else
		return false;
}

//Public
RuleList::RuleList(QObject *parent) :
	QObject(parent),
	_maxDomainsPerApplication(-1),
	gwInterface("eth0")
{
}

unsigned int RuleList::maxDomainsPerApplication()
{
	return _maxDomainsPerApplication;
}

QString RuleList::GWInterface()
{
	return gwInterface;
}

void RuleList::setMaxDomainsPerApplication(unsigned int value)
{
	_maxDomainsPerApplication=value;
}

void RuleList::setGWInterface(QString gwInterface)
{
	this->gwInterface=gwInterface;
}

bool RuleList::addRule(QString appName, QString IPDestination, unsigned int port)
{
	QString accept_cmd=createIPTableAcceptRule(IPDestination, port);

	if(!IPTableRuleExists(accept_cmd))
	{
		rwlock.lockForWrite();
		rules[appName].push_back(accept_cmd);
		rwlock.unlock();

		printf("ContextdPluginIPTableBlocker: Add a rule : %s\n", qPrintable(accept_cmd));

		//Add the IPTable command
		exec(accept_cmd);

		return true;
	}
	else
		return false;
}

bool RuleList::addDomain(QString appName, QString domainName, unsigned int port)
{
	bool ret=true;

	//If we forbid to add a domain
	if(maxDomainsPerApplication()==0)
		return true;

	//If we only one domain per app, then, delete the currently allowed domain
	if(rules[appName].size()>0 && maxDomainsPerApplication()==1)
		removeAppRules(appName);

	//Add the domain
	QList<QString> ips=dnsRequest(domainName);
	for(int i=0;i<ips.size();i++)
		if(!addRule(appName, ips[i], port))
			ret=false;

	return ret;
}

bool RuleList::removeRule(QString appName, QString IPDestination, unsigned int port)
{
	QString accept_cmd=createIPTableAcceptRule(IPDestination, port);
	if(rules[appName].contains(accept_cmd))
		return reject(accept_cmd);
	else
		return false;
}

bool RuleList::removeDomain(QString appName, QString domainName, unsigned int port)
{
	bool ret=true;

	QList<QString> ips=dnsRequest(domainName);
	for(int i=0;i<ips.size();i++)
		if(!removeRule(appName, ips[i], port))
			ret=false;

	return ret;
}

bool RuleList::removeAppRules(QString appName)
{
	bool ret=true;

	QList<QString> r_rules=rules[appName];

	rwlock.lockForRead();

	for(int i=0;i<r_rules.size();i++)
		if(!reject(r_rules[i]))
			ret=false;

	rwlock.unlock();

	return ret;
}

void RuleList::removeAll()
{
	QList<QString> apps=rules.keys();
	for(int i=0;i<apps.size();i++)
		removeAppRules(apps[i]);
}
