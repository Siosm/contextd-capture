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
	pid_t		pid;
	char *		full_path;
	char *		execname;
	char *		sc;
};

struct ausec_socket {
	pid_t		pid;
	char * 		ip_dest;
	int			port;
};

struct ausec_info {
	enum		ausec_type type;
	union
	{
		struct ausec_file file;
		struct ausec_socket socket;
	} ausec_struct;
};


static pid_t		daemon_pid = 0;
static struct ausec_info	kernel_ausec_info;
static const int	ausec_info_len = sizeof(struct ausec_info);
static int			ausec_answer;

static spinlock_t	ausec_answer_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t	ausec_hook_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t	ausec_io_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t	ausec_auth_lock = SPIN_LOCK_UNLOCKED;

#endif

