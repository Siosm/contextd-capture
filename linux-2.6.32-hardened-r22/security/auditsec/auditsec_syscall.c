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
 * Try to tell the kernel if the daemon is launched or not
 *
 * Returns the daemon status, 1 for launched, 0 for off
 **/
asmlinkage long sys_auditsec_reg(int state)
{
	if(state == 1){
		if(down_trylock(auditsec_pid_lock()) == 0){
			printk(KERN_INFO "AuditSec: According to the kernel, the daemon is already launched", task_pid_nr(current));
			return 1;
		}else{
			printk(KERN_INFO "AuditSec: The daemon is now considered launched", task_pid_nr(current));
			return 0;
		}
	}else if(state == 0){
		up(auditsec_pid_lock());
		printk(KERN_INFO "AuditSec: The daemon is now considered stopped", task_pid_nr(current));
		return 0;
	}else{
		return 0;//FIXME get lock state and return
	}
}


asmlinkage long sys_auditsec_question(struct auditsec_info * user_as_i)
{
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
	*auditsec_answer() = answer;
	up(auditsec_answer_lock());

	return 0;
}
