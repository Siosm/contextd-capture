#ifndef RULELIST_H
#define RULELIST_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QReadWriteLock>

class RuleList : public QObject
{
Q_OBJECT

private:
	static QReadWriteLock rwlock;

	//IPTable rule --> List of programs using it
	QMap<QString, QList<QString> > rules;

	unsigned int _maxDomainsPerApplication;
	QString gwInterface;

	QList<QString> dnsRequest(QString host);

	QString createIPTableAcceptRule(QString IPDestination, unsigned int port);
	QString createIPTableRejectRule(QString IPDestination, unsigned int port);

	bool IPTableRuleExists(QString rule);

	bool exec(QString cmd);

	bool reject(QString rule);

public:
    explicit RuleList(QObject *parent = 0);

	unsigned int maxDomainsPerApplication();
	QString GWInterface();

	void setMaxDomainsPerApplication(unsigned int value);
	void setGWInterface(QString gwInterface);

	bool addRule(QString appName, QString IPDestination, unsigned int port);
	bool addDomain(QString appName, QString domainName, unsigned int port);

	bool removeRule(QString appName, QString IPDestination, unsigned int port);
	bool removeDomain(QString appName, QString domainName, unsigned int port);
	bool removeAppRules(QString appName);

	void removeAll();

signals:

public slots:

};

#endif // RULELIST_H
