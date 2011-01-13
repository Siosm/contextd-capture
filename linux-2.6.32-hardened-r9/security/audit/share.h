#ifndef __SHARE_H__
#define __SHARE_H__

#include <linux/pid.h>
#include <asm-generic/uaccess.h>


pid_t		daemon_pid = -1;
struct ausec_info	k_ausec_info;
int			ausec_answer;
int			ausec_info_len = sizeof(struct ausec_info);

struct * semaphore ausec_hook_lock()
struct * semaphore ausec_question_lock()
struct * semaphore ausec_answer_lock()
struct * semaphore ausec_auth_lock()

#endif /* share.h */
