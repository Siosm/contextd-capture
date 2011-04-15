#ifndef __AUDITSEC_INFO_H__
#define __AUDITSEC_INFO_H__


/*
 * Defines structs and static values for system calls
 **/


#include <unistd.h>

#define PATH_MAX 4096
#define NAME_MAX 255
#define TASK_COMM_LEN 16


enum auditsec_type {AUDITSEC_FILE, AUDITSEC_SOCKET, AUDITSEC_DIR};


struct auditsec_file {
	char 		name[NAME_MAX + 1];
	char		fullpath[PATH_MAX + 1];
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier mounter d'un repertoire
	int			mask;
};


struct auditsec_socket {
	//TODO
	//struct aaa	ip_dest;
	int			port;
};


struct auditsec_dir {
	char 		name[NAME_MAX + 1];
	char		fullpath[PATH_MAX + 1];
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier mounter d'un repertoire
	int			mode;
};


struct auditsec_info {
	pid_t		pid;
// 	char		execname[TASK_COMM_LEN];
	//char		fullpath_execname[PATH_MAX + 1];
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
