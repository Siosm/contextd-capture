#include <linux/pid.h>
// #include <asm-generic/uacces.h>

#include "share.h"
#include "hooks.h"


pid_t ** daemon_pid()
{
	static pid_t *		daemon_pid = NULL;
	return &daemon_pid;
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


struct semaphore * auditsec_auth_lock()
{
	static DECLARE_MUTEX(auditsec_auth_lock);
	return &auditsec_auth_lock;
}
