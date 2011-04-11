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
#include <linux/rwsem.h>

#include "hooks-func.h"
#include "hooks.h"
#include "share.h"

int auditsec_info_len = sizeof(struct auditsec_info);


int auditsec_ptrace_access_check(struct task_struct *child, unsigned int mode)
{
	return 0;//cap_ptrace_access_check(child, mode);
}

int auditsec_ptrace_traceme(struct task_struct *parent)
{
	return 0;//cap_ptrace_traceme(parent);
}

int auditsec_capget(struct task_struct *target, kernel_cap_t *effective, kernel_cap_t *inheritable, kernel_cap_t *permitted)
{
	return 0;//cap_capget(target, effective, inheritable, permitted);
}

int auditsec_capset(struct cred *new, const struct cred *old, const kernel_cap_t *effective, const kernel_cap_t *inheritable, const kernel_cap_t *permitted)
{
	return 0;//cap_capset(new, old, effective, inheritable, permitted);
}

int auditsec_capable(int cap)
{
	return 0; //cap_capable(current, current_cred(), cap, SECURITY_CAP_AUDIT);
}

int auditsec_sysctl(struct ctl_table *table, int op)
{
	return 0;
}

int auditsec_quotactl(int cmds, int type, int id, struct super_block *sb)
{
	return 0;
}

int auditsec_quota_on(struct dentry *dentry)
{
	return 0;
}

int auditsec_syslog(int type)
{
	return 0;//cap_syslog(type);
}

int auditsec_vm_enough_memory(long pages)
{
	WARN_ON(current->mm == NULL);
	return 0;//cap_vm_enough_memory(current->mm, pages);
}

int auditsec_bprm_set_creds(struct linux_binprm *bprm)
{
	return 0;//cap_bprm_set_creds(bprm);
}

void auditsec_bprm_committing_creds(struct linux_binprm *bprm)
{
}

void auditsec_bprm_committed_creds(struct linux_binprm *bprm)
{
}

int auditsec_bprm_secureexec(struct linux_binprm *bprm)
{
	return 0;//cap_bprm_secureexec(bprm);
}

int auditsec_sb_alloc(struct super_block *sb)
{
	return 0;
}

void auditsec_sb_free(struct super_block *sb)
{
}

int auditsec_sb_copy_data(char *orig, char *copy)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_sb_copy_data);

int auditsec_sb_kern_mount(struct super_block *sb, int flags, void *data)
{
	return 0;
}

int auditsec_sb_show_options(struct seq_file *m, struct super_block *sb)
{
	return 0;
}

int auditsec_sb_statfs(struct dentry *dentry)
{
	return 0;
}

int auditsec_sb_mount(char *dev_name, struct path *path,
                       char *type, unsigned long flags, void *data)
{
	return 0;
}

int auditsec_sb_umount(struct vfsmount *mnt, int flags)
{
	return 0;
}

int auditsec_sb_set_mnt_opts(struct super_block *sb,
				struct security_mnt_opts *opts)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_sb_set_mnt_opts);

void auditsec_sb_clone_mnt_opts(const struct super_block *oldsb,
				struct super_block *newsb)
{
}
EXPORT_SYMBOL(auditsec_sb_clone_mnt_opts);

int auditsec_sb_parse_opts_str(char *options, struct security_mnt_opts *opts)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_sb_parse_opts_str);

int auditsec_inode_alloc(struct inode *inode)
{
	inode->i_security = NULL;
	return 0;
}

void auditsec_inode_free(struct inode *inode)
{
}

