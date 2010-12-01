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
#include <linux/selinux.h>
#include <linux/mutex.h>
#include <linux/posix-timers.h>

/* Definitions des hooks */
// int audit_security_ptrace_access_check(struct task_struct *child, unsigned int mode)
// {
// 	 return ptrace_access_check(struct task_struct *child, unsigned int mode);
// }
// 
// int audit_security_ptrace_traceme(struct task_struct *parent)
// {
// 	 return 1;
// }
// 
// int audit_security_capget(struct task_struct *target, kernel_cap_t *effective, kernel_cap_t *inheritable, kernel_cap_t *permitted)
// {
// 	 return 1;
// }
// 
// int audit_security_capset(struct cred *new, const struct cred *old,  const kernel_cap_t *effective, const kernel_cap_t *inheritable, const kernel_cap_t *permitted)
// {
// 	 return 1;
// }
// 
// int audit_security_capable(int cap)
// {
// 	 return 1;
// }
// 
// int audit_security_real_capable(struct task_struct *tsk, int cap)
// {
// 	 return 1;
// }
// 
// int audit_security_real_capable_noaudit(struct task_struct *tsk, int cap)
// {
// 	 return 1;
// }
// 
// int audit_security_acct(struct file *file)
// {
// 	 return 1;
// }
// 
// int audit_security_sysctl(struct ctl_table *table, int op)
// {
// 	 return 1;
// }
// 
// int audit_security_quotactl(int cmds, int type, int id, struct super_block *sb)
// {
// 	 return 1;
// }
// 
// int audit_security_quota_on(struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_syslog(int type)
// {
// 	 return 1;
// }
// 
// int audit_security_settime(struct timespec *ts, struct timezone *tz)
// {
// 	 return 1;
// }
// 
// int audit_security_vm_enough_memory(long pages)
// {
// 	 return 1;
// }
// 
// int audit_security_vm_enough_memory_mm(struct mm_struct *mm, long pages)
// {
// 	 return 1;
// }
// 
// int audit_security_vm_enough_memory_kern(long pages)
// {
// 	 return 1;
// }
// 
// int audit_security_bprm_set_creds(struct linux_binprm *bprm)
// {
// 	 return 1;
// }
// 
// int audit_security_bprm_check(struct linux_binprm *bprm)
// {
// 	 return 1;
// }
// 
// void audit_security_bprm_committing_creds(struct linux_binprm *bprm)
// {
// 	 return;
// }
// 
// void audit_security_bprm_committed_creds(struct linux_binprm *bprm)
// {
// 	 return;
// }
// 
// int audit_security_bprm_secureexec(struct linux_binprm *bprm)
// {
// 	 return 1;
// }
// 
// int audit_security_sb_alloc(struct super_block *sb)
// {
// 	 return 1;
// }
// 
// void audit_security_sb_free(struct super_block *sb)
// {
// 	 return;
// }
// 
// int audit_security_sb_copy_data(char *orig, char *copy)
// {
// 	 return 1;
// }
// 
// int audit_security_sb_kern_mount(struct super_block *sb, int flags, void *data)
// {
// 	 return 1;
// }
// 
// int audit_security_sb_show_options(struct seq_file *m, struct super_block *sb)
// {
// 	 return 1;
// }
// 
// int audit_security_sb_statfs(struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_sb_mount(char *dev_name, struct path *path, char *type, unsigned long flags, void *data)
// {
// 	 return 1;
// }
// 
// int audit_security_sb_check_sb(struct vfsmount *mnt, struct path *path)
// {
// 	 return 1;
// }
// 
// int audit_security_sb_umount(struct vfsmount *mnt, int flags)
// {
// 	 return 1;
// }
// 
// void audit_security_sb_umount_close(struct vfsmount *mnt)
// {
// 	 return;
// }
// 
// void audit_security_sb_umount_busy(struct vfsmount *mnt)
// {
// 	 return;
// }
// 
// void audit_security_sb_post_remount(struct vfsmount *mnt, unsigned long flags, void *data)
// {
// 	 return;
// }
// 
// void audit_security_sb_post_addmount(struct vfsmount *mnt, struct path *mountpoint)
// {
// 	 return;
// }
// 
// int audit_security_sb_pivotroot(struct path *old_path, struct path *new_path)
// {
// 	 return 1;
// }
// 
// void audit_security_sb_post_pivotroot(struct path *old_path, struct path *new_path)
// {
// 	 return;
// }
// 
// int audit_security_sb_set_mnt_opts(struct super_block *sb, struct security_mnt_opts *opts)
// {
// 	 return 1;
// }
// 
// void audit_security_sb_clone_mnt_opts(const struct super_block *oldsb, struct super_block *newsb)
// {
// 	 return;
// }
// 
// int audit_security_sb_parse_opts_str(char *options, struct security_mnt_opts *opts)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_alloc(struct inode *inode)
// {
// 	 return 1;
// }
// 
// void audit_security_inode_free(struct inode *inode)
// {
// 	 return;
// }
// 
// int audit_security_inode_init_security(struct inode *inode, struct inode *dir, char **name, void **value, size_t *len)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_create(struct inode *dir, struct dentry *dentry, int mode)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_link(struct dentry *old_dentry, struct inode *dir, struct dentry *new_dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_unlink(struct inode *dir, struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_symlink(struct inode *dir, struct dentry *dentry, const char *old_name)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_mkdir(struct inode *dir, struct dentry *dentry, int mode)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_rmdir(struct inode *dir, struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_rename(struct inode *old_dir, struct dentry *old_dentry, struct inode *new_dir, struct dentry *new_dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_readlink(struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_follow_link(struct dentry *dentry, struct nameidata *nd)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_permission(struct inode *inode, int mask)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_setattr(struct dentry *dentry, struct iattr *attr)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_getattr(struct vfsmount *mnt, struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// void audit_security_inode_delete(struct inode *inode)
// {
// 	 return;
// }
// 
// int audit_security_inode_setxattr(struct dentry *dentry, const char *name,
// 			    const void *value, size_t size, int flags)
// {
// 	 return 1;
// }
// 
// void audit_security_inode_post_setxattr(struct dentry *dentry, const char *name, const void *value, size_t size, int flags)
// {
// 	 return;
// }
// 
// int audit_security_inode_getxattr(struct dentry *dentry, const char *name)
// {
// 	 return 1;
// }
// 
// 
// int audit_security_inode_listxattr(struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// 
// int audit_security_inode_removexattr(struct dentry *dentry, const char *name)
// {
// 	 return 1;
// }
// 
// 
// int audit_security_inode_need_killpriv(struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_killpriv(struct dentry *dentry)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_getsecurity(const struct inode *inode, const char *name, void **buffer, bool alloc)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_setsecurity(struct inode *inode, const char *name, const void *value, size_t size, int flags)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_listsecurity(struct inode *inode, char *buffer, size_t buffer_size)
// {
// 	 return 1;
// }
// 
// void audit_security_inode_getsecid(const struct inode *inode, u32 *secid)
// {
// 	 return;
// }
// 
// int audit_security_file_permission(struct file *file, int mask)
// {
// 	 return 1;
// }
// 
// 
// int audit_security_file_alloc(struct file *file)
// {
// 	 return 1;
// }
// 
// void audit_security_file_free(struct file *file)
// {
// 	 return;
// }
// 
// int audit_security_file_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
// {
// 	 return 1;
// }
// 
// int audit_security_file_mmap(struct file *file, unsigned long reqprot, unsigned long prot, unsigned long flags, unsigned long addr, unsigned long addr_only)
// {
// 	 return 1;
// }
// 
// int audit_security_file_mprotect(struct vm_area_struct *vma, unsigned long reqprot, unsigned long prot)
// {
// 	 return 1;
// }
// 
// int audit_security_file_lock(struct file *file, unsigned int cmd)
// {
// 	 return 1;
// }
// 
// int audit_security_file_fcntl(struct file *file, unsigned int cmd, unsigned long arg)
// {
// 	 return 1;
// }
// 
// int audit_security_file_set_fowner(struct file *file)
// {
// 	 return 1;
// }
// 
// int audit_security_file_send_sigiotask(struct task_struct *tsk, struct fown_struct *fown, int sig)
// {
// 	 return 1;
// }
// 
// int audit_security_file_receive(struct file *file)
// {
// 	 return 1;
// }
// 
// int audit_security_dentry_open(struct file *file, const struct cred *cred)
// {
// 	 return 1;
// }
// 
// int audit_security_task_create(unsigned long clone_flags)
// {
// 	 return 1;
// }
// 
// int audit_security_cred_alloc_blank(struct cred *cred, gfp_t gfp)
// {
// 	 return 1;
// }
// 
// void audit_security_cred_free(struct cred *cred)
// {
// 	 return;
// }
// 
// int audit_security_prepare_creds(struct cred *new, const struct cred *old, gfp_t gfp)
// {
// 	 return 1;
// }
// 
// void audit_security_commit_creds(struct cred *new, const struct cred *old)
// {
// 	 return;
// }
// 
// void audit_security_transfer_creds(struct cred *new, const struct cred *old)
// {
// 	 return;
// }
// 
// int audit_security_kernel_act_as(struct cred *new, u32 secid)
// {
// 	 return 1;
// }
// 
// int audit_security_kernel_create_files_as(struct cred *new, struct inode *inode)
// {
// 	 return 1;
// }
// 
// int audit_security_kernel_module_request(void)
// {
// 	 return 1;
// }
// 
// int audit_security_task_setuid(uid_t id0, uid_t id1, uid_t id2, int flags)
// {
// 	 return 1;
// }
// 
// int audit_security_task_fix_setuid(struct cred *new, const struct cred *old, int flags)
// {
// 	 return 1;
// }
// 
// int audit_security_task_setgid(gid_t id0, gid_t id1, gid_t id2, int flags)
// {
// 	 return 1;
// }
// 
// int audit_security_task_setpgid(struct task_struct *p, pid_t pgid)
// {
// 	 return 1;
// }
// 
// int audit_security_task_getpgid(struct task_struct *p)
// {
// 	 return 1;
// }
// 
// int audit_security_task_getsid(struct task_struct *p)
// {
// 	 return 1;
// }
// 
// void audit_security_task_getsecid(struct task_struct *p, u32 *secid)
// {
// 	 return;
// }
// 
// int audit_security_task_setgroups(struct group_info *group_info)
// {
// 	 return 1;
// }
// 
// int audit_security_task_setnice(struct task_struct *p, int nice)
// {
// 	 return 1;
// }
// 
// int audit_security_task_setioprio(struct task_struct *p, int ioprio)
// {
// 	 return 1;
// }
// 
// int audit_security_task_getioprio(struct task_struct *p)
// {
// 	 return 1;
// }
// 
// int audit_security_task_setrlimit(unsigned int resource, struct rlimit *new_rlim)
// {
// 	 return 1;
// }
// 
// int audit_security_task_setscheduler(struct task_struct *p, int policy, struct sched_param *lp)
// {
// 	 return 1;
// }
// 
// int audit_security_task_getscheduler(struct task_struct *p)
// {
// 	 return 1;
// }
// 
// int audit_security_task_movememory(struct task_struct *p)
// {
// 	 return 1;
// }
// 
// int audit_security_task_kill(struct task_struct *p, struct siginfo *info, int sig, u32 secid)
// {
// 	 return 1;
// }
// 
// int audit_security_task_wait(struct task_struct *p)
// {
// 	 return 1;
// }
// 
// int audit_security_task_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
// {
// 	 return 1;
// }
// 
// void audit_security_task_to_inode(struct task_struct *p, struct inode *inode)
// {
// 	 return;
// }
// 
// int audit_security_ipc_permission(struct kern_ipc_perm *ipcp, short flag)
// {
// 	 return 1;
// }
// 
// void audit_security_ipc_getsecid(struct kern_ipc_perm *ipcp, u32 *secid)
// {
// 	 return;
// }
// 
// int audit_security_msg_msg_alloc(struct msg_msg *msg)
// {
// 	 return 1;
// }
// 
// void audit_security_msg_msg_free(struct msg_msg *msg)
// {
// 	 return;
// }
// 
// int audit_security_msg_queue_alloc(struct msg_queue *msq)
// {
// 	 return 1;
// }
// 
// void audit_security_msg_queue_free(struct msg_queue *msq)
// {
// 	 return;
// }
// 
// int audit_security_msg_queue_associate(struct msg_queue *msq, int msqflg)
// {
// 	 return 1;
// }
// 
// int audit_security_msg_queue_msgctl(struct msg_queue *msq, int cmd)
// {
// 	 return 1;
// }
// 
// int audit_security_msg_queue_msgsnd(struct msg_queue *msq, struct msg_msg *msg, int msqflg)
// {
// 	 return 1;
// }
// 
// int audit_security_msg_queue_msgrcv(struct msg_queue *msq, struct msg_msg *msg, struct task_struct *target, long type, int mode)
// {
// 	 return 1;
// }
// 
// int audit_security_shm_alloc(struct shmid_kernel *shp)
// {
// 	 return 1;
// }
// 
// void audit_security_shm_free(struct shmid_kernel *shp)
// {
// 	 return;
// }
// 
// int audit_security_shm_associate(struct shmid_kernel *shp, int shmflg)
// {
// 	 return 1;
// }
// 
// int audit_security_shm_shmctl(struct shmid_kernel *shp, int cmd)
// {
// 	 return 1;
// }
// 
// int audit_security_shm_shmat(struct shmid_kernel *shp, char __user *shmaddr, int shmflg)
// {
// 	 return 1;
// }
// 
// int audit_security_sem_alloc(struct sem_array *sma)
// {
// 	 return 1;
// }
// 
// void audit_security_sem_free(struct sem_array *sma)
// {
// 	 return;
// }
// 
// int audit_security_sem_associate(struct sem_array *sma, int semflg)
// {
// 	 return 1;
// }
// 
// int audit_security_sem_semctl(struct sem_array *sma, int cmd)
// {
// 	 return 1;
// }
// 
// int audit_security_sem_semop(struct sem_array *sma, struct sembuf *sops,unsigned nsops, int alter)
// {
// 	 return 1;
// }
// 
// void audit_security_d_instantiate(struct dentry *dentry, struct inode *inode)
// {
// 	 return;
// }
// 
// int audit_security_getprocattr(struct task_struct *p, char *name, char **value)
// {
// 	 return 1;
// }
// 
// int audit_security_setprocattr(struct task_struct *p, char *name, void *value, size_t size)
// {
// 	 return 1;
// }
// 
// int audit_security_netlink_send(struct sock *sk, struct sk_buff *skb)
// {
// 	 return 1;
// }
// 
// int audit_security_netlink_recv(struct sk_buff *skb, int cap)
// {
// 	 return 1;
// }
// 
// int audit_security_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
// {
// 	 return 1;
// }
// 
// int audit_security_secctx_to_secid(const char *secdata, u32 seclen, u32 *secid)
// {
// 	 return 1;
// }
// 
// void audit_security_release_secctx(char *secdata, u32 seclen)
// {
// 	 return;
// }
// 
// int audit_security_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_setsecctx(struct dentry *dentry, void *ctx, u32 ctxlen)
// {
// 	 return 1;
// }
// 
// int audit_security_inode_getsecctx(struct inode *inode, void **ctx, u32 *ctxlen)
// {
// 	 return 1;
// }
// 
// static int audit_security_sb_alloc_security(struct super_block *sb)
// {
// 	return 1;
// }
// 
// static void audit_security_sb_free_security(struct super_block *sb)
// {
// 	return;
// }
// 
// static int audit_security_sb_copy_data(char *orig, char *copy)
// {
// 	return 1;
// }
// 
// static int audit_security_sb_kern_mount(struct super_block *sb, int flags, void *data)
// {
// 	return 1;
// }
// 
// static int audit_security_sb_statfs(struct dentry *dentry)
// {
// 	return 1;
// }
// 
// static int audit_security_mount(char *dev_name, struct path *path, char *type, unsigned long flags, void *data)
// {
// 	return 1;	
// }
// 
// static int audit_security_umount(struct vfsmount *mnt, int flags)
// {
// 	return 1;
// }
// 
// static int audit_security_inode_alloc_security(struct inode *inode)
// {
// 	return 1;
// }
// 
// static int audit_security_sb_show_options(struct seq_file *m, struct super_block *sb)
// {
// 	return 1;
// }
// 
// static int audit_security_set_mnt_opts(struct super_block *sb,	struct security_mnt_opts *opts)
// {
// 	return 1;
// }
// 
// static void audit_security_sb_clone_mnt_opts(const struct super_block *oldsb, struct super_block *newsb)
// {
// 	return;
// }
// 
// static int selinux_parse_opts_str(char *options, struct security_mnt_opts *opts)
// {
// 	return 1;
// }
// 
// static int audit_security_inode_alloc_security(struct inode *inode)
// {
// 	return 1;
// }
// 
// static void audit_security_inode_free_security(struct inode *inode)
// {
// 	return 1;
// }
// 
// static int audit_security_file_alloc_security(struct file *file)
// {
// 	return 1;
// }
// 
// static void audit_security_file_free_security(struct file *file)
// {
// 	file_free_security(file);
// }

