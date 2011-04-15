#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "auditsec_info.h"


// system calls for x64 : 299, 300, 301
// system calls for i386 : 337, 338, 339
#ifdef __x86_64
#define __NR_sys_auditsec_reg 299
#define __NR_sys_auditsec_question 300
#define __NR_sys_auditsec_answer 301
#endif /* __x86_64 */

#ifdef __i686
#define __NR_sys_auditsec_reg 337
#define __NR_sys_auditsec_question 338
#define __NR_sys_auditsec_answer 339
#endif /* __x86_64 */

long auditsec_register(int, pid_t, pid_t);
long auditsec_question(struct auditsec_info *);
long auditsec_answer(int);

#endif
