#ifndef DBUSCONTEXT_H
#define DBUSCONTEXT_H

#include "contextclient.h"
#include <QDBusContext>
#include <QMap>
#include <QDBusConnectionInterface>

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
    QString context_changed(const QString &xml_context);
    QString required_context(const QString &xml_context);
    QString current_global_context();
    QString current_local_context();
    QString is_registered();
    QString register_application(const QString &app_name, uint app_pid);
    QString register_for_context_changes_updates();

private slots:
	void onGlobalContextChanged(Context previousGlobalContext, Context globalContext);
	void onLocalContextChanged(Context previousLocalContext, Context localContext);

	void onEvent(ContextdPluginEvent* event);

signals:
    void globalContextChanged(const QString &previous_context, const QString &new_context);
};

#endif
