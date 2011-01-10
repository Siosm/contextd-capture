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
#include <linux/spinlock.h>
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

#include "hooks-func.h"

int audit_security_ptrace_access_check(struct task_struct *child, unsigned int mode)
{
	return 0;//cap_ptrace_access_check(child, mode);
}

int audit_security_ptrace_traceme(struct task_struct *parent)
{
	return 0;//cap_ptrace_traceme(parent);
}

int audit_security_capget(struct task_struct *target, kernel_cap_t *effective, kernel_cap_t *inheritable, kernel_cap_t *permitted)
{
	return 0;//cap_capget(target, effective, inheritable, permitted);
}

int audit_security_capset(struct cred *new, const struct cred *old, const kernel_cap_t *effective, const kernel_cap_t *inheritable, const kernel_cap_t *permitted)
{
	return 0;//cap_capset(new, old, effective, inheritable, permitted);
}

int audit_security_capable(int cap)
{
	return 0; //cap_capable(current, current_cred(), cap, SECURITY_CAP_AUDIT);
}

int audit_security_sysctl(struct ctl_table *table, int op)
{
	return 0;
}

int audit_security_quotactl(int cmds, int type, int id, struct super_block *sb)
{
	return 0;
}

int audit_security_quota_on(struct dentry *dentry)
{
	return 0;
}

int audit_security_syslog(int type)
{
	return 0;//cap_syslog(type);
}

int audit_security_vm_enough_memory(long pages)
{
	WARN_ON(current->mm == NULL);
	return 0;//cap_vm_enough_memory(current->mm, pages);
}

int audit_security_bprm_set_creds(struct linux_binprm *bprm)
{
	return 0;//cap_bprm_set_creds(bprm);
}

void audit_security_bprm_committing_creds(struct linux_binprm *bprm)
{
}

void audit_security_bprm_committed_creds(struct linux_binprm *bprm)
{
}

int audit_security_bprm_secureexec(struct linux_binprm *bprm)
{
	return 0;//cap_bprm_secureexec(bprm);
}

int audit_security_sb_alloc(struct super_block *sb)
{
	return 0;
}

void audit_security_sb_free(struct super_block *sb)
{
}

int audit_security_sb_copy_data(char *orig, char *copy)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_sb_copy_data);

int audit_security_sb_kern_mount(struct super_block *sb, int flags, void *data)
{
	return 0;
}

int audit_security_sb_show_options(struct seq_file *m, struct super_block *sb)
{
	return 0;
}

int audit_security_sb_statfs(struct dentry *dentry)
{
	return 0;
}

int audit_security_sb_mount(char *dev_name, struct path *path,
                       char *type, unsigned long flags, void *data)
{
	return 0;
}

int audit_security_sb_umount(struct vfsmount *mnt, int flags)
{
	return 0;
}

int audit_security_sb_set_mnt_opts(struct super_block *sb,
				struct security_mnt_opts *opts)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_sb_set_mnt_opts);

void audit_security_sb_clone_mnt_opts(const struct super_block *oldsb,
				struct super_block *newsb)
{
}
EXPORT_SYMBOL(audit_security_sb_clone_mnt_opts);

int audit_security_sb_parse_opts_str(char *options, struct security_mnt_opts *opts)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_sb_parse_opts_str);

int audit_security_inode_alloc(struct inode *inode)
{
	inode->i_security = NULL;
	return 0;
}

void audit_security_inode_free(struct inode *inode)
{
}

int audit_security_inode_init_security(struct inode *inode, struct inode *dir,
				  char **name, void **value, size_t *len)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(audit_security_inode_init_security);

int audit_security_inode_create(struct inode *dir, struct dentry *dentry, int mode)
{
	return 0;
}
EXPORT_SYMBOL_GPL(audit_security_inode_create);

int audit_security_inode_link(struct dentry *old_dentry, struct inode *dir,
			 struct dentry *new_dentry)
{
	return 0;
}

