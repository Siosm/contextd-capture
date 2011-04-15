#include <linux/pid.h>
#include <linux/semaphore.h>


#include "share.h"
#include "hooks.h"


char ** monitored_prog(void)
{
	static char monitored_prog[MONITORED_PROG_SIZE][TASK_COMM_LEN]={"firefox","testprog","libreoffice"};
	return monitored_prog;
}


int prog_is_monitored (struct task_struct * current)
{
	int res=1, i=0;
	task_lock(current);

	for(i=0; i<MONITORED_PROG_SIZE && res; ++i) {
		res &= strncmp(execname, current->com, TASK_COMM_LEN);
	}

	task_unlock(current);
	
	return res == 0;
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


struct semaphore * auditsec_daemon_lock()
{
	static DECLARE_MUTEX(auditsec_daemon_lock);
	return &auditsec_daemon_lock;
}
