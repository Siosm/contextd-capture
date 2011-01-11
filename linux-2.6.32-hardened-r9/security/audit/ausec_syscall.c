/**
 * Defines three new system calls for audit security LSM :
 * ausec_auth() : called by the authentification process
 * ausec_wait(struct* ausec_info) : to get the next item to check
 * ausec_answer(int is_operation_allowed) : to send answer
 *
 * TODO: Translate this in english
 * Pour ecrire un hook, il faut commencer par rendre le hook_lock, on verifie
 * que le io_lock est pris, puis on ecrit dans la structure, puis on prend
 * le answer_lock, on lache le io_lock, et on prend (bloquant) le answer_lock,
 * lorsque l'on est debloque, on peut lire la reponse, lacher answer_lock,
 * puis lacher le hook_lock
 *
 * Les hooks ne doivent pas prendre le lock_auth, car on suppose que le daemon
 * est lance avant tous les autres programmes.
 **/

#include <linux/pid.h>
#include <asm-generic/uaccess.h>

#include "hooks.h"


asmlinkage long sys_ausec_auth(void)
{
	// prends le lock de l'authentification
	spin_lock(&ausec_auth_lock);
	// teste si on a deja fait l'authentification
	if(daemon_pid == -1){
		daemon_pid = task_pid_nr(current);
		if(daemon_pid <= 0){
			return -1;
		}
		// lacher le lock de l'auth
		spin_unlock(&ausec_auth_lock);
		return 0;
	}
	// lacher le lock de l'auth.
	spin_unlock(&ausec_auth_lock);

	return -1;
}


asmlinkage long sys_ausec_wait(struct ausec_info * user_as_i)
{
	// regarder s'il y a qqchose de pret a lire, sinon attendre.
	spin_lock(&ausec_io_lock);
	// copier les donnees dans le pointeur passe en arg
	// TODO : faire les tests sur le pointeur donne par le processus ?
	if(likely(user_as_i != NULL)){
		if(likely(copy_to_user(user_as_i, &k_ausec_info, ausec_info_len) == 0)){
			return 0;
		}
	}
	// si cela ne marche pas, on interdit l'operation et on relache tout
	ausec_answer = false;
	spin_unlock(&ausec_answer_lock);
	return -EFAULT;
}


asmlinkage long sys_ausec_answer(int answer)
{
	// on verifie que le lock sur la reponse est ok
	if(spin_trylock(&ausec_answer_lock)){
		return -1;
	}
	// ecrire la reponse
	ausec_answer = answer;
	// lacher le lock de la reponse
	spin_unlock(&ausec_answer_lock);

	return 0;
}
