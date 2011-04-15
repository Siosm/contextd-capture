#ifndef __AUDITSEC_SHARE_H__
#define __AUDITSEC_SHARE_H__


#include <linux/pid.h>
#include <linux/semaphore.h>


#define MONITORED_PROG_SIZE 3


struct auditsec_info * k_auditsec_info(void);

char ** monitored_prog(void);
int prog_is_monitored (struct task_struct *);
int * auditsec_answer(void);

struct semaphore * auditsec_hook_lock(void);
struct semaphore * auditsec_question_lock(void);
struct semaphore * auditsec_answer_lock(void);
struct semaphore * auditsec_daemon_lock(void);


#endif /* share.h */
