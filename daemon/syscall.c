#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "syscall.h"
#include "auditsec_info.h"


long auditsec_register(int state, pid_t contextd, pid_t cnotify)
{
	return syscall(__NR_sys_auditsec_reg, state, contextd, cnotify);
}


long auditsec_question(struct auditsec_info * user_as_i)
{
	return syscall(__NR_sys_auditsec_question, user_as_i);
}


long auditsec_answer(int answer)
{
	return syscall(__NR_sys_auditsec_answer, answer);
}
