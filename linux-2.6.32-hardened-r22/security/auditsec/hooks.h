/*
 * Defines structs and extern values for system calls
 **/


#ifndef __AUDITSEC_HOOKS_H__
#define __AUDITSEC_HOOKS_H__


#include <linux/pid.h>
#include <linux/limits.h>
#include <linux/sched.h>


enum ausec_type {AUSEC_FILE, AUSEC_SOCKET, AUSEC_DIR};


struct ausec_file {
	char 		filename[NAME_MAX + 1];
	char		fullpath_filename[PATH_MAX + NAME_MAX + 1]; // Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier monté d'un repertoire
	int			mask;
};


struct ausec_socket {
	//TODO
	//struct aaa	ip_dest;
	int			port;
};


struct ausec_dir {
	char 		filename[NAME_MAX + 1];
	char		fullpath_filename[PATH_MAX + NAME_MAX + 1]; // Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier monté d'un repertoire
	int			mode;
};


struct ausec_info {
	pid_t		pid;
	char		execname[TASK_COMM_LEN];
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
		struct ausec_dir dir;
	} ausec_struct;
};

#endif
