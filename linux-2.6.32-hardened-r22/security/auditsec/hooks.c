#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/tracehook.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/security.h>
#include <linux/xattr.h>
#include <linux/capability.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/tty.h>
#include <net/icmp.h>
#include <net/ip.h>		/* for local_port_range[] */
#include <net/tcp.h>		/* struct or_callable used in sock_rcv_skb */
#include <net/net_namespace.h>
#include <net/netlabel.h>
#include <linux/uaccess.h>
#include <asm/ioctls.h>
#include <asm/atomic.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>	/* for network interface checks */
#include <linux/netlink.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/dccp.h>
#include <linux/quota.h>
#include <linux/un.h>		/* for Unix socket types */
#include <net/af_unix.h>	/* for Unix socket types */
#include <linux/parser.h>
#include <linux/nfs_mount.h>
#include <net/ipv6.h>
#include <linux/hugetlb.h>
#include <linux/personality.h>
#include <linux/sysctl.h>
#include <linux/audit.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/posix-timers.h>

#include <linux/sched.h>
#include <linux/limits.h>
#include <linux/semaphore.h>
#include <linux/jiffies.h>

#include "include/struct.h"
#include "include/share.h"


#include <linux/module.h>

int auditsec_info_len = sizeof(struct auditsec_info);

int calculate_path(struct dentry *dentry, char *path, size_t len)
{
	int pos = 0, size = 0;

	if (IS_ROOT(dentry)) {
		return 0;
	} else {
		pos = calculate_path(dentry->d_parent, path, len);

		if (pos != -1 ) {
			size = strlen(dentry->d_name.name);

			if (pos + size + 1 < len) {
				path[pos] = '/';
				strncpy(path + pos + 1, dentry->d_name.name, size);
				return pos + size + 1;
			}
		}
	}
	return -1;
}

int file_path(struct file *file, char *path)
{
	const char * mnt_point = file->f_path.mnt->mnt_devname;
	int n = 0;

	if (strcmp("/dev/root", mnt_point)) {
		n = strlen(mnt_point);
		strncpy(path, mnt_point, n);
	}

	n = calculate_path(file->f_path.dentry, path + n, NAME_MAX + PATH_MAX - n);
	path[n] = '\0';
	return n+1;
}

int dir_path(struct dentry *dentry, char *path)
{
	int n = 0;

	n = calculate_path(dentry, path + n, NAME_MAX + PATH_MAX - n);
	path[n] = '\0';
	return n+1;
}


int auditsec_inode_create(struct inode *dir, struct dentry *dentry, int mode)
{
	return 0;
}
EXPORT_SYMBOL_GPL(auditsec_inode_create);

int auditsec_inode_link(struct dentry *old_dentry, struct inode *dir,
			 struct dentry *new_dentry)
{
	return 0;
}

int auditsec_inode_unlink(struct inode *dir, struct dentry *dentry)
{
	return 0;
}

int auditsec_inode_symlink(struct inode *dir, struct dentry *dentry,
			    const char *old_name)
{
	return 0;
}

