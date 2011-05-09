#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/limits.h>
#include <linux/sched.h>


#include "include/share.h"
#include "include/struct.h"


int prog_is_monitored()
{
	int res = 1, i = 0;
	static char monitored_prog[MONITORED_PROG_SIZE][TASK_COMM_LEN]={"firefox","testprog","soffice.bin"};

	task_lock(current);
	for(i=0; i<MONITORED_PROG_SIZE && res; ++i){
		if(strncmp(monitored_prog[i], current->comm, TASK_COMM_LEN) == 0)
			res = 0;
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
