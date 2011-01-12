/*
 * Defines structs and static values for system calls
 **/

#ifndef __AUDIT_SECURITY_HOOKS_H__
#define __AUDIT_SECURITY_HOOKS_H__

#include <linux/pid.h>
#include <linux/limits.h>
#include <linux/spinlock.h>

static int full_path_len = NAME_MAX + PATH_MAX;
enum ausec_type {AUSEC_FILE, AUSEC_SOCKET};

struct ausec_file {
	char 		filename[NAME_MAX + 1];
	char		fullpath_filename[PATH_MAX + NAME_MAX + 1]; // Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier mounter d'un repertoire
};

struct ausec_socket {
	//struct aaa	ip_dest;
	int			port;
};

struct ausec_info {
	pid_t		pid;
	char		execname[NAME_MAX];
	char		fullpath_execname[PATH_MAX];
	/**
	 * Si l'on veut prendre en compte les contextes de sécurité SELinux, il
	 * suffit de les rajouter dans cette structure : char * sc;
	 **/
	enum		ausec_type type;
	union
	{
		struct ausec_file file;
		struct ausec_socket socket;
	} ausec_struct;
};


static pid_t		daemon_pid = -1;
static struct ausec_info	k_ausec_info;
static const int	ausec_info_len = sizeof(struct ausec_info);
static int			ausec_answer;

static spinlock_t	ausec_hook_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t	ausec_question_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t	ausec_answer_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t	ausec_auth_lock = SPIN_LOCK_UNLOCKED;

#endif
