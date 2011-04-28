#ifndef KTHREAD_H
#define KTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QString>

#include "kernel-context.h"
#include "auditsec_lsm/auditsec_info.h"
#include "auditsec_lsm/syscall.h"


class KernelContext;


class KThread : public QThread
{
Q_OBJECT
private:
	KernelContext * KC;

public:
	KThread(KernelContext *);
	void run();

private:
	QString xmlContext(char *, ...);
	QString toXMLEntities(QString str);

signals:
	QString register_application(const QString &app_name, uint app_pid);

	QString domain_changed(const QString &xml_context);
	QString required_domain(const QString &xml_context);

	QString current_domain();
	QString is_registered();

	QString register_for_domain_changes_updates();
};

#endif // KTHREAD_H
