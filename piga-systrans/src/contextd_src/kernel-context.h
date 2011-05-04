#ifndef KERNELCONTEXT_H
#define KERNELCONTEXT_H

#include "contextclient.h"
#include "abstractcontext.h"

#include "auditsec_lsm/auditsec_info.h"
#include "auditsec_lsm/syscall.h"
#include "auditsec_lsm/kthread.h"

#include <QtCore/QMap>
#include <QtCore/QThread>

#define KERNEL_SUCCESS "KernelContext: success"
#define KERNEL_ERROR "KernelContext: error"


class KThread;


class KernelContext: public AbstractContext
{
Q_OBJECT
private:
	struct auditsec_info _usai;
	char exec_path[PATH_MAX];

	KThread * kernelT;

public:
	KernelContext();
    ~KernelContext();

	void start();
	void stop();
	struct auditsec_info * usai();

public slots:
	QString register_application(char * app_name);

	QString domain_changed(const QString &xml_context);
	QString required_domain(const QString &xml_context);

	QString current_domain();
	QString is_registered();

	QString register_for_domain_changes_updates();

private slots:
	void onGlobalContextChanged(Domain previousGlobalContext, Domain globalContext);

signals:
	void globalContextChanged(const QString &previous_context, const QString &new_context);
};

#endif // KERNELCONTEXT_H