int auditsec_inode_init_security(struct inode *inode, struct inode *dir,
				  char **name, void **value, size_t *len)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auditsec_inode_init_security);

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
	int		answer = -1, test = false;
	pid_t 	current_pid = task_pid_nr(current);
	char *	fullpath = NULL;

	down_read(auditsec_pid_lock());
	test =	((*daemon_pid() != -1) && (*daemon_pid() != current_pid));
	test &= ((*contextd_pid() != -1) && (*contextd_pid() != current_pid));
	test &= ((*cnotify_pid() != -1) && (*cnotify_pid() != current_pid));
	up_read(auditsec_pid_lock());

	if(test){
		fullpath = vmalloc(PATH_MAX + 1);
		dir_path(dentry, fullpath);

		if(down_timeout(auditsec_hook_lock(), 5000) != 0){ // 10s timeout. Is it too much ?
			printk(KERN_INFO "AuditSec: mkdir: %s, pid: %d, execname: %s, mode: %d HOOK TIMEOUT",
					fullpath, current_pid, current->comm, mode);
			vfree(fullpath);

			/* FIXME WHEN READY */
			*daemon_pid() == -1;
			*contextd_pid() == -1;
			*cnotify_pid() == -1;
			/* FIXME WHEN READY */

			return -1;
		}

		k_auditsec_info()->pid = current_pid;
		get_task_comm(k_auditsec_info()->execname, current);
		k_auditsec_info()->type = AUDITSEC_DIR;
		strncpy(k_auditsec_info()->auditsec_struct.dir.fullpath, fullpath, PATH_MAX + 1);
		strncpy(k_auditsec_info()->auditsec_struct.dir.name, dentry->d_name.name, NAME_MAX + 1);
		k_auditsec_info()->auditsec_struct.dir.mode = mode;
		// TODO Add fields to this struct ? (se_context)

		up(auditsec_question_lock());
		if(down_timeout(auditsec_answer_lock(), 5000) != 0){ // 10s timeout. Is it too much ?
			printk(KERN_INFO "AuditSec: mkdir: %s, pid: %d, execname: %s, mode: %d ANSWER TIMEOUT",
					fullpath, current_pid, current->comm, mode);
			up(auditsec_hook_lock());
			vfree(fullpath);

			/* FIXME WHEN READY */
			*daemon_pid() == -1;
			*contextd_pid() == -1;
			*cnotify_pid() == -1;
			/* FIXME WHEN READY */

			return -1;
		}

		vfree(fullpath);
		answer = (*auditsec_answer() == 0);
		up(auditsec_hook_lock());
		return answer;
	}
/*	printk(KERN_INFO "AuditSec: mkdir: %s, pid: %d, execname: %s, mode: %d",
				fullpath, task_pid_nr(current), current->comm, mode);*/
	vfree(fullpath);

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

int auditsec_inode_setattr(struct dentry *dentry, struct iattr *attr)
{
	return 0;
}
EXPORT_SYMBOL_GPL(auditsec_inode_setattr);

int auditsec_inode_getattr(struct vfsmount *mnt, struct dentry *dentry)
{
	return 0;
}

int auditsec_inode_setxattr(struct dentry *dentry, const char *name,
			    const void *value, size_t size, int flags)
{
	return 0;//cap_inode_setxattr(dentry, name, value, size, flags);
}

void auditsec_inode_post_setxattr(struct dentry *dentry, const char *name,
				  const void *value, size_t size, int flags)
{
}

int auditsec_inode_getxattr(struct dentry *dentry, const char *name)
{
	return 0;
}

int auditsec_inode_listxattr(struct dentry *dentry)
{
	return 0;
}

int auditsec_inode_removexattr(struct dentry *dentry, const char *name)
{
	return 0;//cap_inode_removexattr(dentry, name);
}

void auditsec_inode_getsecid(const struct inode *inode, u32 *secid)
{
	*secid = 0;
}