static int security_sb_alloc_security(struct super_block *sb)
{
	return 1;
}

static void security_sb_free_security(struct super_block *sb)
{
	return;
}

static int security_parse_opts_str(char *options, struct security_mnt_opts *opts)
{
	return 1;
}

static int security_mount(char *dev_name, struct path *path, char *type, unsigned long flags, void *data)
{
	return 1;
}

static int security_umount(struct vfsmount *mnt, int flags)
{
	return 1;
}

static int security_set_mnt_opts(struct super_block *sb, struct security_mnt_opts *opts)
{
	return 1;
}

static int security_inode_alloc_security(struct inode *inode)
{
	return 1;
}

static void security_inode_free_security(struct inode *inode)
{
	return;
}

static int security_file_alloc_security(struct file *file)
{
	return 1;
}

static void security_file_free_security(struct file *file)
{
	return;
}

static int security_cred_prepare(struct cred *new, const struct cred *old, gfp_t gfp)
{
	return 1;
}

static void security_cred_transfer(struct cred *new, const struct cred *old)
{
	return;
}

static int security_msg_msg_alloc_security(struct msg_msg *msg)
{
	return 1;
}

static void security_msg_msg_free_security(struct msg_msg *msg)
{
	return;
}

static int security_msg_queue_alloc_security(struct msg_queue *msq)
{
	return 1;
}

