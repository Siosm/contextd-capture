#include <stdio.h>
#include <malloc.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

#include <libcontext.h>

#include "auditsec_info.h"
#include "syscall.h"

#define APPS_SIZE 10;

int keep_going = 1;
// char apps[APPS_SIZE][TASK_COMM_LEN] = {"test", ""};


// int is_monitored(char *execname)
// {
// 	int bool = false;
// 	int i = 0;
// 	
// 	while ((i < APPS_SIZE) && (bool == false)){
// 		if(strncmp(execname, apps[i], TASK_COMM_LEN)){
// 			bool = true;
// 		}
// 		++i;
// 	}
// 	return bool;
// }

int read_execpath (pid_t pid, path)
{
	int fd = 0, read = 0;
	int size = 10 + ceil(log((double)pid));
	char proc_file[size];

	snprintf(proc_file, size, "/proc/%s/exe", pid);
	
	if ((fd = open(proc_file, O_RDONLY|O_EXLOCK)) < 0) {
		return 1;
	}
	
	if((read = read(fd, path, PATH_MAX)) <= 0) {
		close(fd);
		return 1;
	}

	close(fd);	

	return read;
}

void signal_manager(int signal)
{
	printf("^C detected...\n");
	keep_going = 0;
}

int main(int argc, char* argv[])
{
	struct auditsec_info * usai = (struct auditsec_info *) malloc(sizeof(struct auditsec_info));
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
	if((i == 5) || (keep_going == 0))
		return -1;

	printf("The daemon is registered with the kernel.\n");

	while(keep_going){
		if(auditsec_question(usai) == 0){
			
			switch (usai->type){
				case AUDITSEC_FILE:
// 					if(is_monitored(usai->execname) == true){
						#ifdef DEBUG
						printf("AuditSec, file access: %s%s, pid: %d, execname: %s, mask: %d\n",
								usai->auditsec_struct.file.fullpath,
								usai->auditsec_struct.file.name, usai->pid,
								usai->execname, usai->auditsec_struct.file.mask);
						#endif /* DEBUG */
						auditsec_answer(true);
// 					}else{
// 						auditsec_answer(true);
// 					}
					break;
				case AUDITSEC_DIR:
// 					if(is_monitored(usai->execname) == true){
						#ifdef DEBUG
						printf("AuditSec, mkdir: %s, pid: %d, execname: %s, mode: %d\n",
								usai->auditsec_struct.dir.fullpath, usai->pid,
								usai->execname, usai->auditsec_struct.dir.mode);
						#endif /* DEBUG */
						auditsec_answer(true);
// 					}else{
// 						auditsec_answer(true);
// 					}
					break;
				default:
					printf("AuditSec, can't determine struct type !");
					auditsec_answer(true);
					break;
			}
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