int auditsec_file_permission(struct file *file, int mask)
{
	int		answer = -1, test = false;
	pid_t 	current_pid = task_pid_nr(current);
	char *	fullpath = NULL;


	down_read(auditsec_pid_lock());
	test =	((*daemon_pid() != -1) && (*daemon_pid() != current_pid));
	test &= ((*contextd_pid() != -1) && (*contextd_pid() != current_pid));
	test &= ((*cnotify_pid() != -1) && (*cnotify_pid() != current_pid));
	up_read(auditsec_pid_lock());

	if(test){
		fullpath = vmalloc(PATH_MAX + 1);
		file_path(file, fullpath);

		if(down_timeout(auditsec_hook_lock(), 500) != 0){ // 10s timeout. Is it too much ?
			printk(KERN_INFO "AuditSec: file access: %s, pid: %d, execname: %s, mask: %d HOOK TIMEOUT",
				fullpath, current_pid, current->comm, mask);
			vfree(fullpath);

			// FIXME WHEN READY 
			down_read(auditsec_pid_lock());
			*daemon_pid() = -1;
			*contextd_pid() = -1;
			*cnotify_pid() = -1;
			up_read(auditsec_pid_lock());
			// FIXME WHEN READY 
		
			up(auditsec_hook_lock());
			return -1;
		}

		k_auditsec_info()->pid = current_pid;
		get_task_comm(k_auditsec_info()->execname, current);
		k_auditsec_info()->type = AUDITSEC_FILE;
		strncpy(k_auditsec_info()->auditsec_struct.file.fullpath, fullpath, PATH_MAX + 1);
		strncpy(k_auditsec_info()->auditsec_struct.file.name, file->f_path.dentry->d_name.name, NAME_MAX + 1);
		k_auditsec_info()->auditsec_struct.file.mask = mask;
		// TODO Add other info to the struct (se_context)

		up(auditsec_question_lock());
		if(down_timeout(auditsec_answer_lock(), 500) != 0){ // 10s timeout. Is it too much ?
			printk(KERN_INFO "AuditSec: file access: %s, pid: %d, execname: %s, mask: %d ANSWER TIMEOUT",
				fullpath, current_pid, current->comm, mask);
			vfree(fullpath);

			// FIXME WHEN READY 
			down_read(auditsec_pid_lock());
			*daemon_pid() = -1;
			*contextd_pid() = -1;
			*cnotify_pid() = -1;
			up_read(auditsec_pid_lock());
			// FIXME WHEN READY 

			up(auditsec_hook_lock());
			return -1;
		}

		vfree(fullpath);
		answer = (*auditsec_answer() == 0);
		up(auditsec_hook_lock());
		return answer;
	}
/*	printk(KERN_INFO "AuditSecu: file access: %s, pid: %d, execname: %s, mask: %d",
				fullpath, task_pid_nr(current), current->comm, mask);*/
	return 0;
}

int auditsec_file_alloc(struct file *file)
{
	return 0;
}

void auditsec_file_free(struct file *file)
{
}

int auditsec_file_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

int auditsec_file_mmap(struct file *file, unsigned long reqprot,
			unsigned long prot, unsigned long flags,
			unsigned long addr, unsigned long addr_only)
{
	return 0;//cap_file_mmap(file, reqprot, prot, flags, addr, addr_only);
}

int auditsec_file_mprotect(struct vm_area_struct *vma, unsigned long reqprot,
			    unsigned long prot)
{
	return 0;
}

int auditsec_file_lock(struct file *file, unsigned int cmd)
{
	return 0;
}

int auditsec_file_fcntl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

int auditsec_file_set_fowner(struct file *file)
{
	return 0;
}

int auditsec_file_send_sigiotask(struct task_struct *tsk,
				  struct fown_struct *fown, int sig)
{
	return 0;
}

