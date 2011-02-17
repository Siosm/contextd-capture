#ifndef __AUDITSEC_SHARE_H__
#define __AUDITSEC_SHARE_H__

#include <linux/pid.h>
#include <linux/semaphore.h>
// #include <asm-generic/uaccess.h>


// pid_t		daemon_pid = -1;
// struct auditsec_info	k_auditsec_info;
// int			auditsec_answer;
// int			auditsec_info_len = sizeof(struct auditsec_info);

pid_t * daemon_pid(void);
struct auditsec_info * k_auditsec_info(void);
int * auditsec_answer(void);

struct semaphore * auditsec_hook_lock(void);
struct semaphore * auditsec_question_lock(void);
struct semaphore * auditsec_answer_lock(void);
struct semaphore * auditsec_auth_lock(void);

#endif /* share.h */
