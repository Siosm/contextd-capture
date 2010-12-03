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

static int security_sb_alloc_security(struct super_block *sb)
{
	return 0;
}

static void security_sb_free_security(struct super_block *sb)
{
	return;
}

static int security_parse_opts_str(char *options, struct security_mnt_opts *opts)
{
	return 0;
}

static int security_mount(char *dev_name, struct path *path, char *type, unsigned long flags, void *data)
{
	return 0;
}

static int security_umount(struct vfsmount *mnt, int flags)
{
	return 0;
}

static int security_set_mnt_opts(struct super_block *sb, struct security_mnt_opts *opts)
{
	return 0;
}

static int security_inode_alloc_security(struct inode *inode)
{
	return 0;
}

static void security_inode_free_security(struct inode *inode)
{
	return;
}

static int security_file_alloc_security(struct file *file)
{
	return 0;
}

static void security_file_free_security(struct file *file)
{
	return;
}

static int security_cred_prepare(struct cred *new, const struct cred *old, gfp_t gfp)
{
	return 0;
}

static void security_cred_transfer(struct cred *new, const struct cred *old)
{
	return;
}

static int security_msg_msg_alloc_security(struct msg_msg *msg)
{
	return 0;
}

static void security_msg_msg_free_security(struct msg_msg *msg)
{
	return;
}

static int security_msg_queue_alloc_security(struct msg_queue *msq)
{
	return 0;
}

static void security_msg_queue_free_security(struct msg_queue *msq)
{
	return;
}

static int security_shm_alloc_security(struct shmid_kernel *shp)
{
	return 0;
}

static void security_shm_free_security(struct shmid_kernel *shp)
{
	return;
}

static int security_sem_alloc_security(struct sem_array *sma)
{
	return 0;
}

static void security_sem_free_security(struct sem_array *sma)
{
	return;
}

static int security_socket_unix_stream_connect(struct socket *sock, struct socket *other, struct sock *newsk)
{
	return 0;
}

static int security_socket_unix_may_send(struct socket *sock, struct socket *other)
{
	return 0;
}

static int security_socket_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	return 0;
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
	return 0;
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