static void security_msg_queue_free_security(struct msg_queue *msq)
{
	return;
}

static int security_shm_alloc_security(struct shmid_kernel *shp)
{
	return 1;
}

static void security_shm_free_security(struct shmid_kernel *shp)
{
	return;
}

static int security_sem_alloc_security(struct sem_array *sma)
{
	return 1;
}

static void security_sem_free_security(struct sem_array *sma)
{
	return;
}

static int security_socket_unix_stream_connect(struct socket *sock, struct socket *other, struct sock *newsk)
{
	return 1;
}

static int security_socket_unix_may_send(struct socket *sock, struct socket *other)
{
	return 1;
}

static int security_socket_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	return 1;
}

static void security_sk_clone_security(const struct sock *sk, struct sock *newsk)
{
	return;
}

static void security_sk_getsecid(struct sock *sk, u32 *secid)
{
	return;
}

static int security_sk_alloc_security(struct sock *sk, int family, gfp_t priority)
{
	return 1;
}

static void security_sk_free_security(struct sock *sk)
{
	return;
}

static struct security_operations audit_ops = {
	.name =				"audit_ops",

	.ptrace_access_check =		security_ptrace_access_check,
	.ptrace_traceme =		security_ptrace_traceme,
	.capget =			security_capget,
	.capset =			security_capset,
	.sysctl =			security_sysctl,
	.capable =			security_capable,
	.quotactl =			security_quotactl,
	.quota_on =			security_quota_on,
	.syslog =			security_syslog,
	.vm_enough_memory =		security_vm_enough_memory,

