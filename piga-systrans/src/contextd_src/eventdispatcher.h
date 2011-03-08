#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include "plugins/contextdpluginevent.h"
#include "plugins/abstractcontextdplugin.h"

#include <QObject>
#include <QVector>
#include <QList>
#include <QReadWriteLock>

class EventDispatcher : public QObject
{
	Q_OBJECT

private:
	static QReadWriteLock readWriteLock;
	static QVector<AbstractContextdPlugin*> plugins;

	EventDispatcher();

	void initPlugins();

public:
	static const EventDispatcher& instance();

	bool addPlugin(AbstractContextdPlugin* plugin);

	QList<QString> getPluginNames() const;
	AbstractContextdPlugin* getPluginByName(QString plugin_name) const;

	void sendEvent(ContextdPluginEvent* event) const;

public slots:
	//Send notifications
	void sendCritical(QString critical_msg) const;
	void sendError(QString error) const;
	void sendNotification(QString msg) const;
	void sendDebug(QString msg) const;

signals:
	void event(ContextdPluginEvent* event) const;
};

const EventDispatcher* eventDispatcher();

#endif // PLUGINS_H
