#ifndef __AUDITSEC_SHARE_H__
#define __AUDITSEC_SHARE_H__


#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/rwsem.h>


#define MONITORED_PROG_SIZE 3


pid_t * daemon_pid(void);
pid_t * contextd_pid(void);
pid_t * cnotify_pid(void);
struct auditsec_info * k_auditsec_info(void);
int * auditsec_answer(void);

char ** monitored_prog(void);

struct semaphore * auditsec_hook_lock(void);
struct semaphore * auditsec_question_lock(void);
struct semaphore * auditsec_answer_lock(void);
struct rw_semaphore * auditsec_pid_lock(void);


#endif /* share.h */