int audit_security_inode_unlink(struct inode *dir, struct dentry *dentry)
{
	return 0;
}

int audit_security_inode_symlink(struct inode *dir, struct dentry *dentry,
			    const char *old_name)
{
	return 0;
}

int audit_security_inode_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	char * path = dentry_path_(dentry);
	printk(KERN_INFO "Audit Security: Dossier cree: %s", path);
	vfree(path);
	return 0;
}
EXPORT_SYMBOL_GPL(audit_security_inode_mkdir);

int audit_security_inode_rmdir(struct inode *dir, struct dentry *dentry)
{
	return 0;
}

int audit_security_inode_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev)
{
	return 0;
}

int audit_security_inode_rename(struct inode *old_dir, struct dentry *old_dentry,
			   struct inode *new_dir, struct dentry *new_dentry)
{
	return 0;
}

int audit_security_inode_readlink(struct dentry *dentry)
{
	return 0;
}

int audit_security_inode_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	return 0;
}

int audit_security_inode_permission(struct inode *inode, int mask)
{
	return 0;
}

int audit_security_inode_setattr(struct dentry *dentry, struct iattr *attr)
{
	return 0;
}
EXPORT_SYMBOL_GPL(audit_security_inode_setattr);

int audit_security_inode_getattr(struct vfsmount *mnt, struct dentry *dentry)
{
	return 0;
}

int audit_security_inode_setxattr(struct dentry *dentry, const char *name,
			    const void *value, size_t size, int flags)
{
	return 0;//cap_inode_setxattr(dentry, name, value, size, flags);
}

void audit_security_inode_post_setxattr(struct dentry *dentry, const char *name,
				  const void *value, size_t size, int flags)
{
}

int audit_security_inode_getxattr(struct dentry *dentry, const char *name)
{
	return 0;
}

int audit_security_inode_listxattr(struct dentry *dentry)
{
	return 0;
}

int audit_security_inode_removexattr(struct dentry *dentry, const char *name)
{
	return 0;//cap_inode_removexattr(dentry, name);
}

void audit_security_inode_getsecid(const struct inode *inode, u32 *secid)
{
	*secid = 0;
}

int audit_security_file_permission(struct file *file, int mask)
{
	char * path = dentry_path_(file->f_path.dentry);
	char * mnt_point = mount_point(file);
	pid_t pid= task_pid_nr(current); 	
	
	if (path == NULL) {
		return 0;
	}

	if(mnt_point != NULL) {
	//	printk(KERN_INFO "AuSecu: Acces au fichier : %s%s (PID %d EXECNAME %s) mask: %d", mnt_point, path, pid, current->comm, mask);
	} else {
	//	printk(KERN_INFO "AuSecu: Acces au fichier : %s (PID %d EXECNAME %s) mask: %d", path, pid, current->comm, mask);
	}
	vfree(path);
	return 0;
}

int audit_security_file_alloc(struct file *file)
{
	return 0;
}

void audit_security_file_free(struct file *file)
{
}

int audit_security_file_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

int audit_security_file_mmap(struct file *file, unsigned long reqprot,
			unsigned long prot, unsigned long flags,
			unsigned long addr, unsigned long addr_only)
{
	return 0;//cap_file_mmap(file, reqprot, prot, flags, addr, addr_only);
}

int audit_security_file_mprotect(struct vm_area_struct *vma, unsigned long reqprot,
			    unsigned long prot)
{
	return 0;
}

int audit_security_file_lock(struct file *file, unsigned int cmd)
{
	return 0;
}

int audit_security_file_fcntl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

int audit_security_file_set_fowner(struct file *file)
{
	return 0;
}

int audit_security_file_send_sigiotask(struct task_struct *tsk,
				  struct fown_struct *fown, int sig)
{
	return 0;
}

int audit_security_file_receive(struct file *file)
{
	return 0;
}

int audit_security_dentry_open(struct file *file, const struct cred *cred)
{
	return 0;
}

int audit_security_task_create(unsigned long clone_flags)
{
	return 0;
}

