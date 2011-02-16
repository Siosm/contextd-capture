#ifndef __AUDITSEC_SHARE_H__
#define __AUDITSEC_SHARE_H__

#include <linux/pid.h>
#include <linux/semaphore.h>
// #include <asm-generic/uaccess.h>


// pid_t		daemon_pid = -1;
// struct ausec_info	k_ausec_info;
// int			ausec_answer;
// int			ausec_info_len = sizeof(struct ausec_info);

pid_t * daemon_pid(void);
struct ausec_info * k_ausec_info(void);
int * ausec_answer(void);

struct semaphore * ausec_hook_lock(void);
struct semaphore * ausec_question_lock(void);
struct semaphore * ausec_answer_lock(void);
struct semaphore * ausec_auth_lock(void);

#endif /* share.h */
