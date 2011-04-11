#ifndef CONTEXTLIST_H
#define CONTEXTLIST_H

#include "xml_common.h"

#include <QVector>
#include <QList>
#include <QMap>
#include <QString>

class DomainList
{
public:

	typedef struct
	{
		///Name of the context
		QString domain_name;

		///Name to be displayed
		QString display_name;

		///path to the icon representing this state
		QString icon;

		///Can we transit to this context ?
		bool reachable;
	}Domain_p;

	DomainList();

	//const QVector<Rule> rulesList();
	const QVector<Domain_p> domainList();
	
	const Domain_p getDomainByName(QString contextName);
	const Domain_p getDomainByName(QString currentDomain, QString domainName);
	
	Domain_p currentDomain();

private:
	QMap<QString, Domain_p> domains_p;

	bool domainReachable(QString currentContext, QString context);
};

#endif // CONTEXTLIST_H
