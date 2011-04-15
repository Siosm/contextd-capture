#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "syscall.h"
#include "auditsec_info.h"


long auditsec_register(int state)
{
	return syscall(__NR_sys_auditsec_reg, state);
}


long auditsec_question(struct auditsec_info * user_as_i)
{
	return syscall(__NR_sys_auditsec_question, user_as_i);
}


long auditsec_answer(int answer)
{
	return syscall(__NR_sys_auditsec_answer, answer);
}