int auditsec_inode_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	int		answer = -1;
	int		ret = 0;
	pid_t 	current_pid = task_pid_nr(current);
	char *	fullpath = NULL;

	if(prog_is_monitored()){
		fullpath = kmalloc(PATH_MAX + 1, GFP_NOFS);
		if(fullpath == NULL){
			printk(KERN_INFO "AuditSec: mkdir: pid: %d, execname: %s, mode: %d NO MEM",
				   current_pid, current->comm, mode);
			return -EFAULT;
		}
		dir_path(dentry, fullpath);
		if(*daemon_launched()){
			if(down_timeout(auditsec_hook_lock(), 30 * HZ) != 0){// 30s timeout. Is it too much ?
				printk(KERN_INFO "AuditSec: mkdir: %s, pid: %d, execname: %s, mode: %d HOOK TIMEOUT",
						fullpath, current_pid, current->comm, mode);
				kfree(fullpath);
				return -EFAULT;
			}

			k_auditsec_info()->pid = current_pid;
			get_task_comm(k_auditsec_info()->execname, current);
			k_auditsec_info()->type = AUDITSEC_DIR;
			strncpy(k_auditsec_info()->auditsec_struct.dir.fullpath, fullpath, PATH_MAX + 1);
			strncpy(k_auditsec_info()->auditsec_struct.dir.name, dentry->d_name.name, NAME_MAX + 1);
			k_auditsec_info()->auditsec_struct.dir.mode = mode;
			// TODO Add fields to this struct (se_context)

			up(auditsec_question_lock());
			if(down_timeout(auditsec_answer_lock(), 30 * HZ) != 0){// 30s timeout. Is it too much ?
				printk(KERN_INFO "AuditSec: mkdir: %s, pid: %d, execname: %s, mode: %d ANSWER TIMEOUT",
						fullpath, current_pid, current->comm, mode);
				kfree(fullpath);

				ret = down_trylock(auditsec_question_lock());
				up(auditsec_hook_lock());
				return -EFAULT;
			}

			kfree(fullpath);
			answer = (*auditsec_answer() == 0);
			up(auditsec_hook_lock());

			return answer == 0 ? 0 : -EACCES;
		}else{
			printk(KERN_INFO "AuditSec: mkdir: %s, pid: %d, execname: %s, mode: %d REFUSED : daemon not launched",
				   fullpath, current_pid, current->comm, mode);
			kfree(fullpath);
			return -EFAULT;
		}
	}else{
		/*printk(KERN_INFO "AuditSec: mkdir: %s, pid: %d, execname: %s, mode: %d ANSWER TIMEOUT",
				fullpath, current_pid, current->comm, mode);
			kfree(fullpath);*/
	}
	return 0;
}
EXPORT_SYMBOL_GPL(auditsec_inode_mkdir);

int auditsec_inode_rmdir(struct inode *dir, struct dentry *dentry)
{
	return 0;
}

int auditsec_inode_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev)
{
	return 0;
}

int auditsec_inode_rename(struct inode *old_dir, struct dentry *old_dentry,
			   struct inode *new_dir, struct dentry *new_dentry)
{
	return 0;
}

int auditsec_inode_readlink(struct dentry *dentry)
{
	return 0;
}

int auditsec_inode_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	return 0;
}

int auditsec_inode_permission(struct inode *inode, int mask)
{
	return 0;
}

int auditsec_file_permission(struct file *file, int mask)
{
	int		answer = -1;
	int		ret = 0;
	pid_t 	current_pid = task_pid_nr(current);
	char *	fullpath = NULL;

	if(prog_is_monitored()){
		fullpath = kmalloc(PATH_MAX + 1, GFP_NOFS);
		if(fullpath == NULL){
			printk(KERN_INFO "AuditSec: file access: pid: %d, execname: %s, mask: %d NO MEM",
					current_pid, current->comm, mask);
			return -EFAULT;
		}
		file_path(file, fullpath);
		if(*daemon_launched()){
			if(down_timeout(auditsec_hook_lock(), 30 * HZ) != 0){// 30s timeout. Is it too much ?
				printk(KERN_INFO "AuditSec: file access: %s, pid: %d, execname: %s, mask: %d HOOK TIMEOUT",
					fullpath, current_pid, current->comm, mask);
				kfree(fullpath);
				return -EFAULT;
			}

			get_task_comm(k_auditsec_info()->execname, current);
			k_auditsec_info()->pid = current_pid;
			k_auditsec_info()->type = AUDITSEC_FILE;
			strncpy(k_auditsec_info()->auditsec_struct.file.fullpath, fullpath, PATH_MAX + 1);
			strncpy(k_auditsec_info()->auditsec_struct.file.name, file->f_path.dentry->d_name.name, NAME_MAX + 1);
			k_auditsec_info()->auditsec_struct.file.mask = mask;
			// TODO Add fields to this struct (se_context)

			up(auditsec_question_lock());
			if(down_timeout(auditsec_answer_lock(), 30 * HZ) != 0){// 30s timeout. Is it too much ?
				printk(KERN_INFO "AuditSec: file access: %s, pid: %d, execname: %s, mask: %d ANSWER TIMEOUT",
					fullpath, current_pid, current->comm, mask);
				kfree(fullpath);

				ret = down_trylock(auditsec_question_lock());
				up(auditsec_hook_lock());
				return -EFAULT;
			}

			kfree(fullpath);
			answer = (*auditsec_answer() == 0);
			up(auditsec_hook_lock());

			return answer == 0 ? 0 : -EACCES;
		}else{
			printk(KERN_INFO "AuditSecu: file access: %s, pid: %d, execname: %s, mask: %d REFUSED : daemon not launched",
				fullpath, task_pid_nr(current), current->comm, mask);
			kfree(fullpath);
			return -EFAULT;
		}
	}else{
		/*printk(KERN_INFO "AuditSecu: file access: %s, pid: %d, execname: %s, mask: %d",
				fullpath, task_pid_nr(current), current->comm, mask);
			kfree(fullpath);*/
	}
	return 0;
}

