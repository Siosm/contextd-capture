/*
 * Defines fucntions to access shared semaphore and data
 **/


#ifndef __AUDITSEC_SHARE_H__
#define __AUDITSEC_SHARE_H__


#include <linux/pid.h>
#include <linux/semaphore.h>
#include <linux/limits.h>
#include <linux/sched.h>

#include "struct.h"


#define MONITORED_PROG_SIZE 3


struct auditsec_info * k_auditsec_info(void);

int prog_is_monitored(void);

int * auditsec_answer(void);

struct semaphore * auditsec_hook_lock(void);
struct semaphore * auditsec_question_lock(void);
struct semaphore * auditsec_answer_lock(void);

bool * daemon_launched(void);


#endif /* __AUDITSEC_SHARE_H__ */
