#include <stdio.h>
#include <malloc.h>
#include <signal.h>
#include <string.h>

#include "ausec_info.h"
#include "syscall.h"

int keep_going = 1;

void signal_manager(int signal)
{
	printf("^C detected...\n");
	keep_going = 0;
}

int main(int argc, char* argv[])
{
	struct ausec_info * usai = malloc(sizeof(struct ausec_info));
	int i = 0;
	struct sigaction action;

	//TODO Finir de bloquer les signaux
	memset(&action, 0, sizeof(struct sigaction));
  	action.sa_handler = signal_manager;
  	sigaction(SIGINT, &action, NULL);
	
	while((ausec_register(true) != 0) && (i < 10)){
		printf("FAILED to authenticate with the kernel.\n");
		++i;
	}
	if((i == 10) || (keep_going == 0))
		return -1;

	printf("The daemon is authenticated with the kernel.\n");

	while(keep_going){
		ausec_question(usai);

		switch (usai->type){
			case AUSEC_FILE:
				printf("Ausec, file access: %s, pid: %d, execname: %s, mask: %d\n",
						usai->file.fullpath_filename, usai->pid,
						usai->execname, usai->file.mask);
				break;		
			case AUSEC_DIR:
				printf("Ausec, mkdir: %s, pid: %d, execname: %s, mode: %d\n",
						usai->dir.fullpath_filename, usai->pid,
						usai->execname, usai->dir.mode);
				break;
			default:
				break;
		}

		sleep(1);

		ausec_answer(true);
	}

	printf("Stopping daemon and telling the kenel.\n");
	if(ausec_register(false) != 0){
		printf("The kernel state may NOT be ok. You should reboot.\n");
		return -1;
	}
	printf("The kernel is ok.\n");

	return 0;
}

