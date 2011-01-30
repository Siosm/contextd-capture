#include <linux/pid.h>
// #include <asm-generic/uacces.h>

#include "share.h"


static pid_t		daemon_pid = -1;
static struct ausec_info	k_ausec_info;
static int			ausec_answer;

static DECLARE_MUTEX(ausec_hook_lock);
static DECLARE_MUTEX(ausec_question_lock);
static DECLARE_MUTEX(ausec_answer_lock);
static DECLARE_MUTEX(ausec_auth_lock);


pid_t * daemon_pid()
{
	return &daemon_pid;
}


struct ausec_info * k_ausec_info()
{
	return &k_ausec_info;
}


int * ausec_answer()
{
	return &ausec_answer;
}


struct semaphore * ausec_hook_lock()
{
	return &ausec_hook_lock;
}


struct semaphore * ausec_question_lock()
{
	return &ausec_question_lock;
}


struct semaphore * ausec_answer_lock()
{
	return &ausec_answer_lock;
}


struct semaphore * ausec_auth_lock()
{
	return &ausec_auth_lock;
}