#include <unistd.h>
#include <sys/syscall.h>

#include "syscall.h"
#include "ausec_info.h"


long ausec_auth(int state)
{
	return syscall(__NR_ausec_auth, state);
}


long ausec_wait(struct ausec_info * user_as_i)
{
	return syscall(__NR_ausec_wait, user_as_i);
}


long ausec_answer(int answer)
{
	return syscall(__NR_ausec_answer, answer);
}