	.netlink_send =			security_netlink_send,
	.netlink_recv =			security_netlink_recv,

	.bprm_set_creds =		security_bprm_set_creds,
	.bprm_committing_creds =	security_bprm_committing_creds,
	.bprm_committed_creds =		security_bprm_committed_creds,
	.bprm_secureexec =		security_bprm_secureexec,

	.sb_alloc_security =		security_sb_alloc_security,
	.sb_free_security =		security_sb_free_security,
	.sb_copy_data =			security_sb_copy_data,
	.sb_kern_mount =		security_sb_kern_mount,
	.sb_show_options =		security_sb_show_options,
	.sb_statfs =			security_sb_statfs,
	.sb_mount =			security_mount,
	.sb_umount =			security_umount,
	.sb_set_mnt_opts =		security_set_mnt_opts,
	.sb_clone_mnt_opts =		security_sb_clone_mnt_opts,
	.sb_parse_opts_str = 		security_parse_opts_str,

	.inode_alloc_security =		security_inode_alloc_security,
	.inode_free_security =		security_inode_free_security,
	.inode_init_security =		security_inode_init_security,
	.inode_create =			security_inode_create,
	.inode_link =			security_inode_link,
	.inode_unlink =			security_inode_unlink,
	.inode_symlink =		security_inode_symlink,
	.inode_mkdir =			security_inode_mkdir,
	.inode_rmdir =			security_inode_rmdir,
	.inode_mknod =			security_inode_mknod,
	.inode_rename =			security_inode_rename,
	.inode_readlink =		security_inode_readlink,
	.inode_follow_link =		security_inode_follow_link,
	.inode_permission =		security_inode_permission,
	.inode_setattr =		security_inode_setattr,
	.inode_getattr =		security_inode_getattr,
	.inode_setxattr =		security_inode_setxattr,
	.inode_post_setxattr =		security_inode_post_setxattr,
	.inode_getxattr =		security_inode_getxattr,
	.inode_listxattr =		security_inode_listxattr,
	.inode_removexattr =		security_inode_removexattr,
	.inode_getsecurity =		security_inode_getsecurity,
	.inode_setsecurity =		security_inode_setsecurity,
	.inode_listsecurity =		security_inode_listsecurity,
	.inode_getsecid =		security_inode_getsecid,

