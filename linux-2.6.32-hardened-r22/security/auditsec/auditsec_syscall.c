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
 * Les hooks ne doivent pas prendre le lock_auth, car on suppose que le daemon
 * est lance avant tous les autres programmes.
 **/

#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/linkage.h>
#include <linux/vmalloc.h>

#include "share.h"
#include "hooks.h"


int pid_branch (struct task_struct *tsk) 
{
	pid_t *branch = NULL;
	pid_t *tmp = NULL;
	int n = 0, i =0;

	while(tsk != NULL){
		tmp = branch;
		vfree(branch);
		branch = (pid_t *) vmalloc((n+1)*sizeof(pid_t));
		for(i=0; i<n; i++) branch[i] = tmp[i];
		vfree(tmp);
		branch[n] = task_pid_nr(tsk);
		tsk = tsk->real_parent;
		n++;
	}
	return n;
}


asmlinkage long sys_auditsec_reg(int state)
{
	if(down_interruptible(auditsec_auth_lock()) != 0)
		return -1;
		
	if(state){
		if(*daemon_pid() == NULL){
			pid_branch(current);
			up(auditsec_auth_lock());
			printk(KERN_INFO "AuditSec: Process %d successfully registered",
					task_pid_nr(current));
			return 0;
		}
	} else {
		if(**daemon_pid() == task_pid_nr(current)){
			vfree(*daemon_pid());
			*daemon_pid() = NULL;
			up(auditsec_auth_lock());
			return 0;
		}
	}
	printk(KERN_INFO "AuditSec: Process %d NOT registered ; Current is %d",
			task_pid_nr(current), **daemon_pid());
	up(auditsec_auth_lock());

	return -1;
}


asmlinkage long sys_auditsec_question(struct auditsec_info * user_as_i)
{
	if(down_interruptible(auditsec_auth_lock()) != 0)
		return -1;

	if(*daemon_pid() == NULL){
		printk(KERN_INFO "AuditSec: Process %d FAILED to question: NOT registered",
				task_pid_nr(current));
		up(auditsec_auth_lock());
		return -1;
	}

	if(**daemon_pid() != task_pid_nr(current)){
		up(auditsec_auth_lock());
		printk(KERN_INFO "AuditSec: Process %d FAILED to question: NOT registered ; Current is %d",
				task_pid_nr(current), **daemon_pid());
		up(auditsec_auth_lock());
		return -1;
	}

	up(auditsec_auth_lock());

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
			task_pid_nr(current), **daemon_pid());
	return -EFAULT;
}


asmlinkage long sys_auditsec_answer(int answer)
{

	if(down_interruptible(auditsec_auth_lock()) != 0)
		return -1;

	if(*daemon_pid() == NULL){
		printk(KERN_INFO "AuditSec: Process %d FAILED to answer: NOT registered",
				task_pid_nr(current));
		up(auditsec_auth_lock());
		return -1;
	}

	if(**daemon_pid() != task_pid_nr(current)){
		up(auditsec_auth_lock());
		printk(KERN_INFO "AuditSec: Process %d FAILED to answer ; Current is %d", task_pid_nr(current), **daemon_pid());
		return -1;
	}

	up(auditsec_auth_lock());
	*auditsec_answer() = answer;
	up(auditsec_answer_lock());

	return 0;
}
