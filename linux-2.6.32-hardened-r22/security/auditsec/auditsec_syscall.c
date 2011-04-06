/**
 * Defines three new system calls for audit security LSM :
 * auditsec_auth() : called by the authentification process
 * auditsec_wait(struct* auditsec_info) : to get the next item to check
 * auditsec_answer(int is_operation_allowed) : to send answer, false == 0
 *
 * TODO: Translate this in english
 * Pour ecrire un hook, il faut :
 * prendre auditsec_hook_lock,
 * remplir la structure,
 * liberer auditsec_question_lock,
 * prendre auditsec_answer_lock,
 * lire la reponse,
 * liberer auditsec_hook_lock.
 *
 **/


#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/linkage.h>

#include "share.h"
#include "hooks.h"


/**
 * Try to register the current process as the main daemon 
 *
 * Returns the pid that is stored in the kernel
 **/
asmlinkage long sys_auditsec_reg(int state, int contextd, int cnotify)
{
	down_write(auditsec_pid_lock());

	if((state == 1) && (*daemon_pid() == -1)){
		*daemon_pid() = task_pid_nr(current);
		*contextd_pid() = contextd;
		*cnotify_pid() = cnotify;
		printk(KERN_INFO "AuditSec: Process %d successfully registered",
				task_pid_nr(current));
	} else if((state == 0) && (*daemon_pid() == task_pid_nr(current))){
		*daemon_pid() = -1;
		*contextd_pid() = -1;
		*cnotify_pid() = -1;
		printk(KERN_INFO "AuditSec: Process %d successfully unregistered",
				task_pid_nr(current));
	} else {
		printk(KERN_INFO "AuditSec: Process %d NOT registered ; Current is
				%d", task_pid_nr(current), *daemon_pid());
	}
	up_write(auditsec_pid_lock());

	return *daemon_pid();
}


asmlinkage long sys_auditsec_question(struct auditsec_info * user_as_i)
{
	if(down_read_trylock(auditsec_pid_lock()) == 0)
		return -1;

	if(*daemon_pid() == -1){
		printk(KERN_INFO "AuditSec: Process %d FAILED to question: NOT registered",
				task_pid_nr(current));
		up_read(auditsec_pid_lock());
		return -1;
	}

	if(*daemon_pid() != task_pid_nr(current)){
		printk(KERN_INFO "AuditSec: Process %d FAILED to question: NOT registered ; Current is %d",
				task_pid_nr(current), *daemon_pid());
		up_read(auditsec_pid_lock());
		return -1;
	}

	up_read(auditsec_pid_lock());

	if(down_interruptible(auditsec_question_lock()) != 0)
		return -1;

	// TODO : faire les tests sur le pointeur donne par le processus ?
	if(likely(user_as_i != NULL)){
		if(likely(copy_to_user(user_as_i, k_auditsec_info(), sizeof(struct auditsec_info)) == 0)){
			return 0;
		}
	}
	// si erreur, on refuse l'operation
	*auditsec_answer() = false;
	up(auditsec_answer_lock());
	printk(KERN_INFO "AuditSec: Process %d, error in data transfer to userspace ; Current is %d",
			task_pid_nr(current), *daemon_pid());
	return -EFAULT;
}


asmlinkage long sys_auditsec_answer(int answer)
{
	if(down_read_trylock(auditsec_pid_lock()) == 0)
		return -1;

	if(*daemon_pid() == -1){
		printk(KERN_INFO "AuditSec: Process %d FAILED to answer: NOT registered",
				task_pid_nr(current));
		up_read(auditsec_pid_lock());
		return -1;
	}

	if(*daemon_pid() != task_pid_nr(current)){
		printk(KERN_INFO "AuditSec: Process %d FAILED to answer ; Current is %d", task_pid_nr(current), *daemon_pid());
		up_read(auditsec_pid_lock());
		return -1;
	}

	up_read(auditsec_pid_lock());
	*auditsec_answer() = answer;
	up(auditsec_answer_lock());

	return 0;
}
