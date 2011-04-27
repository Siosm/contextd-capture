#ifndef KERNELCONTEXT_H
#define KERNELCONTEXT_H

#include "contextclient.h"

#include <QtCore/QMap>

class KernelContext: public QObject
{
	Q_OBJECT
private:
	//There may be some concurrency here, we should have a look to DBusContext to know if functions are called simulteanously (I doubt it).
	QMap<pid_t, ContextClient> clients;

	QString getFullPathFromPID(pid_t pid);

public:
    KernelContext();

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

#endif // KERNELCONTEXT_H
