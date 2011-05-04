#include "auditsec_info.h"
#include "syscall.h"
#include <stdio.h>


long auditsec_register(int state)
{
	printf("auditsec_register");
	return syscall(__NR_sys_auditsec_reg, state);
}


long auditsec_question(struct auditsec_info * user_as_i)
{
	printf("auditsec_question");
	return 0;
	//return syscall(__NR_sys_auditsec_question, user_as_i);
}


long auditsec_answer(int answer)
{
	return syscall(__NR_sys_auditsec_answer, answer);
}