	.file_permission =		security_file_permission,
	.file_alloc_security =		security_file_alloc_security,
	.file_free_security =		security_file_free_security,
	.file_ioctl =			security_file_ioctl,
	.file_mmap =			security_file_mmap,
	.file_mprotect =		security_file_mprotect,
	.file_lock =			security_file_lock,
	.file_fcntl =			security_file_fcntl,
	.file_set_fowner =		security_file_set_fowner,
	.file_send_sigiotask =		security_file_send_sigiotask,
	.file_receive =			security_file_receive,

	.dentry_open =			security_dentry_open,

	.task_create =			security_task_create,
	.cred_alloc_blank =		security_cred_alloc_blank,
	.cred_free =			security_cred_free,
	.cred_prepare =			security_cred_prepare,
	.cred_transfer =		security_cred_transfer,
	.kernel_act_as =		security_kernel_act_as,
	.kernel_create_files_as =	security_kernel_create_files_as,
	.kernel_module_request =	security_kernel_module_request,
	.task_setpgid =			security_task_setpgid,
	.task_getpgid =			security_task_getpgid,
	.task_getsid =			security_task_getsid,
	.task_getsecid =		security_task_getsecid,
	.task_setnice =			security_task_setnice,
	.task_setioprio =		security_task_setioprio,
	.task_getioprio =		security_task_getioprio,
	.task_setrlimit =		security_task_setrlimit,
	.task_setscheduler =		security_task_setscheduler,
	.task_getscheduler =		security_task_getscheduler,
	.task_movememory =		security_task_movememory,
	.task_kill =			security_task_kill,
	.task_wait =			security_task_wait,
	.task_to_inode =		security_task_to_inode,

