#include <stdio.h>
#include <malloc.h>
#include <signal.h>
#include <string.h>

#include "auditsec_info.h"
#include "syscall.h"

int keep_going = 1;

void signal_manager(int signal)
{
	printf("^C detected...\n");
	keep_going = 0;
}

int main(int argc, char* argv[])
{
	struct auditsec_info * usai = malloc(sizeof(struct auditsec_info));
	int i = 0;
	struct sigaction action;

	//TODO Finir de bloquer les signaux
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = signal_manager;
	sigaction(SIGINT, &action, NULL);
	while((auditsec_register(true) != 0) && (i < 5)){
		printf("FAILED to register with the kernel.\n");
		++i;
	}
	if((i == 10) || (keep_going == 0))
		return -1;

	printf("The daemon is registered with the kernel.\n");

	while(keep_going){
		if(auditsec_question(usai) == 0){
			#ifdef DEBUG
			switch (usai->type){
				case AUDITSEC_FILE:
					if(strcmp("sshd", usai->execname) != 0){
					printf("AuditSec, file access: %s%s, pid: %d, execname: %s, mask: %d\n",
							usai->auditsec_struct.file.fullpath,
							usai->auditsec_struct.file.name, usai->pid,
							usai->execname, usai->auditsec_struct.file.mask);
					} 
					break;
				case AUDITSEC_DIR:
					printf("AuditSec, mkdir: %s, pid: %d, execname: %s, mode: %d\n",
							usai->auditsec_struct.dir.fullpath, usai->pid,
							usai->execname, usai->auditsec_struct.dir.mode);
					break;
				default:
					printf("AuditSec, can't determine struct type !");
					break;
			}
			#endif /* DEBUG */

			auditsec_answer(true);
		}
	}

	printf("Stopping daemon and telling the kenel.\n");
	if(auditsec_register(false) != 0){
		printf("The kernel state may NOT be ok. You should reboot.\n");
		return -1;
	}
	printf("The kernel is ok.\n");

	return 0;
}

