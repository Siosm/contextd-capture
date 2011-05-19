/*
 * Defines structs and extern values for system calls
 **/


#ifndef __AUDITSEC_STRUCT_H__
#define __AUDITSEC_STRUCT_H__


#include <linux/pid.h>
#include <linux/limits.h>
#include <linux/sched.h>
// #include <net/ipv6.h>
// #include <linux/un.h>		/* for Unix socket types */
// #include <net/af_unix.h>	/* for Unix socket types */
// #include <linux/netdevice.h>	/* for network interface checks */
// #include <linux/netlink.h>
// #include <net/ip.h>		/* for local_port_range[] */
#include <linux/net.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/in6.h>


enum auditsec_type {AUDITSEC_FILE, AUDITSEC_SOCKET, AUDITSEC_DIR, AUDITSEC_MSG};
enum socket_type {AUDITSEC_IPV4, AUDITSEC_IPV6};


struct auditsec_file {
	char 		name[NAME_MAX + 1];
	char		fullpath[PATH_MAX + 1];
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier monté d'un repertoire
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
	// Doute sur la taille dans le cas d'un fichier appartenant a un systeme de fichier monté d'un repertoire
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
		struct auditsec_msg msg;
	} auditsec_struct;
};

#endif /* __AUDITSEC_STRUCT_H__ */
