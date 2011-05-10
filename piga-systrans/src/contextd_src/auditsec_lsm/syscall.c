#include "auditsec_info.h"
#include "syscall.h"
#include <stdio.h>


long int auditsec_register(int state, char * prog)
{
	return syscall(__NR_sys_auditsec_reg, state, prog);
}


long int auditsec_question(struct auditsec_info * user_as_i)
{
	return syscall(__NR_sys_auditsec_question, user_as_i);
}


long int auditsec_answer(int answer)
{
	return syscall(__NR_sys_auditsec_answer, answer);
}