int audit_security_cred_alloc_blank(struct cred *cred, gfp_t gfp)
{
	return 0;
}

void audit_security_cred_free(struct cred *cred)
{
}

int audit_security_kernel_create_files_as(struct cred *new, struct inode *inode)
{
	return 0;
}

int audit_security_kernel_module_request(void)
{
	return 0;
}

int audit_security_kernel_act_as(struct cred *new, u32 secid)
{
	return 0;
}

void audit_security_transfer_creds(struct cred *new, const struct cred *old)
{
}

int audit_security_prepare_creds(struct cred *new, const struct cred *old, gfp_t gfp)
{
	return 0;
}
int audit_security_task_setpgid(struct task_struct *p, pid_t pgid)
{
	return 0;
}

int audit_security_task_getpgid(struct task_struct *p)
{
	return 0;
}

int audit_security_task_getsid(struct task_struct *p)
{
	return 0;
}

void audit_security_task_getsecid(struct task_struct *p, u32 *secid)
{
	*secid = 0;
}
EXPORT_SYMBOL(audit_security_task_getsecid);

int audit_security_task_setnice(struct task_struct *p, int nice)
{
	return 0;//cap_task_setnice(p, nice);
}

int audit_security_task_setioprio(struct task_struct *p, int ioprio)
{
	return 0;//cap_task_setioprio(p, ioprio);
}

int audit_security_task_getioprio(struct task_struct *p)
{
	return 0;
}

int audit_security_task_setrlimit(unsigned int resource, struct rlimit *new_rlim)
{
	return 0;
}

int audit_security_task_setscheduler(struct task_struct *p,
				int policy, struct sched_param *lp)
{
	return 0;//cap_task_setscheduler(p, policy, lp);
}

int audit_security_task_getscheduler(struct task_struct *p)
{
	return 0;
}

int audit_security_task_movememory(struct task_struct *p)
{
	return 0;
}

int audit_security_task_kill(struct task_struct *p, struct siginfo *info,
			int sig, u32 secid)
{
	return 0;
}

int audit_security_task_wait(struct task_struct *p)
{
	return 0;
}

void audit_security_task_to_inode(struct task_struct *p, struct inode *inode)
{
}

int audit_security_ipc_permission(struct kern_ipc_perm *ipcp, short flag)
{
	return 0;
}

void audit_security_ipc_getsecid(struct kern_ipc_perm *ipcp, u32 *secid)
{
	*secid = 0;
}

int audit_security_msg_msg_alloc(struct msg_msg *msg)
{
	return 0;
}

void audit_security_msg_msg_free(struct msg_msg *msg)
{
}

int audit_security_msg_queue_alloc(struct msg_queue *msq)
{
	return 0;
}

void audit_security_msg_queue_free(struct msg_queue *msq)
{
}

int audit_security_msg_queue_associate(struct msg_queue *msq, int msqflg)
{
	return 0;
}

int audit_security_msg_queue_msgctl(struct msg_queue *msq, int cmd)
{
	return 0;
}

int audit_security_msg_queue_msgsnd(struct msg_queue *msq,
			       struct msg_msg *msg, int msqflg)
{
	return 0;
}

int audit_security_msg_queue_msgrcv(struct msg_queue *msq, struct msg_msg *msg,
			       struct task_struct *target, long type, int mode)
{
	return 0;
}

int audit_security_shm_alloc(struct shmid_kernel *shp)
{
	return 0;
}

void audit_security_shm_free(struct shmid_kernel *shp)
{
}

int audit_security_shm_associate(struct shmid_kernel *shp, int shmflg)
{
	return 0;
}

int audit_security_shm_shmctl(struct shmid_kernel *shp, int cmd)
{
	return 0;
}

int audit_security_shm_shmat(struct shmid_kernel *shp, char __user *shmaddr, int shmflg)
{
	return 0;
}

int audit_security_sem_alloc(struct sem_array *sma)
{
	return 0;
}

