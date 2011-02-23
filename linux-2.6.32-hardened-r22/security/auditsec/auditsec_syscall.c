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


asmlinkage long sys_auditsec_reg(int state)
{
	if(spin_trylock(auditsec_pid_lock()) == 0)
		return -1;

	if(state){
		if(*daemon_pid() == -1){
			*daemon_pid() = task_pid_nr(current);
			spin_unlock(auditsec_pid_lock());
			printk(KERN_INFO "AuditSec: Process %d successfully registered",
					task_pid_nr(current));
			return 0;
		}
	} else {
		if(*daemon_pid() == task_pid_nr(current)){
			*daemon_pid() = -1;
			spin_unlock(auditsec_pid_lock());
			return 0;
		}
	}
	printk(KERN_INFO "AuditSec: Process %d NOT registered ; Current is %d",
			task_pid_nr(current), *daemon_pid());
	spin_unlock(auditsec_pid_lock());

	return -1;
}


asmlinkage long sys_auditsec_question(struct auditsec_info * user_as_i)
{
	if(spin_trylock(auditsec_pid_lock()) == 0)
		return -1;

	if(*daemon_pid() == -1){
		printk(KERN_INFO "AuditSec: Process %d FAILED to question: NOT registered",
				task_pid_nr(current));
		spin_unlock(auditsec_pid_lock());
		return -1;
	}

	if(*daemon_pid() != task_pid_nr(current)){
		printk(KERN_INFO "AuditSec: Process %d FAILED to question: NOT registered ; Current is %d",
				task_pid_nr(current), *daemon_pid());
		spin_unlock(auditsec_pid_lock());
		return -1;
	}

	spin_unlock(auditsec_pid_lock());

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
	if(spin_trylock(auditsec_pid_lock()) == 0)
		return -1;

	if(*daemon_pid() == -1){
		printk(KERN_INFO "AuditSec: Process %d FAILED to answer: NOT registered",
				task_pid_nr(current));
		spin_unlock(auditsec_pid_lock());
		return -1;
	}

	if(*daemon_pid() != task_pid_nr(current)){
		printk(KERN_INFO "AuditSec: Process %d FAILED to answer ; Current is %d", task_pid_nr(current), *daemon_pid());
		spin_unlock(auditsec_pid_lock());
		return -1;
	}

	spin_unlock(auditsec_pid_lock());
	*auditsec_answer() = answer;
	up(auditsec_answer_lock());

	return 0;
}
