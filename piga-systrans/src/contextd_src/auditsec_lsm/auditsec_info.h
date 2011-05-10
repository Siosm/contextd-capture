#ifndef __AUDITSEC_INFO_H__
#define __AUDITSEC_INFO_H__

#ifdef __cplusplus
extern "C" {
	#endif

/*
 * Defines structs and static values for system calls
 **/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PATH_MAX 4096
#define NAME_MAX 255
#define TASK_COMM_LEN 16

// system calls for x64 : 299, 300, 301
// system calls for i386 : 337, 338, 339
#ifdef __x86_64
#define __NR_sys_auditsec_reg 299
#define __NR_sys_auditsec_question 300
#define __NR_sys_auditsec_answer 301
#endif /* __x86_64 */

#ifdef __i686
#define __NR_sys_auditsec_reg 337
#define __NR_sys_auditsec_question 338
#define __NR_sys_auditsec_answer 339
#endif /* __x86_64 */


enum auditsec_type {AUDITSEC_FILE, AUDITSEC_SOCKET, AUDITSEC_DIR, AUDITSEC_MSG};
enum socket_type {AUDITSEC_IPV4, AUDITSEC_IPV6};


struct auditsec_file {
	char 		name[NAME_MAX + 1];
	char		fullpath[PATH_MAX + 1];
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier mounter d'un repertoire
	int			mask;
};


struct auditsec_socket {
	enum		socket_type type;
	union 
	{
		struct sockaddr_in  addr4; // Adresse ip V4
		struct sockaddr_in6  addr6; // Adresse ip V6
	} addr;

};


struct auditsec_dir {
	char 		name[NAME_MAX + 1];
	char		fullpath[PATH_MAX + 1];
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier mounter d'un repertoire
	int			mode;
};

struct auditsec_msg {
	struct msghdr msg;
	int size;
};

struct auditsec_info {
	pid_t		pid;
	char		execname[TASK_COMM_LEN];
	//FIXME Add SELinux context to this struct
	enum		auditsec_type type;
	union
	{
		struct auditsec_file file;
		struct auditsec_socket socket;
		struct auditsec_dir dir;
		struct auditsec_dir msg;
	} auditsec_struct;
};

#ifdef __cplusplus
}
#endif

#endif