void audit_security_sem_free(struct sem_array *sma)
{
}

int audit_security_sem_associate(struct sem_array *sma, int semflg)
{
	return 0;
}

int audit_security_sem_semctl(struct sem_array *sma, int cmd)
{
	return 0;
}

int audit_security_sem_semop(struct sem_array *sma, struct sembuf *sops,
			unsigned nsops, int alter)
{
	return 0;
}

void audit_security_d_instantiate(struct dentry *dentry, struct inode *inode)
{
}
EXPORT_SYMBOL(audit_security_d_instantiate);

int audit_security_getprocattr(struct task_struct *p, char *name, char **value)
{
	return -EINVAL;
}

int audit_security_setprocattr(struct task_struct *p, char *name, void *value, size_t size)
{
	return -EINVAL;
}

int audit_security_netlink_send(struct sock *sk, struct sk_buff *skb)
{
	return 0;//cap_netlink_send(sk, skb);
}

int audit_security_netlink_recv(struct sk_buff *skb, int cap)
{
	return 0;//cap_netlink_recv(skb, cap);
}
EXPORT_SYMBOL(audit_security_netlink_recv);

int audit_security_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(audit_security_secid_to_secctx);

int audit_security_secctx_to_secid(const char *secdata, u32 seclen, u32 *secid)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(audit_security_secctx_to_secid);

void audit_security_release_secctx(char *secdata, u32 seclen)
{
}
EXPORT_SYMBOL(audit_security_release_secctx);

int audit_security_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(audit_security_inode_notifysecctx);

int audit_security_inode_setsecctx(struct dentry *dentry, void *ctx, u32 ctxlen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(audit_security_inode_setsecctx);

int audit_security_inode_getsecctx(struct inode *inode, void **ctx, u32 *ctxlen)
{
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(audit_security_inode_getsecctx);

int audit_security_inode_getsecurity(const struct inode *inode, const char *name, void **buffer, bool alloc)
{
	return 0;
}

int audit_security_inode_setsecurity(struct inode *inode, const char *name, const void *value, size_t size, int flags)
{
	return 0;
}

int audit_security_inode_listsecurity(struct inode *inode, char *buffer, size_t buffer_size)
{
	return 0;
}

#ifdef CONFIG_SECURITY_NETWORK

int audit_security_unix_stream_connect(struct socket *sock, struct socket *other,
				 struct sock *newsk)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_unix_stream_connect);

int audit_security_unix_may_send(struct socket *sock,  struct socket *other)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_unix_may_send);

int audit_security_socket_create(int family, int type, int protocol, int kern)
{
	printk(KERN_INFO "Audit Security: socket created ok");
	return 0;
}

int audit_security_socket_post_create(struct socket *sock, int family,
				int type, int protocol, int kern)
{
	return 0;
}

int audit_security_socket_bind(struct socket *sock, struct sockaddr *address, int addrlen)
{
	printk(KERN_INFO "Audit Security: bind ok");
	return 0;
}

int audit_security_socket_connect(struct socket *sock, struct sockaddr *address, int addrlen)
{
	printk(KERN_INFO "Audit Security: connect ok");
	return 0;
}

int audit_security_socket_listen(struct socket *sock, int backlog)
{
	printk(KERN_INFO "Audit Security: listen ok");
	return 0;
}

int audit_security_socket_accept(struct socket *sock, struct socket *newsock)
{
	printk(KERN_INFO "Audit Security: accept ok");
	return 0;
}

int audit_security_socket_sendmsg(struct socket *sock, struct msghdr *msg, int size)
{
	return 0;
}

int audit_security_socket_recvmsg(struct socket *sock, struct msghdr *msg,
			    int size, int flags)
{
	return 0;
}

int audit_security_socket_getsockname(struct socket *sock)
{
	return 0;
}

int audit_security_socket_getpeername(struct socket *sock)
{
	return 0;
}

int audit_security_socket_getsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

int audit_security_socket_setsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

int audit_security_socket_shutdown(struct socket *sock, int how)
{
	return 0;
}

