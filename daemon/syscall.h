#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <unistd.h>
#include <sys/syscall.h>

#include "ausec_info.h"

#define true 1
#define false 0

// appels system sur x64 : 299, 300, 301
// appels system sur i386 : 337, 338, 339
#ifdef __X86_64
#define __NR_ausec_auth 299
#define __NR_ausec_wait 300
#define __NR_ausec_answer 301
#endif

#ifdef __I686
#define __NR_ausec_auth 337
#define __NR_ausec_wait 338
#define __NR_ausec_answer 339
#endif

long ausec_auth(int state);
long ausec_wait(struct ausec_info * user_as_i);
long ausec_answer(int answer);

#endif