int auditsec_file_alloc(struct file *file)
{
	return 0;
}

void auditsec_file_free(struct file *file)
{
}

int auditsec_dentry_open(struct file *file, const struct cred *cred)
{
	return 0;
}

int auditsec_task_create(unsigned long clone_flags)
{
	return 0;
}

int auditsec_cred_alloc_blank(struct cred *cred, gfp_t gfp)
{
	return 0;
}

void auditsec_cred_free(struct cred *cred)
{
}

int auditsec_kernel_create_files_as(struct cred *new, struct inode *inode)
{
	return 0;
}

int auditsec_netlink_send(struct sock *sk, struct sk_buff *skb)
{
	return 0;//cap_netlink_send(sk, skb);
}

int auditsec_netlink_recv(struct sk_buff *skb, int cap)
{
	return 0;//cap_netlink_recv(skb, cap);
}
EXPORT_SYMBOL(auditsec_netlink_recv);

#ifdef CONFIG_SECURITY_NETWORK

int auditsec_unix_stream_connect(struct socket *sock, struct socket *other,
				 struct sock *newsk)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_unix_stream_connect);

int auditsec_unix_may_send(struct socket *sock,  struct socket *other)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_unix_may_send);

int auditsec_socket_create(int family, int type, int protocol, int kern)
{
// 	printk(KERN_INFO "AuditSec: socket created ok");
	return 0;
}

int auditsec_socket_post_create(struct socket *sock, int family,
				int type, int protocol, int kern)
{
	return 0;
}

int auditsec_socket_bind(struct socket *sock, struct sockaddr *address, int addrlen)
{
	int		answer = -1;
	int		ret = 0;
	struct sock * sk = sock->sk;
	pid_t 	current_pid = task_pid_nr(current);

	if(prog_is_monitored()){
		if(*daemon_launched()){
			if(down_timeout(auditsec_hook_lock(), 30 * HZ) != 0){// 30s timeout. Is it too much ?
				printk(KERN_INFO "AuditSec: socket bind/connect: pid: %d, execname: %s, HOOK TIMEOUT",
					current_pid, current->comm);
				return -EFAULT;
			}
			
			printk(KERN_INFO "AuditSec: socket bind/connect: pid: %d, execname: %s",
					current_pid, current->comm);
			
			get_task_comm(k_auditsec_info()->execname, current);
			k_auditsec_info()->pid = current_pid;
			k_auditsec_info()->type = AUDITSEC_SOCKET;
			// IP
			if (sk->sk_family == PF_INET) {
				k_auditsec_info()->auditsec_struct.socket.type = AUDITSEC_IPV4;
				memcpy(&k_auditsec_info()->auditsec_struct.socket.addr.addr4, address, sizeof(struct sockaddr_in));
				if (addrlen < sizeof(struct sockaddr_in)) {
					return -EINVAL;
				}
			} else {
				k_auditsec_info()->auditsec_struct.socket.type = AUDITSEC_IPV6;
				memcpy(&k_auditsec_info()->auditsec_struct.socket.addr.addr6, address, sizeof(struct sockaddr_in6));
				if (addrlen < SIN6_LEN_RFC2133) {
					return -EINVAL;
				}
			}
			// TODO Add fields to this struct (se_context)

			up(auditsec_question_lock());
			if(down_timeout(auditsec_answer_lock(), 30 * HZ) != 0){// 30s timeout. Is it too much ?
				printk(KERN_INFO "AuditSec: socket bind/connect: pid: %d, execname: %s, ANSWER TIMEOUT",
					current_pid, current->comm);

				ret = down_trylock(auditsec_question_lock());
				up(auditsec_hook_lock());
				return -EFAULT;
			}

			answer = (*auditsec_answer() == 0);
			up(auditsec_hook_lock());

			return answer == 0 ? 0 : -EACCES;
		}else{
			printk(KERN_INFO "AuditSecu: socket bind/connect: pid: %d, execname: %s, REFUSED : daemon not launched",
				task_pid_nr(current), current->comm);
			return -EFAULT;
		}
	}else{
		/*printk(KERN_INFO "AuditSecu: socket bind: pid: %d, execname: %s",
				task_pid_nr(current), current->comm);*/
	}
	return 0;
}

