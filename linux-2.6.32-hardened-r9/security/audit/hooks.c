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



static struct security_operations audit_ops = {
	.name =				"audit_ops",

	.ptrace_access_check =		ptrace_access_check,
	.ptrace_traceme =		ptrace_traceme,
	.capget =			capget,
	.capset =			capset,
	.sysctl =			sysctl,
	.capable =			capable,
	.quotactl =			quotactl,
	.quota_on =			quota_on,
	.syslog =			syslog,
	.vm_enough_memory =		vm_enough_memory,

	.netlink_send =			netlink_send,
	.netlink_recv =			netlink_recv,

	.bprm_set_creds =		bprm_set_creds,
	.bprm_committing_creds =	bprm_committing_creds,
	.bprm_committed_creds =		bprm_committed_creds,
	.bprm_secureexec =		bprm_secureexec,

	.sb_alloc_security =		sb_alloc_security,
	.sb_free_security =		sb_free_security,
	.sb_copy_data =			sb_copy_data,
	.sb_kern_mount =		sb_kern_mount,
	.sb_show_options =		sb_show_options,
	.sb_statfs =			sb_statfs,
	.sb_mount =			mount,
	.sb_umount =			umount,
	.sb_set_mnt_opts =		set_mnt_opts,
	.sb_clone_mnt_opts =		sb_clone_mnt_opts,
	.sb_parse_opts_str = 		parse_opts_str,

	.inode_alloc_security =		inode_alloc_security,
	.inode_free_security =		inode_free_security,
	.inode_init_security =		inode_init_security,
	.inode_create =			inode_create,
	.inode_link =			inode_link,
	.inode_unlink =			inode_unlink,
	.inode_symlink =		inode_symlink,
	.inode_mkdir =			inode_mkdir,
	.inode_rmdir =			inode_rmdir,
	.inode_mknod =			inode_mknod,
	.inode_rename =			inode_rename,
	.inode_readlink =		inode_readlink,
	.inode_follow_link =		inode_follow_link,
	.inode_permission =		inode_permission,
	.inode_setattr =		inode_setattr,
	.inode_getattr =		inode_getattr,
	.inode_setxattr =		inode_setxattr,
	.inode_post_setxattr =		inode_post_setxattr,
	.inode_getxattr =		inode_getxattr,
	.inode_listxattr =		inode_listxattr,
	.inode_removexattr =		inode_removexattr,
	.inode_getsecurity =		inode_getsecurity,
	.inode_setsecurity =		inode_setsecurity,
	.inode_listsecurity =		inode_listsecurity,
	.inode_getsecid =		inode_getsecid,

	.file_permission =		file_permission,
	.file_alloc_security =		file_alloc_security,
	.file_free_security =		file_free_security,
	.file_ioctl =			file_ioctl,
	.file_mmap =			file_mmap,
	.file_mprotect =		file_mprotect,
	.file_lock =			file_lock,
	.file_fcntl =			file_fcntl,
	.file_set_fowner =		file_set_fowner,
	.file_send_sigiotask =		file_send_sigiotask,
	.file_receive =			file_receive,

	.dentry_open =			dentry_open,

	.task_create =			task_create,
	.cred_alloc_blank =		cred_alloc_blank,
	.cred_free =			cred_free,
	.cred_prepare =			cred_prepare,
	.cred_transfer =		cred_transfer,
	.kernel_act_as =		kernel_act_as,
	.kernel_create_files_as =	kernel_create_files_as,
	.kernel_module_request =	kernel_module_request,
	.task_setpgid =			task_setpgid,
	.task_getpgid =			task_getpgid,
	.task_getsid =			task_getsid,
	.task_getsecid =		task_getsecid,
	.task_setnice =			task_setnice,
	.task_setioprio =		task_setioprio,
	.task_getioprio =		task_getioprio,
	.task_setrlimit =		task_setrlimit,
	.task_setscheduler =		task_setscheduler,
	.task_getscheduler =		task_getscheduler,
	.task_movememory =		task_movememory,
	.task_kill =			task_kill,
	.task_wait =			task_wait,
	.task_to_inode =		task_to_inode,

	.ipc_permission =		ipc_permission,
	.ipc_getsecid =			ipc_getsecid,

