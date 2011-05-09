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
 * Try to tell the kernel if the daemon is launched or not, or
 * register the process_name program to be watched by contextd
 *
 * Returns the daemon status:
 *		1 for launched
 * 		0 for off
 * 		-EFAULT if nothing performed or if an error occurred
 **/
asmlinkage long sys_auditsec_reg(int state, char * process_name)
{
	// Contextd isn't registered yet
	if(*contextd_pid() == -1){
		if(state == 1){
			if(process_name == NULL){
				*contextd_pid() = task_pid_nr(current);
				*daemon_launched() = true;
				printk(KERN_INFO "AuditSec: The daemon is now considered launched");
				return 1;
			}else{
				printk(KERN_INFO "AuditSec: The daemon is not launched: can't register a program !");
				return 0;
			}
		}else if(state == 0){
			printk(KERN_INFO "AuditSec: According to the kernel, the daemon is already stopped");
			return 0;
		}else{
			printk(KERN_INFO "AuditSec: Invalid state provided !");
			return 0;
		}

	// Contextd is already registered
	}else if(*contextd_pid() == task_pid_nr(current)){
		if(state == 1){
			if(process_name == NULL){
				printk(KERN_INFO "AuditSec: According to the kernel, the daemon is already launched");
				return 1;
			}else{
				printk(KERN_INFO "AuditSec: Registering program: %s", process_name);
				if(register_prog(process_name)){
					printk(KERN_INFO "AuditSec: Program registered: %s", process_name);
					return 1;
				}else{
					printk(KERN_INFO "AuditSec: Program NOT registered: %s", process_name);
					return -1;
				}
			}
		}else if(state == 0){
			if(process_name == NULL){
				*contextd_pid() = -1;
				*daemon_launched() = false;
				printk(KERN_INFO "AuditSec: The daemon is now considered stopped. Program list emptied");
				return 0;
			}else{
				printk(KERN_INFO "AuditSec: Unregistering program: %s", process_name);
				if(unregister_prog(process_name)){
					printk(KERN_INFO "AuditSec: Program unregistered: %s", process_name);
					return 1;
				}else{
					printk(KERN_INFO "AuditSec: Error: Program: %s", process_name);
					return -1;
				}
			}
		}

	// Contextd is registered but access is denied to other process
	}else{
		printk(KERN_INFO "AuditSec: You're not contextd");
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
// 			printk(KERN_INFO "AuditSec: pid: %d, execname: %s, file: %s (%d) COPIED",
//				   user_as_i->pid, user_as_i->execname, user_as_i->auditsec_struct.file.fullpath, strlen(user_as_i->auditsec_struct.file.fullpath));
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
// 	printk(KERN_INFO "AuditSec: answer = %d ; affectation = %d", answer, *auditsec_answer());
	up(auditsec_answer_lock());

	return 0;
}
