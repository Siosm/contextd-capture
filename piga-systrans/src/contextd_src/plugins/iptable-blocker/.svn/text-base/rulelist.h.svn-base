#ifndef RULELIST_H
#define RULELIST_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QUrl>
#include <QReadWriteLock>

// Http
#include "domaincachethread.h"

class RuleList : public QObject
{
Q_OBJECT

private:
	static QReadWriteLock rwlock;

	//IPTable rule --> List of programs using it
	QMap<QString, QList<QString> > rules;

	// Cache
	DomainCacheThread cacheThread;

	unsigned int _maxDomainsPerApplication;
	QString gwInterface;
	QString iptablesPath;

	QList<QString> dnsRequest(QString host);

	QString createIPTableAcceptRule(QString IPDestination, unsigned int port);
	QString createIPTableRejectRule(QString IPDestination, unsigned int port);

	bool IPTableRuleExists(QString rule);

	bool exec(QString cmd);

	bool reject(QString rule);

	QUrl urlToCacheUrl(QUrl url);

public:
	explicit RuleList(QObject *parent = 0);

	unsigned int maxDomainsPerApplication() const;
	QString GWInterface() const;
	QString IPTablesPath() const;

	void setMaxDomainsPerApplication(unsigned int value);
	void setGWInterface(QString gwInterface);
	void setIPTablesPath(QString path);

public slots:

	bool addRule(QString appName, QString IPDestination, unsigned int port);
	bool addDomain(QString appName, QString domainName, unsigned int port);
	bool addURL(QString appName, QUrl url);

	bool removeRule(QString appName, QString IPDestination, unsigned int port);
	bool removeDomain(QString appName, QString domainName, unsigned int port);
	bool removeAppRules(QString appName);

	void removeAll();

signals:
};

#endif // RULELIST_H
