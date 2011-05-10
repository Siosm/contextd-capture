/*
 * Defines fucntions to access shared semaphore and data
 **/


#ifndef __AUDITSEC_SHARE_H__
#define __AUDITSEC_SHARE_H__


#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/limits.h>
#include <linux/sched.h>
#include <linux/list.h>

#include "struct.h"


struct prog {
	char execname[TASK_COMM_LEN];
	struct list_head list;
};

// #define MONITORED_PROG_SIZE 3


struct auditsec_info * k_auditsec_info(void);

char * prog_liste(void);
// struct list_head * prog_list(void);

int prog_is_monitored(void);
int register_prog(char *);
int unregister_prog(char *);
int clean_prog_list(void);

int * auditsec_answer(void);

pid_t * contextd_pid(void);

struct semaphore * auditsec_hook_lock(void);
struct semaphore * auditsec_question_lock(void);
struct semaphore * auditsec_answer_lock(void);

bool * daemon_launched(void);


#endif /* __AUDITSEC_SHARE_H__ */
