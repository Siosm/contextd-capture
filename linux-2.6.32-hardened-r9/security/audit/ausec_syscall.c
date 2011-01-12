/**
 * Defines three new system calls for audit security LSM :
 * ausec_auth() : called by the authentification process
 * ausec_wait(struct* ausec_info) : to get the next item to check
 * ausec_answer(int is_operation_allowed) : to send answer, false == 0
 *
 * TODO: Translate this in english
 * Pour ecrire un hook, il faut :
 * prendre ausec_hook_lock,
 * remplir la structure,
 * liberer ausec_question_lock,
 * prendre ausec_answer_lock,
 * lire la reponse,
 * liberer ausec_hook_lock.
 *
 * Les hooks ne doivent pas prendre le lock_auth, car on suppose que le daemon
 * est lance avant tous les autres programmes.
 **/

#include <linux/pid.h>
#include <asm-generic/uaccess.h>

#include "hooks.h"


asmlinkage long sys_ausec_auth(void)
{
	spin_lock(&ausec_auth_lock);
	if(daemon_pid == -1){
		daemon_pid = task_pid_nr(current);
		if(daemon_pid <= 0){
			return -1;
		}
		spin_unlock(&ausec_auth_lock);
		return 0;
	}
	spin_unlock(&ausec_auth_lock);

	return -1;
}


asmlinkage long sys_ausec_wait(struct ausec_info * user_as_i)
{
	spin_lock(&ausec_auth_lock);
	if(daemon_pid != task_pid_nr(current)){
		spin_unlock(&ausec_auth_lock);
		return -1;
	}
	spin_lock(&ausec_question_lock);
	// TODO : faire les tests sur le pointeur donne par le processus ?
	if(likely(user_as_i != NULL)){
		if(likely(copy_to_user(user_as_i, &k_ausec_info, ausec_info_len) == 0)){
			spin_unlock(&ausec_auth_lock);
			return 0;
		}
	}
	// si erreur, on refuse l'operation
	ausec_answer = false;
	spin_unlock(&ausec_answer_lock);
	spin_unlock(&ausec_auth_lock);
	return -EFAULT;
}


asmlinkage long sys_ausec_answer(int answer)
{
	spin_lock(&ausec_auth_lock);
	if(daemon_pid != task_pid_nr(current)){
		spin_unlock(&ausec_auth_lock);
		return -1;
	}
	ausec_answer = answer;
	spin_unlock(&ausec_answer_lock);
	spin_unlock(&ausec_auth_lock);

	return 0;
}