int auditsec_socket_connect(struct socket *sock, struct sockaddr *address, int addrlen)
{
	return auditsec_socket_bind(sock, address, addrlen);
}

int auditsec_socket_listen(struct socket *sock, int backlog)
{
// 	printk(KERN_INFO "AuditSec: listen ok");
	return 0;
}

int auditsec_socket_accept(struct socket *sock, struct socket *newsock)
{
// 	printk(KERN_INFO "AuditSec: accept ok");
	return 0;
}

int auditsec_socket_sendmsg(struct socket *sock, struct msghdr *msg, int size)
{
	return 0;
}

int auditsec_socket_recvmsg(struct socket *sock, struct msghdr *msg,
			    int size, int flags)
{
	return 0;
}

#endif	/* CONFIG_SECURITY_NETWORK */

static struct security_operations audit_ops = {
	.name =				"audit_ops",

//	.netlink_send =					auditsec_netlink_send,
//	.netlink_recv =					auditsec_netlink_recv,

//	.inode_create =					auditsec_inode_create,
//	.inode_link =					auditsec_inode_link,
//	.inode_unlink =					auditsec_inode_unlink,
//	.inode_symlink =				auditsec_inode_symlink,
	.inode_mkdir =					auditsec_inode_mkdir,
//	.inode_mknod =					auditsec_inode_mknod,
//	.inode_readlink =				auditsec_inode_readlink,
//	.inode_follow_link =			auditsec_inode_follow_link,
//	.inode_permission =				auditsec_inode_permission,

	.file_permission =				auditsec_file_permission,

//	.dentry_open =					auditsec_dentry_open,

#ifdef CONFIG_SECURITY_NETWORK

//	.unix_stream_connect =			auditsec_unix_stream_connect,
//	.unix_may_send =				auditsec_unix_may_send,

//	.socket_create =				auditsec_socket_create,
//	.socket_post_create =			auditsec_socket_post_create,
	.socket_bind =					auditsec_socket_bind,
	.socket_connect =				auditsec_socket_connect,
//	.socket_listen =				auditsec_socket_listen,
//	.socket_accept =				auditsec_socket_accept,
//	.socket_sendmsg =				auditsec_socket_sendmsg,
//	.socket_recvmsg =				auditsec_socket_recvmsg,

#endif
};

int auditsec_procfile_read(char *buffer, char **buffer_location,  off_t offset, int buffer_length, int *eof, void *data)
{
	int result;
	
	if (offset > 0) {
		/* we have finished to read, return 0 */
		result  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		result = sprintf(buffer, "HelloWorld!\n");
	}

	return result;
}
					



static __init int auditsec_init(void)
{
	struct proc_dir_entry * auditsec_procfile;

	if (!security_module_enable(&audit_ops)) {
		printk(KERN_INFO "AuditSec: Abort initialization.\n");
		return 0;
	}

	printk(KERN_INFO "AuditSec: Initializing.\n");

	if (register_security(&audit_ops))
		panic("AuditSec: Unable to register with kernel.\n");

	init_MUTEX(auditsec_hook_lock());
	init_MUTEX(auditsec_question_lock());
	init_MUTEX(auditsec_answer_lock());

	down(auditsec_question_lock());
	down(auditsec_answer_lock());

	auditsec_procfile = create_proc_entry("contextd", 0400, NULL);

	if (auditsec_procfile == NULL) {
		printk(KERN_INFO "AuditSec: failed to creat /proc file");
		remove_proc_entry("contextd", NULL);
		return -ENOMEM;
	}

	auditsec_procfile->read_proc    = auditsec_procfile_read;
	auditsec_procfile->mode         = S_IFREG | S_IRUGO;
	auditsec_procfile->uid          = 0;
	auditsec_procfile->gid          = 0;
	auditsec_procfile->size         = 0;


	printk(KERN_INFO "AuditSec: Waiting for daemon.\n");

	return 0;
}

security_initcall(auditsec_init);
