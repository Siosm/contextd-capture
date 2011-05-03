#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "auditsec_info.h"


long auditsec_register(int);
long auditsec_question(struct auditsec_info *);
long auditsec_answer(int);

#ifdef __cplusplus
}
#endif

#endif