	.msg_msg_alloc_security =	msg_msg_alloc_security,
	.msg_msg_free_security =	msg_msg_free_security,

	.msg_queue_alloc_security =	msg_queue_alloc_security,
	.msg_queue_free_security =	msg_queue_free_security,
	.msg_queue_associate =		msg_queue_associate,
	.msg_queue_msgctl =		msg_queue_msgctl,
	.msg_queue_msgsnd =		msg_queue_msgsnd,
	.msg_queue_msgrcv =		msg_queue_msgrcv,

	.shm_alloc_security =		shm_alloc_security,
	.shm_free_security =		shm_free_security,
	.shm_associate =		shm_associate,
	.shm_shmctl =			shm_shmctl,
	.shm_shmat =			shm_shmat,

	.sem_alloc_security =		sem_alloc_security,
	.sem_free_security =		sem_free_security,
	.sem_associate =		sem_associate,
	.sem_semctl =			sem_semctl,
	.sem_semop =			sem_semop,

	.d_instantiate =		d_instantiate,

	.getprocattr =			getprocattr,
	.setprocattr =			setprocattr,

	.secid_to_secctx =		secid_to_secctx,
	.secctx_to_secid =		secctx_to_secid,
	.release_secctx =		release_secctx,
	.inode_notifysecctx =		inode_notifysecctx,
	.inode_setsecctx =		inode_setsecctx,
	.inode_getsecctx =		inode_getsecctx,

	.unix_stream_connect =		socket_unix_stream_connect,
	.unix_may_send =		socket_unix_may_send,

	.socket_create =		socket_create,
	.socket_post_create =		socket_post_create,
	.socket_bind =			socket_bind,
	.socket_connect =		socket_connect,
	.socket_listen =		socket_listen,
	.socket_accept =		socket_accept,
	.socket_sendmsg =		socket_sendmsg,
	.socket_recvmsg =		socket_recvmsg,
	.socket_getsockname =		socket_getsockname,
	.socket_getpeername =		socket_getpeername,
	.socket_getsockopt =		socket_getsockopt,
	.socket_setsockopt =		socket_setsockopt,
	.socket_shutdown =		socket_shutdown,
	.socket_sock_rcv_skb =		socket_sock_rcv_skb,
	.socket_getpeersec_stream =	socket_getpeersec_stream,
	.socket_getpeersec_dgram =	socket_getpeersec_dgram,
	.sk_alloc_security =		sk_alloc_security,
	.sk_free_security =		sk_free_security,
	.sk_clone_security =		sk_clone_security,
	.sk_getsecid =			sk_getsecid,
	.sock_graft =			sock_graft,
	.inet_conn_request =		inet_conn_request,
	.inet_csk_clone =		inet_csk_clone,
	.inet_conn_established =	inet_conn_established,
	.req_classify_flow =		req_classify_flow,
	.tun_dev_create =		tun_dev_create,
	.tun_dev_post_create = 		tun_dev_post_create,
	.tun_dev_attach =		tun_dev_attach,

#ifdef CONFIG_SECURITY_NETWORK_XFRM
	.xfrm_policy_alloc_security =	xfrm_policy_alloc,
	.xfrm_policy_clone_security =	xfrm_policy_clone,
	.xfrm_policy_free_security =	xfrm_policy_free,
	.xfrm_policy_delete_security =	xfrm_policy_delete,
	.xfrm_state_alloc_security =	xfrm_state_alloc,
	.xfrm_state_free_security =	xfrm_state_free,
	.xfrm_state_delete_security =	xfrm_state_delete,
	.xfrm_policy_lookup =		xfrm_policy_lookup,
	.xfrm_state_pol_flow_match =	xfrm_state_pol_flow_match,
	.xfrm_decode_session =		xfrm_decode_session,
#endif

#ifdef CONFIG_KEYS
	.key_alloc =			key_alloc,
	.key_free =			key_free,
	.key_permission =		key_permission,
	.key_getsecurity =		key_getsecurity,
#endif

#ifdef CONFIG_AUDIT
	.audit_rule_init =		audit_rule_init,
	.audit_rule_known =		audit_rule_known,
	.audit_rule_match =		audit_rule_match,
	.audit_rule_free =		audit_rule_free,
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
