#ifndef DBUSCONTEXT_H
#define DBUSCONTEXT_H

#include "contextclient.h"
#include <QtDBus/QDBusContext>
#include <QtCore/QMap>
#include <QtDBus/QDBusConnectionInterface>

class DBusContext: public QObject, public QDBusContext
{
Q_OBJECT
private:
	//There may be some concurrency here, we should have a look to DBusContext to know if functions are called simulteanously (I doubt it).
	QMap<QString, ContextClient> clients;

	QString getFullPathFromPID(pid_t pid);

public:
	DBusContext();

public slots:
	QString register_application(const QString &app_name, uint app_pid);

	QString domain_changed(const QString &xml_context);
	QString required_domain(const QString &xml_context);

	QString current_domain();
	QString is_registered();

	QString register_for_domain_changes_updates();

private slots:
	void onGlobalContextChanged(Domain previousGlobalContext, Domain globalContext);

	void onEvent(ContextdPluginEvent* event);

signals:
	void globalContextChanged(const QString &previous_context, const QString &new_context);
};

#endif
