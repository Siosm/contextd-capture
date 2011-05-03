#include "auditsec_info.h"
#include "syscall.h"


long int auditsec_register(int state)
{
	return syscall(__NR_sys_auditsec_reg, state);
}


long int auditsec_question(struct auditsec_info * user_as_i)
{
	return syscall(__NR_sys_auditsec_question, user_as_i);
}


long int auditsec_answer(int answer)
{
	return syscall(__NR_sys_auditsec_answer, answer);
}
