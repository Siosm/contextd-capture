#ifndef ABSTRACTCONTEXT_H
#define ABSTRACTCONTEXT_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QReadWriteLock>

#include "contextclient.h"

class AbstractContext : public QObject
{
Q_OBJECT
protected:
	//There is some concurrency here. Use the lock!
	static QMap<pid_t, ContextClient> clients;
	QReadWriteLock lock;

	QString getFullPathFromPID(pid_t pid);

public slots:
	virtual QString register_application(const QString &app_name, uint app_pid) = 0;

	virtual QString domain_changed(const QString &xml_context) = 0;
	virtual QString required_domain(const QString &xml_context) = 0;

	virtual QString current_domain() = 0;
	virtual QString is_registered() = 0;

	virtual QString register_for_domain_changes_updates() = 0;

protected slots:
	virtual void onGlobalContextChanged(Domain previousGlobalContext, Domain globalContext) = 0;

	void onEvent(ContextdPluginEvent* event);

signals:
	void globalContextChanged(const QString &previous_context, const QString &new_context);
};

#endif // ABSTRACTCONTEXT_H