int audit_security_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_sock_rcv_skb);

int audit_security_socket_getpeersec_stream(struct socket *sock, char __user *optval,
				      int __user *optlen, unsigned len)
{
	return -ENOPROTOOPT;
}

int audit_security_socket_getpeersec_dgram(struct socket *sock, struct sk_buff *skb, u32 *secid)
{
	return -ENOPROTOOPT;
}
EXPORT_SYMBOL(audit_security_socket_getpeersec_dgram);

int audit_security_sk_alloc(struct sock *sk, int family, gfp_t priority)
{
	return 0;
}

void audit_security_sk_free(struct sock *sk)
{
}

void audit_security_sk_clone(const struct sock *sk, struct sock *newsk)
{
}

void audit_security_sock_graft(struct sock *sk, struct socket *parent)
{
}
EXPORT_SYMBOL(audit_security_sock_graft);

int audit_security_inet_conn_request(struct sock *sk,
			struct sk_buff *skb, struct request_sock *req)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_inet_conn_request);

void audit_security_inet_csk_clone(struct sock *newsk,
			const struct request_sock *req)
{
}

void audit_security_inet_conn_established(struct sock *sk,
			struct sk_buff *skb)
{
}

int audit_security_tun_dev_create(void)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_tun_dev_create);

void audit_security_tun_dev_post_create(struct sock *sk)
{
}
EXPORT_SYMBOL(audit_security_tun_dev_post_create);

int audit_security_tun_dev_attach(struct sock *sk)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_tun_dev_attach);

void audit_security_req_classify_flow(const struct request_sock *req, struct flowi *fl)
{
}
EXPORT_SYMBOL(audit_security_req_classify_flow);
#endif	/* CONFIG_SECURITY_NETWORK */

#ifdef CONFIG_SECURITY_NETWORK_XFRM

int audit_security_xfrm_policy_alloc(struct xfrm_sec_ctx **ctxp, struct xfrm_user_sec_ctx *sec_ctx)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_xfrm_policy_alloc);

int audit_security_xfrm_policy_clone(struct xfrm_sec_ctx *old_ctx,
			      struct xfrm_sec_ctx **new_ctxp)
{
	return 0;
}

void audit_security_xfrm_policy_free(struct xfrm_sec_ctx *ctx)
{
}
EXPORT_SYMBOL(audit_security_xfrm_policy_free);

int audit_security_xfrm_policy_delete(struct xfrm_sec_ctx *ctx)
{
	return 0;
}

int audit_security_xfrm_state_alloc(struct xfrm_state *x, struct xfrm_user_sec_ctx *sec_ctx)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_xfrm_state_alloc);

int audit_security_xfrm_state_delete(struct xfrm_state *x)
{
	return 0;
}
EXPORT_SYMBOL(audit_security_xfrm_state_delete);

void audit_security_xfrm_state_free(struct xfrm_state *x)
{
}

int audit_security_xfrm_policy_lookup(struct xfrm_sec_ctx *ctx, u32 fl_secid, u8 dir)
{
	return 0;
}

int audit_security_xfrm_state_pol_flow_match(struct xfrm_state *x,
				       struct xfrm_policy *xp, struct flowi *fl)
{
	return 1;
}

int audit_security_xfrm_decode_session(struct sk_buff *skb, u32 *secid)
{
	return 0;
}

#endif

#ifdef CONFIG_KEYS

void audit_security_key_free(struct key *key)
{
}

int audit_security_key_alloc(struct key *key, const struct cred *cred, unsigned long flags)
{
	return 0;
}


int audit_security_key_permission(key_ref_t key_ref,
			    const struct cred *cred, key_perm_t perm)
{
	return 0;
}

int audit_security_key_getsecurity(struct key *key, char **_buffer)
{
	*_buffer = NULL;
	return 0;
}

int audit_security_key_session_to_parent(const struct cred *cred,
				   const struct cred *parent_cred,
				   struct key *key)
{
	return 0;
}

