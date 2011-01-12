#ifndef __SHARE_H__
#define __SHARE_H__

#include <linux/pid.h>
#include <asm-generic/uaccess.h>


pid_t		daemon_pid = -1;
struct ausec_info	k_ausec_info;
int			ausec_answer;
int			ausec_info_len = sizeof(struct ausec_info);

DECLARE_MUTEX(ausec_hook_lock);
DECLARE_MUTEX(ausec_question_lock);
DECLARE_MUTEX(ausec_answer_lock);
DECLARE_MUTEX(ausec_auth_lock);

#endif /* share.h */
