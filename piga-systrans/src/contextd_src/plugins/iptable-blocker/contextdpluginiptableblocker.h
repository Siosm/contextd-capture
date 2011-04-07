#ifndef CONTEXTDPLUGINIPTABLEBLOCKER_H
#define CONTEXTDPLUGINIPTABLEBLOCKER_H

#include "../abstractcontextdplugin.h"
#include "rulelist.h"
#include <QReadWriteLock>

class ContextdPluginIPTableBlocker : public AbstractContextdPlugin
{
private:
	static QReadWriteLock rwlock;
	RuleList rules;
	bool flushRulesWhenChangingContext;
	QMap<QString, QList<QPair<QString, unsigned short> > > static_allow;

	bool readConf();

	bool addStaticAllows(const QString& domain);

public:
    ContextdPluginIPTableBlocker();
    ~ContextdPluginIPTableBlocker();

    void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINKILLER_H