int auditsec_file_receive(struct file *file)
{
	return 0;
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

int auditsec_kernel_module_request(void)
{
	return 0;
}

int auditsec_kernel_act_as(struct cred *new, u32 secid)
{
	return 0;
}

void auditsec_transfer_creds(struct cred *new, const struct cred *old)
{
}

int auditsec_prepare_creds(struct cred *new, const struct cred *old, gfp_t gfp)
{
	return 0;
}
int auditsec_task_setpgid(struct task_struct *p, pid_t pgid)
{
	return 0;
}

int auditsec_task_getpgid(struct task_struct *p)
{
	return 0;
}

int auditsec_task_getsid(struct task_struct *p)
{
	return 0;
}

void auditsec_task_getsecid(struct task_struct *p, u32 *secid)
{
	*secid = 0;
}
EXPORT_SYMBOL(auditsec_task_getsecid);

int auditsec_task_setnice(struct task_struct *p, int nice)
{
	return 0;//cap_task_setnice(p, nice);
}

int auditsec_task_setioprio(struct task_struct *p, int ioprio)
{
	return 0;//cap_task_setioprio(p, ioprio);
}

int auditsec_task_getioprio(struct task_struct *p)
{
	return 0;
}

int auditsec_task_setrlimit(unsigned int resource, struct rlimit *new_rlim)
{
	return 0;
}

int auditsec_task_setscheduler(struct task_struct *p,
				int policy, struct sched_param *lp)
{
	return 0;//cap_task_setscheduler(p, policy, lp);
}

int auditsec_task_getscheduler(struct task_struct *p)
{
	return 0;
}

int auditsec_task_movememory(struct task_struct *p)
{
	return 0;
}

int auditsec_task_kill(struct task_struct *p, struct siginfo *info,
			int sig, u32 secid)
{
	return 0;
}

int auditsec_task_wait(struct task_struct *p)
{
	return 0;
}

void auditsec_task_to_inode(struct task_struct *p, struct inode *inode)
{
}

int auditsec_ipc_permission(struct kern_ipc_perm *ipcp, short flag)
{
	return 0;
}

void auditsec_ipc_getsecid(struct kern_ipc_perm *ipcp, u32 *secid)
{
	*secid = 0;
}

int auditsec_msg_msg_alloc(struct msg_msg *msg)
{
	return 0;
}

void auditsec_msg_msg_free(struct msg_msg *msg)
{
}

int auditsec_msg_queue_alloc(struct msg_queue *msq)
{
	return 0;
}

void auditsec_msg_queue_free(struct msg_queue *msq)
{
}

int auditsec_msg_queue_associate(struct msg_queue *msq, int msqflg)
{
	return 0;
}

int auditsec_msg_queue_msgctl(struct msg_queue *msq, int cmd)
{
	return 0;
}

int auditsec_msg_queue_msgsnd(struct msg_queue *msq,
			       struct msg_msg *msg, int msqflg)
{
	return 0;
}

int auditsec_msg_queue_msgrcv(struct msg_queue *msq, struct msg_msg *msg,
			       struct task_struct *target, long type, int mode)
{
	return 0;
}

int auditsec_shm_alloc(struct shmid_kernel *shp)
{
	return 0;
}

void auditsec_shm_free(struct shmid_kernel *shp)
{
}

int auditsec_shm_associate(struct shmid_kernel *shp, int shmflg)
{
	return 0;
}

int auditsec_shm_shmctl(struct shmid_kernel *shp, int cmd)
{
	return 0;
}

int auditsec_shm_shmat(struct shmid_kernel *shp, char __user *shmaddr, int shmflg)
{
	return 0;
}

int auditsec_sem_alloc(struct sem_array *sma)
{
	return 0;
}

void auditsec_sem_free(struct sem_array *sma)
{
}

int auditsec_sem_associate(struct sem_array *sma, int semflg)
{
	return 0;
}

int auditsec_sem_semctl(struct sem_array *sma, int cmd)
{
	return 0;
}

int auditsec_sem_semop(struct sem_array *sma, struct sembuf *sops,
			unsigned nsops, int alter)
{
	return 0;
}

void auditsec_d_instantiate(struct dentry *dentry, struct inode *inode)
{
}
EXPORT_SYMBOL(auditsec_d_instantiate);

int auditsec_getprocattr(struct task_struct *p, char *name, char **value)
{
	return -EINVAL;
}

int auditsec_setprocattr(struct task_struct *p, char *name, void *value, size_t size)
{
	return -EINVAL;
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

int auditsec_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auditsec_secid_to_secctx);

int auditsec_secctx_to_secid(const char *secdata, u32 seclen, u32 *secid)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auditsec_secctx_to_secid);

void auditsec_release_secctx(char *secdata, u32 seclen)
{
}
EXPORT_SYMBOL(auditsec_release_secctx);

int auditsec_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auditsec_inode_notifysecctx);

int auditsec_inode_setsecctx(struct dentry *dentry, void *ctx, u32 ctxlen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auditsec_inode_setsecctx);

int auditsec_inode_getsecctx(struct inode *inode, void **ctx, u32 *ctxlen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auditsec_inode_getsecctx);

int auditsec_inode_getsecurity(const struct inode *inode, const char *name, void **buffer, bool alloc)
{
	return 0;
}

int auditsec_inode_setsecurity(struct inode *inode, const char *name, const void *value, size_t size, int flags)
{
	return 0;
}

