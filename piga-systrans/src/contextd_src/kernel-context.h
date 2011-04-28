#ifndef KERNELCONTEXT_H
#define KERNELCONTEXT_H

#include "contextclient.h"
#include "abstractcontext.h"

#include "auditsec_lsm/auditsec_info.h"
#include "auditsec_lsm/syscall.h"
#include "auditsec_lsm/kthread.h"

#include "malloc.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <QtCore/QMap>
#include <QtCore/QThread>

#define KERNEL_SUCCESS "Kernel: action ok"
#define KERNEL_ERROR "Kernel: error"


class KernelContext: public AbstractContext
{
Q_OBJECT
private:
	struct auditsec_info * usai;
	char exec_path[PATH_MAX];

	KThread * kernelT;

	long auditsec_register(int);
	long auditsec_question(struct auditsec_info *);
	long auditsec_answer(int);

public:
    KernelContext();
	~KernelContext();

	void start();
	struct auditsec_info * usai();

public slots:
	QString register_application(const QString &app_name, uint app_pid);

	QString domain_changed(const QString &xml_context);
	QString required_domain(const QString &xml_context);

	QString current_domain();
	QString is_registered();

	QString register_for_domain_changes_updates();

private slots:
	void onGlobalContextChanged(Domain previousGlobalContext, Domain globalContext);

// 	void onEvent(ContextdPluginEvent* event);

signals:
	void globalContextChanged(const QString &previous_context, const QString &new_context);
};

#endif // KERNELCONTEXT_H
