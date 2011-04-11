#include "rulelist.h"

#include <syslog.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sstream>
#include <qprocess.h>
#include <QStringList>

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"

QReadWriteLock RuleList::rwlock;

QList<QString> RuleList::dnsRequest(QString host)
{
	static QReadWriteLock rwLock;
	static QMap<QString, QList<QString> > dnsCache;

	rwLock.lockForRead();
	QList<QString> ipsCache=dnsCache[host];
	rwLock.unlock();

	if(ipsCache.size() > 0)
	{
		//qDebug("On a %i ips dans le cache pour l'host '%s'", ipsCache.size(), qPrintable(host));
		return ipsCache;
	}
	else
	{
		QList<QString> ips;

		struct hostent *s_host;
		if((s_host = gethostbyname(qPrintable(host)))!=NULL)
		{
			struct in_addr **a;
			for (a=(struct in_addr **)s_host->h_addr_list; *a; a++) {
				ips.push_back(inet_ntoa(**a));
			}

			// add the ips to the cache
			rwLock.lockForWrite();
			dnsCache[host].clear();
			dnsCache[host].append(ips);
			rwLock.unlock();
		}
		else
			EventDispatcher::instance().sendCritical(QObject::tr("Error while querying the DNS server(Host='%1')").arg(host));

		return ips;
	}
}

QString RuleList::createIPTableAcceptRule(QString IPDestination, unsigned int port)
{
	QString accept_cmd("%0 -I OUTPUT -d %1 -p tcp --dport %2 -j %3 -o %4");
	accept_cmd=accept_cmd.arg(IPTablesPath(), IPDestination).arg(port).arg("ACCEPT").arg(gwInterface);

	return accept_cmd;
}

QString RuleList::createIPTableRejectRule(QString IPDestination, unsigned int port)
{
	QString accept_cmd("%0 -D OUTPUT -d %1 -p tcp --dport %2 -j %3 -o %4");
	accept_cmd=accept_cmd.arg(IPTablesPath(), IPDestination).arg(port).arg("DROP").arg(gwInterface);

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

bool RuleList::exec(QString cmd)
{
	#warning "RuleList::exec: There is a memory leak here !"
	QProcess* process;
	if(QThread::currentThread()==this->thread())
		process=new QProcess(this);
	else
	{
		//EventDispatcher::instance().sendNotification("RuleList::exec will leak a QProcess because it cannot find the thread it has been called from.");
		process=new QProcess();
	}
	process->startDetached(cmd);
	return true;
}

bool RuleList::reject(QString rule)
{
	if(IPTableRuleExists(rule))
	{
		rule=rule.replace(" -I", " -D");
		rule=rule.replace(" -A", " -D");

		qDebug("ContextdPluginIPTableBlocker: Remove a rule : %s", qPrintable(rule));

		return exec(rule);
	}
	else
		return false;
}

//Public
RuleList::RuleList(QObject *parent) :
	QObject(parent),
	cacheThread(this),
	_maxDomainsPerApplication(-1),
	gwInterface("eth0"),
	iptablesPath("/sbin/iptables")
{
	/*iptablesPath="/usr/sbin/iptables";
	QUrl url("http://annonces.ebay.fr/");
	url.setPort(80);
	addURL("firefox", url);

	addURL("firefox", url);

	sleep(10);*/
}

unsigned int RuleList::maxDomainsPerApplication() const
{
	return _maxDomainsPerApplication;
}

QString RuleList::GWInterface() const
{
	return gwInterface;
}

QString RuleList::IPTablesPath() const
{
	return iptablesPath;
}

void RuleList::setMaxDomainsPerApplication(unsigned int value)
{
	_maxDomainsPerApplication=value;
}

void RuleList::setGWInterface(QString gwInterface)
{
	this->gwInterface=gwInterface;
}

void RuleList::setIPTablesPath(QString path)
{
	iptablesPath=path;
}

bool RuleList::addRule(QString appName, QString IPDestination, unsigned int port)
{
	QString accept_cmd=createIPTableAcceptRule(IPDestination, port);

	if(!IPTableRuleExists(accept_cmd))
	{
		rwlock.lockForWrite();
		rules[appName].push_back(accept_cmd);
		rwlock.unlock();

		qDebug("ContextdPluginIPTableBlocker: Add a rule : %s", qPrintable(accept_cmd));

		//Add the IPTable command
		exec(accept_cmd);

		return true;
	}
	else
		return false;
}

bool RuleList::addDomain(QString appName, QString domainName, unsigned int port)
{
	//If we forbid to add a domain
	if(maxDomainsPerApplication()==0)
		return false;

	//If we only one domain per app, then, delete the currently allowed domain
	if(rules[appName].size()>0 && maxDomainsPerApplication()==1)
		removeAppRules(appName);

	//Add the domain
	QList<QString> ips=dnsRequest(domainName);
	for(int i=0;i<ips.size();i++)
		addRule(appName, ips[i], port);

	return true;
}

bool RuleList::addURL(QString appName, QUrl url)
{
	// Add the domain for the current url
	addDomain(appName, url.host(), url.port());

	cacheThread.updateCache(appName, url);

	return true;
}

bool RuleList::removeRule(QString appName, QString IPDestination, unsigned int port)
{
	QString accept_cmd=createIPTableAcceptRule(IPDestination, port);
	if(rules[appName].contains(accept_cmd))
	{
		rules[appName].removeAll(accept_cmd);
		return reject(accept_cmd);
	}
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

	rwlock.lockForRead();
	QList<QString> r_rules=rules[appName];
	rwlock.unlock();

	for(int i=0;i<r_rules.size();i++)
		if(!reject(r_rules[i]))
			ret=false;

	rwlock.lockForWrite();
	rules[appName].clear();
	rwlock.unlock();

	return ret;
}

void RuleList::removeAll()
{
	QList<QString> apps=rules.keys();
	for(int i=0;i<apps.size();i++){
		removeAppRules(apps[i]);
	}
}