int auditsec_inode_listsecurity(struct inode *inode, char *buffer, size_t buffer_size)
{
	return 0;
}

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
// 	printk(KERN_INFO "AuditSec: bind ok");
	return 0;
}

int auditsec_socket_connect(struct socket *sock, struct sockaddr *address, int addrlen)
{
// 	printk(KERN_INFO "AuditSec: connect ok");
	return 0;
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

int auditsec_socket_getsockname(struct socket *sock)
{
	return 0;
}

int auditsec_socket_getpeername(struct socket *sock)
{
	return 0;
}

int auditsec_socket_getsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

int auditsec_socket_setsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

int auditsec_socket_shutdown(struct socket *sock, int how)
{
	return 0;
}

int auditsec_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_sock_rcv_skb);

int auditsec_socket_getpeersec_stream(struct socket *sock, char __user *optval,
				      int __user *optlen, unsigned len)
{
	return -ENOPROTOOPT;
}

int auditsec_socket_getpeersec_dgram(struct socket *sock, struct sk_buff *skb, u32 *secid)
{
	return -ENOPROTOOPT;
}
EXPORT_SYMBOL(auditsec_socket_getpeersec_dgram);

int auditsec_sk_alloc(struct sock *sk, int family, gfp_t priority)
{
	return 0;
}

void auditsec_sk_free(struct sock *sk)
{
}

void auditsec_sk_clone(const struct sock *sk, struct sock *newsk)
{
}

void auditsec_sock_graft(struct sock *sk, struct socket *parent)
{
}
EXPORT_SYMBOL(auditsec_sock_graft);

int auditsec_inet_conn_request(struct sock *sk,
			struct sk_buff *skb, struct request_sock *req)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_inet_conn_request);

void auditsec_inet_csk_clone(struct sock *newsk,
			const struct request_sock *req)
{
}

void auditsec_inet_conn_established(struct sock *sk,
			struct sk_buff *skb)
{
}

int auditsec_tun_dev_create(void)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_tun_dev_create);

void auditsec_tun_dev_post_create(struct sock *sk)
{
}
EXPORT_SYMBOL(auditsec_tun_dev_post_create);

int auditsec_tun_dev_attach(struct sock *sk)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_tun_dev_attach);

void auditsec_req_classify_flow(const struct request_sock *req, struct flowi *fl)
{
}
EXPORT_SYMBOL(auditsec_req_classify_flow);
#endif	/* CONFIG_SECURITY_NETWORK */

#ifdef CONFIG_SECURITY_NETWORK_XFRM

int auditsec_xfrm_policy_alloc(struct xfrm_sec_ctx **ctxp, struct xfrm_user_sec_ctx *sec_ctx)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_xfrm_policy_alloc);

int auditsec_xfrm_policy_clone(struct xfrm_sec_ctx *old_ctx,
			      struct xfrm_sec_ctx **new_ctxp)
{
	return 0;
}

void auditsec_xfrm_policy_free(struct xfrm_sec_ctx *ctx)
{
}
EXPORT_SYMBOL(auditsec_xfrm_policy_free);

int auditsec_xfrm_policy_delete(struct xfrm_sec_ctx *ctx)
{
	return 0;
}

int auditsec_xfrm_state_alloc(struct xfrm_state *x, struct xfrm_user_sec_ctx *sec_ctx)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_xfrm_state_alloc);

int auditsec_xfrm_state_delete(struct xfrm_state *x)
{
	return 0;
}
EXPORT_SYMBOL(auditsec_xfrm_state_delete);

void auditsec_xfrm_state_free(struct xfrm_state *x)
{
}

int auditsec_xfrm_policy_lookup(struct xfrm_sec_ctx *ctx, u32 fl_secid, u8 dir)
{
	return 0;
}

int auditsec_xfrm_state_pol_flow_match(struct xfrm_state *x,
				       struct xfrm_policy *xp, struct flowi *fl)
{
	return 1;
}

int auditsec_xfrm_decode_session(struct sk_buff *skb, u32 *secid)
{
	return 0;
}

#endif

#ifdef CONFIG_KEYS

void auditsec_key_free(struct key *key)
{
}

int auditsec_key_alloc(struct key *key, const struct cred *cred, unsigned long flags)
{
	return 0;
}


