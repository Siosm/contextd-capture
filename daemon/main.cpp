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


bool keep_going = true;


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
	std::cout << " detected..." << std::endl;
	keep_going = 0;
}

int main(int argc, char* argv[])
{
	struct auditsec_info * usai = (struct auditsec_info *) malloc(sizeof(struct auditsec_info));
	#ifdef DEBUG
	char exec_path[PATH_MAX];
	#endif
	struct sigaction action;

	if(argc > 1) {
		std::cerr << "Usage: " << argv[0] << std::endl;
		return -1;
	}

	//TODO Finir de bloquer les signaux
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = signal_manager;
	sigaction(SIGINT, &action, NULL);

	std::cout << "Trying to register with the kernel" << std::endl;
	if(auditsec_register(1) != 1){
		std::cerr << "FAILED to register with the kernel." << std::endl;
		return -1;
	}
	std::cout << "The daemon is registered with the kernel." << std::endl;

	std::cout << "Trying to register with contextd" << std::endl;
	if(context_register_application("daemon") != CONTEXT_TRUE){
		std::cerr << "FAILED to register with contextd." << std::endl;
		if(auditsec_register(0) == 1){
			std::cerr << "The kernel state may NOT be ok. You should reboot." << std::endl;
		}else{
			std::cout << "The kernel is ok." << std::endl;
		}
		return -1;
	}
	std::cout << "The daemon is registered with contextd." << std::endl;

	while(keep_going){
		switch (usai->type){
		case AUDITSEC_FILE:
			switch (context_changed(//"pid", usai->pid,
					"fullpath", usai->auditsec_struct.file.fullpath,
//					"filename", usai->auditsec_struct.file.name,
					NULL, NULL)){
			case CONTEXT_ACCEPTED:
				auditsec_answer(true);
				std::cout << "Transition acceptée." << std::endl;
				break;
			case CONTEXT_REFUSED:
				auditsec_answer(false);
				std::cerr << "Transition refusée." << std::endl;
				break;
			case CONTEXT_ERROR:
				auditsec_answer(false);
				std::cerr << "Erreur dans la transition : " << context_getLastError() << std::endl;
				break;
			default:
				auditsec_answer(false);
				std::cerr << "Default ! On ne devrait pas être là !" << std::endl;
				break;
			}
			#ifdef DEBUG
			read_execpath(usai->pid, exec_path);
			std::cout << "AuditSec, file access: " << usai->auditsec_struct.file.fullpath
			<< "/" << usai->auditsec_struct.file.name << ", pid: " << usai->pid << ", execname: "
			<< exec_path /*<< ", mask: " << usai->auditsec_struct.file.mask*/ << std::endl;
			#endif /* DEBUG */
			break;

		case AUDITSEC_DIR:
			switch (context_changed("pid", usai->pid,
					"fullpath", usai->auditsec_struct.dir.fullpath,
					NULL, NULL)){
				case CONTEXT_ACCEPTED:
					auditsec_answer(true);
					std::cout << "Transition acceptée." << std::endl;
					break;
				case CONTEXT_REFUSED:
					auditsec_answer(false);
					std::cerr << "Transition refusée." << std::endl;
					break;
				case CONTEXT_ERROR:
					auditsec_answer(false);
					std::cerr << "Erreur dans la transition : " << context_getLastError() << std::endl;
					break;
				default:
					auditsec_answer(false);
					std::cerr << "Default ! On ne devrait pas être là !" << std::endl;
					break;
			}
			#ifdef DEBUG
			read_execpath(usai->pid, exec_path);
			std::cout << "AuditSec, mkdir: " << usai->auditsec_struct.dir.fullpath
			<< ", pid: " << usai->pid << ", execname: " << exec_path << usai->execname /*<< ", mode: "
			<< usai->auditsec_struct.dir.mode*/ << std::endl;
			#endif /* DEBUG */
			break;
		default:
			std::cerr << "AuditSec, can't determine struct type !" << std::endl;
			auditsec_answer(false);
			break;
		}
	}

	std::cout << "Stopping daemon and telling the kenel." << std::endl;
	if(auditsec_register(0) != 0){
		std::cerr << "The kernel state may NOT be ok. You should reboot." << std::endl;
		return -1;
	}
	std::cout << "The kernel is ok." << std::endl;

	return 0;
}
