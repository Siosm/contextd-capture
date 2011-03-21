#include "domainlist.h"
#include <QRegExp>
#include <fstream>
#include <sstream>
#include <iostream>

#include "configuration.h"

#include "../libcontext_src/libcontext.h"

DomainList::DomainList()
{
	QList<Domain> domains=Configuration::instance().domainList();

	for(int i=0; i<domains.size(); i++)
	{
		Domain_p domain;
		domain.domain_name=domains[i].name();
		domain.display_name=domains[i].displayName();
		domain.icon=domains[i].iconPath();

		domains_p[domain.domain_name]=domain;
	}
}

const QVector<DomainList::Domain_p> DomainList::domainList()
{
	QVector<DomainList::Domain_p> v_domains=domains_p.values().toVector();
	QString currentContext=currentDomain().domain_name;

	//Update the reachable field of domain
	for(int i=0;i<v_domains.size();i++)
		v_domains[i].reachable=domainReachable(currentContext, v_domains[i].domain_name);

	return v_domains;
}

const DomainList::Domain_p DomainList::getDomainByName(QString domainName)
{
	QString currentContext=currentDomain().domain_name;

	return getDomainByName(currentContext, domainName);
}

const DomainList::Domain_p DomainList::getDomainByName(QString currentdomain, QString domainName)
{
	if(domains_p.contains(domainName))
		domains_p[domainName].reachable=domainReachable(currentdomain, domainName);

	return domains_p[domainName];
}

#include <QMessageBox>
DomainList::Domain_p DomainList::currentDomain()
{
	// Register ourselves to the daemon
	if(context_is_registered()==CONTEXT_FALSE)
	{
		if(context_register_application("context-notify")==CONTEXT_FALSE)
		{
			QMessageBox::critical(NULL, QString("Context-notify: Failed to register the application."), QString("Context-notify: Failed to register the application."));
			exit(1);
		}
	}

	QString currentDomain;
	char* cDomainBuffer=new char[51];

	if(context_current_domain(cDomainBuffer, 51)==CONTEXT_TRUE)
		currentDomain=cDomainBuffer;
	else
		currentDomain="";

	delete[] cDomainBuffer;

	domains_p[currentDomain].reachable=domainReachable(currentDomain, currentDomain);

	return domains_p[currentDomain];
}

bool DomainList::domainReachable(QString currentDomain, QString wantedDomain)
{
	Program this_prog=Configuration::instance().getProgramByName(APP_CONTEXT_NOTIFY);
	if(this_prog==Program())
		return false;

	Domain domain=Configuration::instance().getDomainByName(currentDomain);
	if(domain==Domain())
		return false;

	QMap<QString, QString> values;
	values["domain"]=wantedDomain;

	return Configuration::instance().getFirstMatchingTransition(this_prog, domain, values).isValid();
}
