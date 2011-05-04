#include "kernel-context.h"
#include "../context-common.h"
#include "eventdispatcher.h"
#include "plugins/contextdpluginrestartevent.h"
#include "plugins/domainholder/domainholder.h"

#include <QCoreApplication>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "auditsec_lsm/auditsec_info.h"
#include "contextd_adaptator.h"


KernelContext::KernelContext()
{
	qDebug("Trying to register with the kernel");
	if(auditsec_register(1) != 1){
		qFatal("FAILED to register with the kernel.");
	}
	qDebug("The daemon is registered with the kernel.");

	kernelT = new KThread(this);
}


void KernelContext::start()
{
	kernelT->start();
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


void KernelContext::stop()
{
	kernelT->_keep_going = false;

	kernelT->wait();

	qDebug("Stopping daemon and telling the kenel.");
	if(auditsec_register(0) != 0){
		qCritical("The kernel state may NOT be ok. You should reboot.");
	}else{
		qDebug("The kernel is ok.");
	}
}


struct auditsec_info * KernelContext::usai()
{
	return &_usai;
}


QString KernelContext::register_application(const QString &app_name, uint /*app_pid*/)
{
	QWriteLocker _lock(&lock);
	pid_t pid = _usai.pid;

	EventDispatcher::instance().sendNotification("KernelContext: Trying to register " + app_name);

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

			EventDispatcher::instance().sendNotification("KernelContext: " + app_name + " has been successfuly registered.");

			return KERNEL_SUCCESS;
		}else{
			EventDispatcher::instance().sendError("KernelContext: " + full_path + " cannot be identified as "+ app_name +".");

			return KERNEL_ERROR;
		}
	}
	else
		return KERNEL_ERROR;
}


QString KernelContext::is_registered()
{
	QReadLocker _lock(&lock);
	pid_t pid = _usai.pid;

	if(clients.contains(pid))
		return KERNEL_SUCCESS;
	else
		return KERNEL_ERROR;
}


QString KernelContext::domain_changed(const QString &xml_context)
{
	QReadLocker _lock(&lock);
	pid_t pid = _usai.pid;

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
	QReadLocker _lock(&lock);
	pid_t pid = _usai.pid;

	qDebug("KernelContext: required_domain");

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
	QReadLocker _lock(&lock);
	pid_t pid = _usai.pid;

	//You have to be registered to use this function !
	if(clients.contains(pid))
		return DomainHolder::instance().currentDomain().name();
	else
		return KERNEL_ERROR;
}


QString KernelContext::register_for_domain_changes_updates()
{
	QWriteLocker _lock(&lock);
	pid_t pid = _usai.pid;

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