int auditsec_key_permission(key_ref_t key_ref,
			    const struct cred *cred, key_perm_t perm)
{
	return 0;
}

int auditsec_key_getsecurity(struct key *key, char **_buffer)
{
	*_buffer = NULL;
	return 0;
}

int auditsec_key_session_to_parent(const struct cred *cred,
				   const struct cred *parent_cred,
				   struct key *key)
{
	return 0;
}

#endif

#ifdef CONFIG_AUDIT

int auditsec_audit_rule_init(u32 field, u32 op, char *rulestr, void **lsmrule)
{
	return 0;
}

int auditsec_audit_rule_known(struct audit_krule *krule)
{
	return 0;
}

void auditsec_audit_rule_free(void *lsmrule)
{
}

int auditsec_audit_rule_match(u32 secid, u32 field, u32 op, void *lsmrule,
			      struct audit_context *actx)
{
	return 0;
}

#endif

static struct security_operations audit_ops = {
	.name =				"audit_ops",

//	.ptrace_access_check =			auditsec_ptrace_access_check,
//	.ptrace_traceme =				auditsec_ptrace_traceme,
//	.capget =						auditsec_capget,
//	.capset =						auditsec_capset,
//	.sysctl =						auditsec_sysctl,
//	.capable =						auditsec_capable,
//	.quotactl =						auditsec_quotactl,
//	.quota_on =						auditsec_quota_on,
//	.syslog =						auditsec_syslog,
//	.vm_enough_memory =				auditsec_vm_enough_memory,

//	.netlink_send =					auditsec_netlink_send,
//	.netlink_recv =					auditsec_netlink_recv,

//	.bprm_set_creds =				auditsec_bprm_set_creds,
//	.bprm_committing_creds =		auditsec_bprm_committing_creds,
//	.bprm_committed_creds =			auditsec_bprm_committed_creds,
//	.bprm_secureexec =				auditsec_bprm_secureexec,

//	.sb_alloc_security =			auditsec_sb_alloc,
//	.sb_free_security =				auditsec_sb_free,
//	.sb_copy_data =					auditsec_sb_copy_data,
//	.sb_kern_mount =				auditsec_sb_kern_mount,
//	.sb_show_options =				auditsec_sb_show_options,
//	.sb_statfs =					auditsec_sb_statfs,
//	.sb_mount =						auditsec_sb_mount,
//	.sb_umount =					auditsec_sb_umount,
//	.sb_set_mnt_opts =				auditsec_sb_set_mnt_opts,
//	.sb_clone_mnt_opts =			auditsec_sb_clone_mnt_opts,
//	.sb_parse_opts_str = 			auditsec_sb_parse_opts_str,

//	.inode_alloc_security =			auditsec_inode_alloc,
//	.inode_free_security =			auditsec_inode_free,
//	.inode_init_security =			auditsec_inode_init_security,
//	.inode_create =					auditsec_inode_create,
//	.inode_link =					auditsec_inode_link,
//	.inode_unlink =					auditsec_inode_unlink,
//	.inode_symlink =				auditsec_inode_symlink,
//	.inode_mkdir =					auditsec_inode_mkdir,
//	.inode_rmdir =					auditsec_inode_rmdir,
//	.inode_mknod =					auditsec_inode_mknod,
//	.inode_rename =					auditsec_inode_rename,
//	.inode_readlink =				auditsec_inode_readlink,
//	.inode_follow_link =			auditsec_inode_follow_link,
//	.inode_permission =				auditsec_inode_permission,
//	.inode_setattr =				auditsec_inode_setattr,
//	.inode_getattr =				auditsec_inode_getattr,
//	.inode_setxattr =				auditsec_inode_setxattr,
//	.inode_post_setxattr =			auditsec_inode_post_setxattr,
//	.inode_getxattr =				auditsec_inode_getxattr,
//	.inode_listxattr =				auditsec_inode_listxattr,
//	.inode_removexattr =			auditsec_inode_removexattr,
//	.inode_getsecurity =			auditsec_inode_getsecurity,
//	.inode_setsecurity =			auditsec_inode_setsecurity,
//	.inode_listsecurity =			auditsec_inode_listsecurity,
//	.inode_getsecid =				auditsec_inode_getsecid,