	.ipc_permission =		security_ipc_permission,
	.ipc_getsecid =			security_ipc_getsecid,

	.msg_msg_alloc_security =	security_msg_msg_alloc_security,
	.msg_msg_free_security =	security_msg_msg_free_security,

	.msg_queue_alloc_security =	security_msg_queue_alloc_security,
	.msg_queue_free_security =	security_msg_queue_free_security,
	.msg_queue_associate =		security_msg_queue_associate,
	.msg_queue_msgctl =		security_msg_queue_msgctl,
	.msg_queue_msgsnd =		security_msg_queue_msgsnd,
	.msg_queue_msgrcv =		security_msg_queue_msgrcv,

	.shm_alloc_security =		security_shm_alloc_security,
	.shm_free_security =		security_shm_free_security,
	.shm_associate =		security_shm_associate,
	.shm_shmctl =			security_shm_shmctl,
	.shm_shmat =			security_shm_shmat,

	.sem_alloc_security =		security_sem_alloc_security,
	.sem_free_security =		security_sem_free_security,
	.sem_associate =		security_sem_associate,
	.sem_semctl =			security_sem_semctl,
	.sem_semop =			security_sem_semop,

	.d_instantiate =		security_d_instantiate,

	.getprocattr =			security_getprocattr,
	.setprocattr =			security_setprocattr,

