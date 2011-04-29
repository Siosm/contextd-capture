#include "kernel-context.h"
#include "../context-common.h"
#include "eventdispatcher.h"
#include "plugins/contextdpluginrestartevent.h"
#include "plugins/domainholder/domainholder.h"

#include <QCoreApplication>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "contextd_adaptator.h"


KernelContext::KernelContext()
{
	_usai = (struct auditsec_info *) malloc(sizeof(struct auditsec_info));

	qDebug("Trying to register with the kernel");
	if(auditsec_register(1) != 1){
		qFatal("FAILED to register with the kernel.");
	}
	qDebug("The daemon is registered with the kernel.");

	kernelT = new KThread(this);
}


KernelContext::~KernelContext()
{
	qDebug("Stopping daemon and telling the kenel.");
	if(auditsec_register(0) != 0){
		qCritical("The kernel state may NOT be ok. You should reboot.");
	}else{
		qDebug("The kernel is ok.");
	}
}


void KernelContext::start()
{
	kernelT->start();
}


auditsec_info* KernelContext::usai()
{
	return _usai;
}


long int KernelContext::auditsec_register(int state)
{
	return syscall(__NR_sys_auditsec_reg, state);
}


long int KernelContext::auditsec_question(auditsec_info* user_as_i)
{
	return syscall(__NR_sys_auditsec_question, user_as_i);
}


long int KernelContext::auditsec_answer(int answer)
{
	return syscall(__NR_sys_auditsec_answer, answer);
}


QString KernelContext::register_application(const QString &app_name, uint /*app_pid*/)
{
	pid_t pid=0;

	//Get the pid
	pid = _usai->pid;

	EventDispatcher::instance().sendNotification("Trying to register program from kernel-context class.");

	QWriteLocker _lock(&lock);

	//You can't register two applications using the same pid !
	if((pid != 0) && !clients.contains(pid))
	{
		//Get the exe full path
		QString full_path=getFullPathFromPID(pid);

		const Configuration& transRules=Configuration::instance();

		//If the program is white listed
		if(transRules.isProgramAllowed(app_name, full_path))
		{
			Program program=transRules.getProgramByName(app_name);
			clients[pid]=ContextClient(0, program, pid);

			EventDispatcher::instance().sendNotification("the application "+app_name+" has been successfuly registered (kernel).");

			return KERNEL_SUCCESS;
		}
		else
		{
			EventDispatcher::instance().sendError("The program "+full_path+" cannot be identified as "+ app_name +" (kernel).");

			return KERNEL_ERROR;
		}
	}
	else
		return KERNEL_ERROR;
}

QString KernelContext::is_registered()
{
	pid_t pid=0;

	//Get the pid
	pid = _usai->pid;

	QReadLocker _lock(&lock);

	if(clients.contains(pid))
		return KERNEL_SUCCESS;
	else
		return KERNEL_ERROR;
}

QString KernelContext::domain_changed(const QString &xml_context)
{
	pid_t pid=0;

	//Get the pid
	pid = _usai->pid;

	QReadLocker _lock(&lock);

	//You have to be registered to use this function !
	if(clients.contains(pid))
	{
		if(clients[pid].updateState(xml_context) == CONTEXT_ACCEPT){
			auditsec_answer(true);
			return CONTEXT_ACCEPT;
		}else{
			auditsec_answer(false);
			return KERNEL_ERROR;
		}
	} else {
		auditsec_answer(false);
		return KERNEL_ERROR;
	}
}

QString KernelContext::required_domain(const QString &xml_context)
{
	pid_t pid=0;

	//Get the pid
	pid = _usai->pid;

	qDebug("Registering");

	//You have to be registered to use this function !
	if(clients.contains(pid))
	{
		return clients[pid].required_domain(xml_context);
	} else {
// 		auditsec_answer(false);
		return KERNEL_ERROR;
	}
}

QString KernelContext::current_domain()
{
	pid_t pid=0;
	pid = _usai->pid;

	//Get the pid

	//You have to be registered to use this function !
	if(clients.contains(pid))
		return DomainHolder::instance().currentDomain().name();
	else
		return KERNEL_ERROR;
}

QString KernelContext::register_for_domain_changes_updates()
{
	pid_t pid=0;
	pid = _usai->pid;

	//Get the pid

	//You have to be registered to use this function !
	if(clients.contains(pid))
		return clients[pid].registerForContextChangesUpdates();
	else
		return KERNEL_ERROR;
}

void KernelContext::onGlobalContextChanged(Domain previousGlobalContext, Domain globalContext)
{
	emit globalContextChanged(previousGlobalContext.name(), globalContext.name());
}


// void KernelContext::onEvent(ContextdPluginEvent* event)
// {
// }


bool KernelContext::is_registered_k(pid_t pid)
{
	QReadLocker _lock(&lock);
	return clients.contains(pid);
}