	.file_permission =				auditsec_file_permission,
//	.file_alloc_security =			auditsec_file_alloc,
//	.file_free_security =			auditsec_file_free,
//	.file_ioctl =					auditsec_file_ioctl,
//	.file_mmap =					auditsec_file_mmap,
//	.file_mprotect =				auditsec_file_mprotect,
//	.file_lock =					auditsec_file_lock,
//	.file_fcntl =					auditsec_file_fcntl,
//	.file_set_fowner =				auditsec_file_set_fowner,
//	.file_send_sigiotask =			auditsec_file_send_sigiotask,
//	.file_receive =					auditsec_file_receive,

//	.dentry_open =					auditsec_dentry_open,

//	.task_create =					auditsec_task_create,
//	.cred_alloc_blank =				auditsec_cred_alloc_blank,
//	.cred_free =					auditsec_cred_free,
//	.cred_prepare =					auditsec_prepare_creds,
//	.cred_transfer =				auditsec_transfer_creds,
//	.kernel_act_as =				auditsec_kernel_act_as,
//	.kernel_create_files_as =		auditsec_kernel_create_files_as,
//	.kernel_module_request =		auditsec_kernel_module_request,
//	.task_setpgid =					auditsec_task_setpgid,
//	.task_getpgid =					auditsec_task_getpgid,
//	.task_getsid =					auditsec_task_getsid,
//	.task_getsecid =				auditsec_task_getsecid,
//	.task_setnice =					auditsec_task_setnice,
//	.task_setioprio =				auditsec_task_setioprio,
//	.task_getioprio =				auditsec_task_getioprio,
//	.task_setrlimit =				auditsec_task_setrlimit,
//	.task_setscheduler =			auditsec_task_setscheduler,
//	.task_getscheduler =			auditsec_task_getscheduler,
//	.task_movememory =				auditsec_task_movememory,
//	.task_kill =					auditsec_task_kill,
//	.task_wait =					auditsec_task_wait,
//	.task_to_inode =				auditsec_task_to_inode,

//	.ipc_permission =				auditsec_ipc_permission,
//	.ipc_getsecid =					auditsec_ipc_getsecid,

//	.msg_msg_alloc_security =		auditsec_msg_msg_alloc,
//	.msg_msg_free_security =		auditsec_msg_msg_free,

//	.msg_queue_alloc_security =		auditsec_msg_queue_alloc,
//	.msg_queue_free_security =		auditsec_msg_queue_free,
//	.msg_queue_associate =			auditsec_msg_queue_associate,
//	.msg_queue_msgctl =				auditsec_msg_queue_msgctl,
//	.msg_queue_msgsnd =				auditsec_msg_queue_msgsnd,
//	.msg_queue_msgrcv =				auditsec_msg_queue_msgrcv,

//	.shm_alloc_security =			auditsec_shm_alloc,
//	.shm_free_security =			auditsec_shm_free,
//	.shm_associate =				auditsec_shm_associate,
//	.shm_shmctl =					auditsec_shm_shmctl,
//	.shm_shmat =					auditsec_shm_shmat,

//	.sem_alloc_security =			auditsec_sem_alloc,
//	.sem_free_security =			auditsec_sem_free,
//	.sem_associate =				auditsec_sem_associate,
//	.sem_semctl =					auditsec_sem_semctl,
//	.sem_semop =					auditsec_sem_semop,

//	.d_instantiate =				auditsec_d_instantiate,

//	.getprocattr =					auditsec_getprocattr,
//	.setprocattr =					auditsec_setprocattr,

//	.secid_to_secctx =				auditsec_secid_to_secctx,
//	.secctx_to_secid =				auditsec_secctx_to_secid,
//	.release_secctx =				auditsec_release_secctx,
//	.inode_notifysecctx =			auditsec_inode_notifysecctx,
//	.inode_setsecctx =				auditsec_inode_setsecctx,
//	.inode_getsecctx =				auditsec_inode_getsecctx,

//	.sk_getsecid =			security_sk_getsecid,

#ifdef CONFIG_SECURITY_NETWORK

//	.unix_stream_connect =			auditsec_unix_stream_connect,
//	.unix_may_send =				auditsec_unix_may_send,

//	.socket_create =				auditsec_socket_create,
//	.socket_post_create =			auditsec_socket_post_create,
//	.socket_bind =					auditsec_socket_bind,
//	.socket_connect =				auditsec_socket_connect,
//	.socket_listen =				auditsec_socket_listen,
//	.socket_accept =				auditsec_socket_accept,
//	.socket_sendmsg =				auditsec_socket_sendmsg,
//	.socket_recvmsg =				auditsec_socket_recvmsg,
//	.socket_getsockname =			auditsec_socket_getsockname,
//	.socket_getpeername =			auditsec_socket_getpeername,
//	.socket_getsockopt =			auditsec_socket_getsockopt,
//	.socket_setsockopt =			auditsec_socket_setsockopt,
//	.socket_shutdown =				auditsec_socket_shutdown,
//	.socket_sock_rcv_skb =			auditsec_sock_rcv_skb,
//	.socket_getpeersec_stream =		auditsec_socket_getpeersec_stream,
//	.socket_getpeersec_dgram =		auditsec_socket_getpeersec_dgram,
//	.sk_alloc_security =			auditsec_sk_alloc,
//	.sk_free_security =				auditsec_sk_free,
//	.sk_clone_security =			auditsec_sk_clone,
//	.sock_graft =					auditsec_sock_graft,
//	.inet_conn_request =			auditsec_inet_conn_request,
//	.inet_csk_clone =				auditsec_inet_csk_clone,
//	.inet_conn_established =		auditsec_inet_conn_established,
//	.req_classify_flow =			auditsec_req_classify_flow,
//	.tun_dev_create =				auditsec_tun_dev_create,
//	.tun_dev_post_create = 			auditsec_tun_dev_post_create,
//	.tun_dev_attach =				auditsec_tun_dev_attach,

#endif

#ifdef CONFIG_SECURITY_NETWORK_XFRM

//	.xfrm_policy_alloc_security =	auditsec_xfrm_policy_alloc,
//	.xfrm_policy_clone_security =	auditsec_xfrm_policy_clone,
//	.xfrm_policy_free_security =	auditsec_xfrm_policy_free,
//	.xfrm_policy_delete_security =	auditsec_xfrm_policy_delete,
//	.xfrm_state_alloc_security =	auditsec_xfrm_state_alloc,
//	.xfrm_state_free_security =		auditsec_xfrm_state_free,
//	.xfrm_state_delete_security =	auditsec_xfrm_state_delete,
//	.xfrm_policy_lookup =			auditsec_xfrm_policy_lookup,
//	.xfrm_state_pol_flow_match =	auditsec_xfrm_state_pol_flow_match,
//	.xfrm_decode_session =			auditsec_xfrm_decode_session,

#endif

#ifdef CONFIG_KEYS

//	.key_alloc =					auditsec_key_alloc,
//	.key_free =						auditsec_key_free,
//	.key_permission =				auditsec_key_permission,
//	.key_getsecurity =				auditsec_key_getsecurity,

#endif

#ifdef CONFIG_AUDIT

//	.audit_rule_init =				auditsec_audit_rule_init,
//	.audit_rule_known =				auditsec_audit_rule_known,
//	.audit_rule_match =				auditsec_audit_rule_match,
//	.audit_rule_free =				auditsec_audit_rule_free,

#endif
};

static __init int auditsec_init(void)
{
	if (!security_module_enable(&audit_ops)) {
		printk(KERN_INFO "AuditSec:  Abort initialization.\n");
		return 0;
	}

	printk(KERN_INFO "AuditSec:  Initializing.\n");

	if (register_security(&audit_ops))
		panic("AuditSec: Unable to register with kernel.\n");

	init_MUTEX(auditsec_hook_lock());
	init_MUTEX(auditsec_question_lock());
	init_MUTEX(auditsec_answer_lock());

	init_rwsem(auditsec_pid_lock());

	down(auditsec_question_lock());
	down(auditsec_answer_lock());

	printk(KERN_INFO "AuditSec:  Waiting for daemon.\n");

	return 0;
}

security_initcall(auditsec_init);
