#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <unistd.h>
#include <sys/syscall.h>

#include "ausec_info.h"

#define true 1
#define false 0

// appels system sur x64 : 299, 300, 301
// appels system sur i386 : 337, 338, 339
#ifdef __x86_64
#define __NR_sys_ausec_reg 242
#define __NR_sys_ausec_question 243
#define __NR_sys_ausec_answer 244
#endif

#ifdef __i686
#define __NR_sys_ausec_reg 242
#define __NR_sys_ausec_question 243
#define __NR_sys_ausec_answer 244
#endif

long ausec_register(int state);
long ausec_wait(struct ausec_info * user_as_i);
long ausec_answer(int answer);

#endif
