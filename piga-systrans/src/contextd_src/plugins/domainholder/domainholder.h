#ifndef CONTEXTHOLDER_H
#define CONTEXTHOLDER_H

#include <QtCore/QObject>
#include "../abstractcontextdplugin.h"
#include "domain.h"

#include <QtCore/QReadWriteLock>

class DomainHolder : public QObject, public AbstractContextdPlugin
{
Q_OBJECT
private:
	//Context
	static Domain c_domain;
	static Domain default_domain;
	
	//To ensure thread-safe and reentrant code
	static QReadWriteLock readwritelock;

public:
	static DomainHolder& instance();

	explicit DomainHolder(QObject *parent = 0);
	
	void eventHandler(const ContextdPluginEvent* event);
	
	static Domain currentDomain();
	static Domain setCurrentDomain(Domain domain);
	
	static Domain setDefaultDomain(Domain default_domain);
	static Domain defaultDomain();
	
	static Domain resetToDefaultDomain();

signals:
	void domainChanged(Domain previousDomain, Domain domain);

public slots:

};

#endif // CONTEXTHOLDER_H
