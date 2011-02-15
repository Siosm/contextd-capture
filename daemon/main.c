#include <stdio.h>
#include <malloc.h>
#include <signal.h>
#include <string.h>

#include "ausec_info.h"
#include "syscall.h"

// TODO : GÃrer les signaux

int keep_going = 1;

void signal_manager(int signal)
{
	printf(" detected...\n");
	keep_going = 0;
}

int main(int argc, char* argv[])
{
	struct ausec_info * usai = malloc(sizeof(struct ausec_info));
	int i = 0;
	struct sigaction action;

	memset(&action, 0, sizeof(struct sigaction));
  	action.sa_handler = signal_manager;
  	sigaction(SIGINT, &action, NULL);
	
	if(ausec_auth(true) != 0){
		printf("FAILED to authenticate with the kernel.\n");
		return -1;
	}
	printf("The daemon is authenticated with the kernel.\n");

	while(keep_going){
		ausec_wait(usai);

		if(usai->type == AUSEC_FILE){
			printf("Ausec, file access: %s, pid: %d, execname: %s, mask: %d\n",
					usai->file.fullpath_filename, usai->pid,
					usai->execname, usai->mask);
		}

		ausec_answer(true);
	}

	printf("Stopping daemon and telling the kenel.\n");
	if(ausec_auth(false) != 0){
		printf("The kernel is NOT ok. You should reboot.\n");
	}
	printf("The kernel is ok.\n");

	return 0;
}

