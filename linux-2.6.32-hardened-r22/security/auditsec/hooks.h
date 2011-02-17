/*
 * Defines structs and extern values for system calls
 **/


#ifndef __AUDITSEC_HOOKS_H__
#define __AUDITSEC_HOOKS_H__


#include <linux/pid.h>
#include <linux/limits.h>
#include <linux/sched.h>


enum auditsec_type {AUDITSEC_FILE, AUDITSEC_SOCKET, AUDITSEC_DIR};


struct auditsec_file {
	char 		filename[NAME_MAX + 1];
	char		fullpath_filename[PATH_MAX + NAME_MAX + 1];
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier monté d'un repertoire
	int			mask;
};


struct auditsec_socket {
	//TODO
	//struct aaa	ip_dest;
	int			port;
};


struct auditsec_dir {
	char 		filename[NAME_MAX + 1];
	char		fullpath_filename[PATH_MAX + NAME_MAX + 1];
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier monté d'un repertoire
	int			mode;
};


struct auditsec_info {
	pid_t		pid;
	char		execname[TASK_COMM_LEN];
	char		fullpath_execname[PATH_MAX];
	/**
	 * Si l'on veut prendre en compte les contextes de sécurité SELinux, il
	 * suffit de les rajouter dans cette structure : char * sc;
	 **/
	enum		auditsec_type type;
	union
	{
		struct auditsec_file file;
		struct auditsec_socket socket;
		struct auditsec_dir dir;
	} auditsec_struct;
};

#endif
