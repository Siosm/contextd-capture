#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/rwsem.h>


#include "share.h"
#include "hooks.h"


pid_t * daemon_pid()
{
	static pid_t 		daemon_pid = -1;
	return &daemon_pid;
}


pid_t * contextd_pid(void)
{
	static pid_t 		contextd_pid = -1;
	return &contextd_pid;
}


pid_t * cnotify_pid(void)
{
	static pid_t 		cnotify_pid = -1;
	return &cnotify_pid;
}


struct auditsec_info * k_auditsec_info()
{
	static struct auditsec_info	k_auditsec_info;
	return &k_auditsec_info;
}


int * auditsec_answer()
{
	static int			auditsec_answer = 1;
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


struct rw_semaphore * auditsec_pid_lock()
{
	static DECLARE_RWSEM(auditsec_pid_lock);
	return &auditsec_pid_lock;
}
