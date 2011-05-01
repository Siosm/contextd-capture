#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "auditsec_info.h"


long auditsec_register(int);
long auditsec_question(struct auditsec_info *);
long auditsec_answer(int);

#endif