	.secid_to_secctx =		security_secid_to_secctx,
	.secctx_to_secid =		security_secctx_to_secid,
	.release_secctx =		security_release_secctx,
	.inode_notifysecctx =		security_inode_notifysecctx,
	.inode_setsecctx =		security_inode_setsecctx,
	.inode_getsecctx =		security_inode_getsecctx,

	.unix_stream_connect =		security_socket_unix_stream_connect,
	.unix_may_send =		security_socket_unix_may_send,

	.socket_create =		security_socket_create,
	.socket_post_create =		security_socket_post_create,
	.socket_bind =			security_socket_bind,
	.socket_connect =		security_socket_connect,
	.socket_listen =		security_socket_listen,
	.socket_accept =		security_socket_accept,
	.socket_sendmsg =		security_socket_sendmsg,
	.socket_recvmsg =		security_socket_recvmsg,
	.socket_getsockname =		security_socket_getsockname,
	.socket_getpeername =		security_socket_getpeername,
	.socket_getsockopt =		security_socket_getsockopt,
	.socket_setsockopt =		security_socket_setsockopt,
	.socket_shutdown =		security_socket_shutdown,
	.socket_sock_rcv_skb =		security_socket_sock_rcv_skb,
	.socket_getpeersec_stream =	security_socket_getpeersec_stream,
	.socket_getpeersec_dgram =	security_socket_getpeersec_dgram,
	.sk_alloc_security =		security_sk_alloc_security,
	.sk_free_security =		security_sk_free_security,
	.sk_clone_security =		security_sk_clone_security,
	.sk_getsecid =			security_sk_getsecid,
	.sock_graft =			security_sock_graft,
	.inet_conn_request =		security_inet_conn_request,
	.inet_csk_clone =		security_inet_csk_clone,
	.inet_conn_established =	security_inet_conn_established,
	.req_classify_flow =		security_req_classify_flow,
	.tun_dev_create =		security_tun_dev_create,
	.tun_dev_post_create = 		security_tun_dev_post_create,
	.tun_dev_attach =		security_tun_dev_attach,

#ifdef CONFIG_SECURITY_NETWORK_XFRM
	.xfrm_policy_alloc_security =	security_xfrm_policy_alloc,
	.xfrm_policy_clone_security =	security_xfrm_policy_clone,
	.xfrm_policy_free_security =	security_xfrm_policy_free,
	.xfrm_policy_delete_security =	security_xfrm_policy_delete,
	.xfrm_state_alloc_security =	security_xfrm_state_alloc,
	.xfrm_state_free_security =	security_xfrm_state_free,
	.xfrm_state_delete_security =	security_xfrm_state_delete,
	.xfrm_policy_lookup =		security_xfrm_policy_lookup,
	.xfrm_state_pol_flow_match =	security_xfrm_state_pol_flow_match,
	.xfrm_decode_session =		security_xfrm_decode_session,
#endif

#ifdef CONFIG_KEYS
	.key_alloc =			security_key_alloc,
	.key_free =			security_key_free,
	.key_permission =		security_key_permission,
	.key_getsecurity =		security_key_getsecurity,
#endif

#ifdef CONFIG_AUDIT
	.audit_rule_init =		security_audit_rule_init,
	.audit_rule_known =		security_audit_rule_known,
	.audit_rule_match =		security_audit_rule_match,
	.audit_rule_free =		security_audit_rule_free,
#endif
};

static __init int audit_security_init(void)
{
	if (!security_module_enable(&audit_ops)) {
		printk(KERN_INFO "Audit Security:  Abort initialization.\n");
		return 0;
	}

	printk(KERN_INFO "Audit Security:  Initializing.\n");

	/* Set the security state for the initial task. */
//	cred_init_security();

	//sel_inode_cache = kmem_cache_create("selinux_inode_security", sizeof(struct inode_security_struct), 0, SLAB_PANIC, NULL);
	//avc_init();

//	secondary_ops = security_ops;
//	if (!secondary_ops)
//		panic("Audit Security: No initial security operations\n");
//	secondary_ops = security_ops;
//	if (!secondary_ops)
//		panic("Audit Security: No initial security operations\n");
	if (register_security(&audit_ops))
		panic("Audit Security: Unable to register with kernel.\n");

	return 0;
}

security_initcall(audit_security_init);
