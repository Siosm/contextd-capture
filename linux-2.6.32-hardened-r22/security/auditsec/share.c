#include <linux/pid.h>
// #include <asm-generic/uacces.h>

#include "share.h"
#include "hooks.h"


pid_t * daemon_pid()
{
	static pid_t		daemon_pid = -1;
	return &daemon_pid;
}


struct ausec_info * k_ausec_info()
{
	static struct ausec_info	k_ausec_info;
	return &k_ausec_info;
}


int * ausec_answer()
{
	static int			ausec_answer = 1;
	return &ausec_answer;
}


struct semaphore * ausec_hook_lock()
{
	static DECLARE_MUTEX(ausec_hook_lock);
	return &ausec_hook_lock;
}


struct semaphore * ausec_question_lock()
{
	static DECLARE_MUTEX(ausec_question_lock);
	return &ausec_question_lock;
}


struct semaphore * ausec_answer_lock()
{
	static DECLARE_MUTEX(ausec_answer_lock);
	return &ausec_answer_lock;
}


struct semaphore * ausec_auth_lock()
{
	static DECLARE_MUTEX(ausec_auth_lock);
	return &ausec_auth_lock;
}
