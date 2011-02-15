#include <unistd.h>
#include <sys/syscall.h>

#include "syscall.h"
#include "ausec_info.h"


long ausec_reg(int state)
{
	return syscall(__NR_sys_ausec_rec, state);
}


long ausec_question(struct ausec_info * user_as_i)
{
	return syscall(__NR_sys_ausec_question, user_as_i);
}


long ausec_answer(int answer)
{
	return syscall(__NR_sys_ausec_answer, answer);
}
