#ifndef KTHREAD_H
#define KTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QString>

#include "kernel-context.h"
#include "auditsec_info.h"
#include "syscall.h"


class KernelContext;

class KThread : public QThread
{
Q_OBJECT
public:
	KThread(KernelContext *);
	void run();

	bool _keep_going;

private:
	KernelContext * KC;
	QString xmlContext(char *, ...);
	QString toXMLEntities(QString str);
};

#endif // KTHREAD_H
