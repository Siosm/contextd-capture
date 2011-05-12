#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/limits.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "include/share.h"
#include "include/struct.h"


static LIST_HEAD(prog_list);


// struct list_head *prog_list()
// {
// 	return &prog_list;
// }


char * prog_liste()
{
	char * result = NULL;
	char * tmp = NULL;
	int alloc = 20 * (TASK_COMM_LEN + 1);
	int res_len = 0;
	struct prog * p;
	
	result = kmalloc(sizeof(char) * alloc, GFP_NOFS);
	if(result == NULL){
		printk(KERN_INFO "AuditSec: Can't allocate mem for /proc/contextd/programs");
		return NULL;
	}

	list_for_each_entry(p, &prog_list, list){
		if((res_len + TASK_COMM_LEN + 1) < alloc){
			res_len += sprintf(result + res_len, "%s\n", p->execname);
		}else{
			alloc += 20 * (TASK_COMM_LEN + 1);
			tmp = kmalloc(sizeof(char) * alloc, GFP_NOFS);
			strncpy(tmp, result, alloc);
			kfree(result);
			result = tmp;
			res_len += sprintf(result + res_len, "%s\n", p->execname);
		}
	}

	return result;
}


int prog_is_monitored()
{
	char current_task_comm[TASK_COMM_LEN];
	struct prog * p;

	get_task_comm(current_task_comm, current);
	list_for_each_entry(p, &prog_list, list){
		if(strncmp(p->execname, current_task_comm, TASK_COMM_LEN) == 0){
// 			printk(KERN_INFO "AuditSec: Prog %s is monitored", current_task_comm);
			return 1;
		}
	}
	return 0;
}


int register_prog(char * prog_name)
{
	struct prog * p;

// 	printk(KERN_INFO "AuditSec: Registering prog %s", prog_name);
	list_for_each_entry(p, &prog_list, list) {
		if(strncmp(p->execname, prog_name, TASK_COMM_LEN) == 0){
			printk(KERN_INFO "AuditSec: Prog %s already registered", prog_name);
			return -1;
		}
	}

	p = kmalloc(sizeof(*p), GFP_NOFS);
	if(p == NULL){
		return -ENOMEM;
	}
	strncpy(p->execname, prog_name, TASK_COMM_LEN);
	INIT_LIST_HEAD(&p->list);

	list_add(&p->list, &prog_list);
// 	printk(KERN_INFO "AuditSec: Prog %s registered", prog_name);
	return 0;
}


int unregister_prog(char * prog_name)
{
	struct prog * p;

	if(list_empty(&prog_list) != 0){
		printk(KERN_INFO "AuditSec: Prog list empty !");
		return -1;
	}

	list_for_each_entry(p, &prog_list, list) {
		if(strncmp(p->execname, prog_name, TASK_COMM_LEN) == 0){
			list_del(&p->list);
			kfree(p);
// 			printk(KERN_INFO "AuditSec: Unregistering prog %s", prog_name);
			return 0;
		}
	}

	return -1;
}


int clean_prog_list()
{
	struct prog * p, * next_p;

	printk(KERN_INFO "AuditSec: Cleanning prog list");

	list_for_each_entry_safe(p, next_p, &prog_list, list) {
		list_del(&p->list);
		kfree(p);
	}

	if(list_empty(&prog_list) == 0){
		printk(KERN_INFO "AuditSec: Prog list NOT empty !");
		return -1;
	}

	return 0;
}


struct auditsec_info * k_auditsec_info()
{
	static struct auditsec_info	k_auditsec_info;
	return &k_auditsec_info;
}


int * auditsec_answer()
{
	static int auditsec_answer = 1;
	return &auditsec_answer;
}


pid_t * contextd_pid()
{
	static pid_t contextd_pid = -1;
	return &contextd_pid;
}


struct semaphore * auditsec_hook_lock()
{
	static DECLARE_MUTEX(auditsec_hook_lock);
	return &auditsec_hook_lock;
}


struct semaphore * auditsec_question_lock()
{
	static DECLARE_MUTEX(auditsec_question_lock);
	return &auditsec_question_lock;
}


struct semaphore * auditsec_answer_lock()
{
	static DECLARE_MUTEX(auditsec_answer_lock);
	return &auditsec_answer_lock;
}


bool * daemon_launched()
{
	static bool daemon_launched = false;
	return &daemon_launched;
}
