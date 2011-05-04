/**
 * Defines three new system calls for audit security LSM :
 * auditsec_reg(): to tell the kernel to start asking questions to the
 * daemon for selected task name
 * auditsec_question(struct* auditsec_info): to get the next item to check
 * auditsec_answer(int is_operation_allowed): to send answer, false == 0
 *
 * To write a correct hook, you need to:
 * down auditsec_hook_lock,
 * fill the struct with correct informations,
 * up auditsec_question_lock,
 * down auditsec_answer_lock,
 * read the answer,
 * up auditsec_hook_lock.
 *
 **/


#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/linkage.h>

#include "include/share.h"
#include "include/struct.h"

/**
 * Try to tell the kernel if the daemon is launched or not
 *
 * Returns the daemon status:
 *		1 for launched
 * 		0 for off
 * 		-EFAULT if nothing performed
 **/
asmlinkage long sys_auditsec_reg(int state)
{
	if(state == 1){
		if(*daemon_launched()){
			printk(KERN_INFO "AuditSec: According to the kernel, the daemon is already launched");
		}else{
			*daemon_launched() = true;
			printk(KERN_INFO "AuditSec: The daemon is now considered launched");
		}
		return 1;

	}else if(state == 0){
		if(*daemon_launched()){
			*daemon_launched() = false;
			printk(KERN_INFO "AuditSec: The daemon is now considered stopped");
		}else{
			printk(KERN_INFO "AuditSec: According to the kernel, the daemon is already stopped");
		}
		return 0;
	}

	printk(KERN_INFO "AuditSec: No action performed");
	return -EFAULT;
}


asmlinkage long sys_auditsec_question(struct auditsec_info * user_as_i)
{
	if(*daemon_launched() == false){
		printk(KERN_INFO "AuditSec: The daemon is not launched");
		return -EFAULT;
	}

	if(down_interruptible(auditsec_question_lock()) != 0){
		printk(KERN_INFO "AuditSec: Can't get the question lock or interrupted");
		return -EFAULT;
	}

	// FIXME faire des tests sur le pointeur donné par le process en userspace
	if(likely(user_as_i != NULL)){
		if(likely(copy_to_user(user_as_i, k_auditsec_info(), sizeof(struct auditsec_info)) == 0)){
			printk(KERN_INFO "AuditSec: pid: %d, execname: %s COPIED",
				   user_as_i->pid, user_as_i->execname);
			return 0;
		}
	}
	// If an error occured during the copy, the transaction is rejected
	*auditsec_answer() = false;
	up(auditsec_answer_lock());
	printk(KERN_INFO "AuditSec: Process %d, error in data transfer to userspace.", task_pid_nr(current));

	return -EFAULT;
}


asmlinkage long sys_auditsec_answer(int answer)
{
	if(*daemon_launched() == false){
		printk(KERN_INFO "AuditSec: The daemon is not launched");
		return -EFAULT;
	}

	*auditsec_answer() = answer;
	printk(KERN_INFO "AuditSec: %d", *auditsec_answer());
	up(auditsec_answer_lock());

	return 0;
}
