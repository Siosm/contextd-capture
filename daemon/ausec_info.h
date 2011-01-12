#ifndef __AUSEC_INFO_H__
#define __AUSEC_INFO_H__

/*
 * Defines structs and static values for system calls
 **/

#include <unistd.h>

#define PATH_MAX 4096
#define NAME_MAX 255
#define TASK_COMM_LEN 16

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
	char		execname[TASK_COMM_LEN];
	char		fullpath_execname[PATH_MAX];
	int			mask;
	/**
	 * Si l'on veut prendre en compte les contextes de sécurité SELinux, il
	 * suffit de les rajouter dans cette structure : char * sc;
	 **/
	enum		ausec_type type;
	union
	{
		struct ausec_file file;
		struct ausec_socket socket;
	};
};

#endif
