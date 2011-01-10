/**
 * Defines three new system calls for audit security LSM :
 * ausec_auth() : called by the authentification process
 * ausec_wait(struct* ausec_info) : to get the next item to check
 * ausec_answer(int is_operation_allowed) : to send answer
 **/

#include <linux/pid.h>

#include "hooks.h"


asmlinkage long sys_ausec_auth()
{
	//prendre le semaphore de l'authentification
	if(daemon_pid == 0){
		daemon_pid = current->tgid;
		//lacher le semaphore de l'auth
		return 0;
	}
	//lacher le semaphore de l'auth.
	return -1;
}


asmlinkage long sys_ausec_wait(struct* ausec_info)
{
	//regarder s'il y a qqchose a lire et attendre.
	//prendre le semaphore de la reponse
	//prendre un semaphore lorsque l'on lit
	//copier les donnees dans le pointeur passe en arg

	/*if(copy_to_user(ausec_info, kernel_ausec_info, len)){
		return -EFAULT;
	}*/

	return 0;
}


asmlinkage long sys_ausec_answer(int is_operation_allowed)
{
	//placer la reponse
	//lacher le semaphore
	return current->tgid;
}