#endif

#ifdef CONFIG_AUDIT

int audit_security_audit_rule_init(u32 field, u32 op, char *rulestr, void **lsmrule)
{
	return 0;
}

int audit_security_audit_rule_known(struct audit_krule *krule)
{
	return 0;
}

void audit_security_audit_rule_free(void *lsmrule)
{
}

int audit_security_audit_rule_match(u32 secid, u32 field, u32 op, void *lsmrule,
			      struct audit_context *actx)
{
	return 0;
}

#endif

static struct security_operations audit_ops = {
	.name =				"audit_ops",

//	.ptrace_access_check =			audit_security_ptrace_access_check,
//	.ptrace_traceme =				audit_security_ptrace_traceme,
//	.capget =						audit_security_capget,
//	.capset =						audit_security_capset,
//	.sysctl =						audit_security_sysctl,
//	.capable =						audit_security_capable,
//	.quotactl =						audit_security_quotactl,
//	.quota_on =						audit_security_quota_on,
//	.syslog =						audit_security_syslog,
//	.vm_enough_memory =				audit_security_vm_enough_memory,

//	.netlink_send =					audit_security_netlink_send,
//	.netlink_recv =					audit_security_netlink_recv,

//	.bprm_set_creds =				audit_security_bprm_set_creds,
//	.bprm_committing_creds =		audit_security_bprm_committing_creds,
//	.bprm_committed_creds =			audit_security_bprm_committed_creds,
//	.bprm_secureexec =				audit_security_bprm_secureexec,

//	.sb_alloc_security =			audit_security_sb_alloc,
//	.sb_free_security =				audit_security_sb_free,
//	.sb_copy_data =					audit_security_sb_copy_data,
//	.sb_kern_mount =				audit_security_sb_kern_mount,
//	.sb_show_options =				audit_security_sb_show_options,
//	.sb_statfs =					audit_security_sb_statfs,
//	.sb_mount =						audit_security_sb_mount,
//	.sb_umount =					audit_security_sb_umount,
//	.sb_set_mnt_opts =				audit_security_sb_set_mnt_opts,
//	.sb_clone_mnt_opts =			audit_security_sb_clone_mnt_opts,
//	.sb_parse_opts_str = 			audit_security_sb_parse_opts_str,

//	.inode_alloc_security =			audit_security_inode_alloc,
//	.inode_free_security =			audit_security_inode_free,
//	.inode_init_security =			audit_security_inode_init_security,
//	.inode_create =					audit_security_inode_create,
//	.inode_link =					audit_security_inode_link,
//	.inode_unlink =					audit_security_inode_unlink,
//	.inode_symlink =				audit_security_inode_symlink,
	.inode_mkdir =					audit_security_inode_mkdir,
//	.inode_rmdir =					audit_security_inode_rmdir,
//	.inode_mknod =					audit_security_inode_mknod,
//	.inode_rename =					audit_security_inode_rename,
//	.inode_readlink =				audit_security_inode_readlink,
//	.inode_follow_link =			audit_security_inode_follow_link,
//	.inode_permission =				audit_security_inode_permission,
//	.inode_setattr =				audit_security_inode_setattr,
//	.inode_getattr =				audit_security_inode_getattr,
//	.inode_setxattr =				audit_security_inode_setxattr,
//	.inode_post_setxattr =			audit_security_inode_post_setxattr,
//	.inode_getxattr =				audit_security_inode_getxattr,
//	.inode_listxattr =				audit_security_inode_listxattr,
//	.inode_removexattr =			audit_security_inode_removexattr,
//	.inode_getsecurity =			audit_security_inode_getsecurity,
//	.inode_setsecurity =			audit_security_inode_setsecurity,
//	.inode_listsecurity =			audit_security_inode_listsecurity,
//	.inode_getsecid =				audit_security_inode_getsecid,

