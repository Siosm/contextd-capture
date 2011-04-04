#include <iostream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <libcontext.h>

#include "auditsec_info.h"
#include "syscall.h"


int keep_going = true;
int testprog_reg = false;


int read_execpath (pid_t pid, char * path)
{
	ssize_t nb = 0;
	std::string proc_file("/proc/");
	std::stringstream out;
	
	out << (double) pid;
	proc_file += out.str() + "/exe";
	
	
	if((nb = readlink(proc_file.c_str(), path, PATH_MAX)) <= 0) {
		return 1;
	}
	path[nb] = '\0';
	return nb;
}

void signal_manager(int signal)
{
	std::cout << "^C detected..." << std::endl;
	keep_going = 0;
}

int main(int argc, char* argv[])
{
	struct auditsec_info * usai = (struct auditsec_info *) malloc(sizeof(struct auditsec_info));
#ifdef DEBUG
	char exec_path[PATH_MAX];
#endif
	int i = 0;
	struct sigaction action;

	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << "contextd_pid" << std::endl;
		return -1;
	}

	pid_t contextd_pid = atoi(argv[1]);

	//TODO Finir de bloquer les signaux
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = signal_manager;
	sigaction(SIGINT, &action, NULL);
	while((auditsec_register(true) != getpid()) && (i < 5)){
		std::cerr << "FAILED to register with the kernel." << std::endl;
		++i;
	}
	if((i == 5) || (keep_going == 0))
		return -1;

	std::cout << "The daemon is registered with the kernel." << std::endl;

	while(keep_going){
		std::cout << "Boucle" << std::endl;
		if(auditsec_question(usai) == 0){
			
			switch (usai->type){
				case AUDITSEC_FILE:
					if(strncmp(usai->execname, "testprog", TASK_COMM_LEN) || (usai->pid != contextd_pid)){
						if(testprog_reg == false){
							std::cout << "Trying to reg as daemon" << std::endl;
							context_register_application("daemon");
						}
						switch (context_changed(//"pid", usai->pid,
					"fullpath", usai->auditsec_struct.file.fullpath,
// 					"filename", usai->auditsec_struct.file.name,
					NULL, NULL)){
							case CONTEXT_ACCEPTED:
								auditsec_answer(true);
								std::cout << "Transition acceptee." << std::endl;
								break;
							case CONTEXT_REFUSED:
								auditsec_answer(true);
								std::cout << "Transition refuse." << std::endl;
								break;
							case CONTEXT_ERROR:
								auditsec_answer(true);
								std::cout << "Erreur dans la transition." << std::endl;
								break;
							default:
								auditsec_answer(false);
								std::cout << "Default ! On ne devrait pas etre la !" << std::endl;
								break;
						}
						#ifdef DEBUG
						read_execpath(usai->pid, exec_path);
						std::cout << "AuditSec, file access: " << usai->auditsec_struct.file.fullpath
						<< usai->auditsec_struct.file.name << ", pid: " << usai->pid << ", execname: "
						<< exec_path << ", mask: " << usai->auditsec_struct.file.mask << std::endl;
						#endif /* DEBUG */
					}else{
						auditsec_answer(true);
					}
					break;
				case AUDITSEC_DIR:
					if(strncmp(usai->execname, "testprog", TASK_COMM_LEN)){
						if(testprog_reg == false){
							context_register_application("daemon");
						}
						switch (context_changed("pid", usai->pid,
					"fullpath", usai->auditsec_struct.dir.fullpath,
					NULL, NULL)){
							case CONTEXT_ACCEPTED:
								auditsec_answer(true);
								std::cout << "Transition acceptée." << std::endl;
								break;
							case CONTEXT_REFUSED:
								auditsec_answer(true);
								std::cout << "Transition refusée." << std::endl;
								break;
							case CONTEXT_ERROR:
								auditsec_answer(true);
								std::cout << "Erreur dans la transition : " << context_getLastError() << std::endl;
								break;
							default:
								auditsec_answer(true);
								std::cout << "Default ! On ne devrait pas être là !" << std::endl;
								break;
						}
						#ifdef DEBUG
						read_execpath(usai->pid, exec_path);
						std::cout << "AuditSec, mkdir: " << usai->auditsec_struct.dir.fullpath
						<< ", pid: " << usai->pid << ", execname: " << exec_path << usai->execname << ", mode: "
						<< usai->auditsec_struct.dir.mode << std::endl;
						#endif /* DEBUG */
					}else{
						auditsec_answer(true);
					}
					break;
				default:
					std::cerr << "AuditSec, can't determine struct type !" << std::endl;
					auditsec_answer(true);
					break;
			}
		}
	}

	std::cout << "Stopping daemon and telling the kenel." << std::endl;
	if(auditsec_register(false) != -1){
		std::cerr << "The kernel state may NOT be ok. You should reboot." << std::endl;
		return -1;
	}
	std::cout << "The kernel is ok." << std::endl;

	return 0;
}
