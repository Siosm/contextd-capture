/**
 * Defines three new system calls for audit security LSM :
 * ausec_auth() : called by the authentification process
 * ausec_wait(struct* ausec_info) : to get the next item to check
 * ausec_answer(int is_operation_allowed) : to send answer, false == 0
 *
 * TODO: Translate this in english
 * Pour ecrire un hook, il faut :
 * prendre ausec_hook_lock,
 * remplir la structure,
 * liberer ausec_question_lock,
 * prendre ausec_answer_lock,
 * lire la reponse,
 * liberer ausec_hook_lock.
 *
 * Les hooks ne doivent pas prendre le lock_auth, car on suppose que le daemon
 * est lance avant tous les autres programmes.
 **/

#include <linux/pid.h>
#include <linux/semaphore.h>
#include <asm-generic/uaccess.h>
#include <linux/linkage.h>

#include "../security/audit/share.h"
#include "../security/audit/hooks.h"


asmlinkage long sys_ausec_auth(int state)
{
	down_interruptible(ausec_auth_lock());
	if(state){
		if(*daemon_pid() == -1){
			*daemon_pid() = task_pid_nr(current);
			up(ausec_auth_lock());
			printk(KERN_INFO "AuSec: Process %d successfully registered", task_pid_nr(current));
			return 0;
		}
	} else {
		if(*daemon_pid() == task_pid_nr(current)){
			*daemon_pid() = -1;
			up(ausec_auth_lock());
			printk(KERN_INFO "AuSec: Process %d NOT registered ; Current is %d", task_pid_nr(current), *daemon_pid());
			return 0;
		}
	}
	up(ausec_auth_lock());

	return -1;
}


asmlinkage long sys_ausec_wait(struct ausec_info * user_as_i)
{
	down_interruptible(ausec_auth_lock());
	if(*daemon_pid() != task_pid_nr(current)){
		up(ausec_auth_lock());
		printk(KERN_INFO "AuSec: Process %d FAILED to wait as it isn't registered ; Current is %d", task_pid_nr(current), *daemon_pid());
		return -1;
	}
	up(ausec_auth_lock());
	/*
	down_interruptible(ausec_question_lock());
	// TODO : faire les tests sur le pointeur donne par le processus ?
	if(likely(user_as_i != NULL)){
		if(likely(copy_to_user(user_as_i, k_ausec_info(), sizeof(struct ausec_info)) == 0)){
			return 0;
		}
	}
	// si erreur, on refuse l'operation
	*ausec_answer() = false;
	up(ausec_answer_lock());
	printk(KERN_INFO "AuSec: Process %d, error in data transfer to userspace ; Current is %d", task_pid_nr(current), *daemon_pid());
	return -EFAULT;
	*/
	return 0;
}


asmlinkage long sys_ausec_answer(int answer)
{

	down_interruptible(ausec_auth_lock());
	if(*daemon_pid() != task_pid_nr(current)){
		up(ausec_auth_lock());
		printk(KERN_INFO "AuSec: Process %d FAILED to answer ; Current is %d", task_pid_nr(current), *daemon_pid());
		return -1;
	}
	up(ausec_auth_lock());
	*ausec_answer() = answer;
	up(ausec_answer_lock());

	return 0;
}