	.file_permission =				audit_security_file_permission,
//	.file_alloc_security =			audit_security_file_alloc,
//	.file_free_security =			audit_security_file_free,
//	.file_ioctl =					audit_security_file_ioctl,
//	.file_mmap =					audit_security_file_mmap,
//	.file_mprotect =				audit_security_file_mprotect,
//	.file_lock =					audit_security_file_lock,
//	.file_fcntl =					audit_security_file_fcntl,
//	.file_set_fowner =				audit_security_file_set_fowner,
//	.file_send_sigiotask =			audit_security_file_send_sigiotask,
//	.file_receive =					audit_security_file_receive,

//	.dentry_open =					audit_security_dentry_open,

//	.task_create =					audit_security_task_create,
//	.cred_alloc_blank =				audit_security_cred_alloc_blank,
//	.cred_free =					audit_security_cred_free,
//	.cred_prepare =					audit_security_prepare_creds,
//	.cred_transfer =				audit_security_transfer_creds,
//	.kernel_act_as =				audit_security_kernel_act_as,
//	.kernel_create_files_as =		audit_security_kernel_create_files_as,
//	.kernel_module_request =		audit_security_kernel_module_request,
//	.task_setpgid =					audit_security_task_setpgid,
//	.task_getpgid =					audit_security_task_getpgid,
//	.task_getsid =					audit_security_task_getsid,
//	.task_getsecid =				audit_security_task_getsecid,
//	.task_setnice =					audit_security_task_setnice,
//	.task_setioprio =				audit_security_task_setioprio,
//	.task_getioprio =				audit_security_task_getioprio,
//	.task_setrlimit =				audit_security_task_setrlimit,
//	.task_setscheduler =			audit_security_task_setscheduler,
//	.task_getscheduler =			audit_security_task_getscheduler,
//	.task_movememory =				audit_security_task_movememory,
//	.task_kill =					audit_security_task_kill,
//	.task_wait =					audit_security_task_wait,
//	.task_to_inode =				audit_security_task_to_inode,

//	.ipc_permission =				audit_security_ipc_permission,
//	.ipc_getsecid =					audit_security_ipc_getsecid,

//	.msg_msg_alloc_security =		audit_security_msg_msg_alloc,
//	.msg_msg_free_security =		audit_security_msg_msg_free,

//	.msg_queue_alloc_security =		audit_security_msg_queue_alloc,
//	.msg_queue_free_security =		audit_security_msg_queue_free,
//	.msg_queue_associate =			audit_security_msg_queue_associate,
//	.msg_queue_msgctl =				audit_security_msg_queue_msgctl,
//	.msg_queue_msgsnd =				audit_security_msg_queue_msgsnd,
//	.msg_queue_msgrcv =				audit_security_msg_queue_msgrcv,

//	.shm_alloc_security =			audit_security_shm_alloc,
//	.shm_free_security =			audit_security_shm_free,
//	.shm_associate =				audit_security_shm_associate,
//	.shm_shmctl =					audit_security_shm_shmctl,
//	.shm_shmat =					audit_security_shm_shmat,

//	.sem_alloc_security =			audit_security_sem_alloc,
//	.sem_free_security =			audit_security_sem_free,
//	.sem_associate =				audit_security_sem_associate,
//	.sem_semctl =					audit_security_sem_semctl,
//	.sem_semop =					audit_security_sem_semop,

//	.d_instantiate =				audit_security_d_instantiate,

//	.getprocattr =					audit_security_getprocattr,
//	.setprocattr =					audit_security_setprocattr,

//	.secid_to_secctx =				audit_security_secid_to_secctx,
//	.secctx_to_secid =				audit_security_secctx_to_secid,
//	.release_secctx =				audit_security_release_secctx,
//	.inode_notifysecctx =			audit_security_inode_notifysecctx,
//	.inode_setsecctx =				audit_security_inode_setsecctx,
//	.inode_getsecctx =				audit_security_inode_getsecctx,

//	.sk_getsecid =			security_sk_getsecid,

#ifdef CONFIG_SECURITY_NETWORK

//	.unix_stream_connect =			audit_security_unix_stream_connect,
//	.unix_may_send =				audit_security_unix_may_send,

//	.socket_create =				audit_security_socket_create,
//	.socket_post_create =			audit_security_socket_post_create,
//	.socket_bind =					audit_security_socket_bind,
//	.socket_connect =				audit_security_socket_connect,
//	.socket_listen =				audit_security_socket_listen,
//	.socket_accept =				audit_security_socket_accept,
//	.socket_sendmsg =				audit_security_socket_sendmsg,
//	.socket_recvmsg =				audit_security_socket_recvmsg,
//	.socket_getsockname =			audit_security_socket_getsockname,
//	.socket_getpeername =			audit_security_socket_getpeername,
//	.socket_getsockopt =			audit_security_socket_getsockopt,
//	.socket_setsockopt =			audit_security_socket_setsockopt,
//	.socket_shutdown =				audit_security_socket_shutdown,
//	.socket_sock_rcv_skb =			audit_security_sock_rcv_skb,
//	.socket_getpeersec_stream =		audit_security_socket_getpeersec_stream,
//	.socket_getpeersec_dgram =		audit_security_socket_getpeersec_dgram,
//	.sk_alloc_security =			audit_security_sk_alloc,
//	.sk_free_security =				audit_security_sk_free,
//	.sk_clone_security =			audit_security_sk_clone,
//	.sock_graft =					audit_security_sock_graft,
//	.inet_conn_request =			audit_security_inet_conn_request,
//	.inet_csk_clone =				audit_security_inet_csk_clone,
//	.inet_conn_established =		audit_security_inet_conn_established,
//	.req_classify_flow =			audit_security_req_classify_flow,
//	.tun_dev_create =				audit_security_tun_dev_create,
//	.tun_dev_post_create = 			audit_security_tun_dev_post_create,
//	.tun_dev_attach =				audit_security_tun_dev_attach,

#endif

#ifdef CONFIG_SECURITY_NETWORK_XFRM

//	.xfrm_policy_alloc_security =	audit_security_xfrm_policy_alloc,
//	.xfrm_policy_clone_security =	audit_security_xfrm_policy_clone,
//	.xfrm_policy_free_security =	audit_security_xfrm_policy_free,
//	.xfrm_policy_delete_security =	audit_security_xfrm_policy_delete,
//	.xfrm_state_alloc_security =	audit_security_xfrm_state_alloc,
//	.xfrm_state_free_security =		audit_security_xfrm_state_free,
//	.xfrm_state_delete_security =	audit_security_xfrm_state_delete,
//	.xfrm_policy_lookup =			audit_security_xfrm_policy_lookup,
//	.xfrm_state_pol_flow_match =	audit_security_xfrm_state_pol_flow_match,
//	.xfrm_decode_session =			audit_security_xfrm_decode_session,

#endif

#ifdef CONFIG_KEYS

//	.key_alloc =					audit_security_key_alloc,
//	.key_free =						audit_security_key_free,
//	.key_permission =				audit_security_key_permission,
//	.key_getsecurity =				audit_security_key_getsecurity,

#endif

#ifdef CONFIG_AUDIT

//	.audit_rule_init =				audit_security_audit_rule_init,
//	.audit_rule_known =				audit_security_audit_rule_known,
//	.audit_rule_match =				audit_security_audit_rule_match,
//	.audit_rule_free =				audit_security_audit_rule_free,

#endif
};

static __init int audit_security_init(void)
{
	if (!security_module_enable(&audit_ops)) {
		printk(KERN_INFO "Audit Security:  Abort initialization.\n");
		return 0;
	}

	printk(KERN_INFO "Audit Security:  Initializing.\n");

	if (register_security(&audit_ops))
		panic("Audit Security: Unable to register with kernel.\n");
	
	if (sys_mknod("/dev/auditsecurity", 700, S_IFIFO))
		panic("Audit Security: Unable to create fifo.\n");

	return 0;
}

security_initcall(audit_security_init);
