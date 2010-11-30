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

static struct security_operations audit_ops = {
	.name =				"audit_security"

//	.ptrace_access_check =		audit_security_ptrace_access_check,
//	.ptrace_traceme =		audit_security_ptrace_traceme,
//	.capget =			audit_security_capget,
//	.capset =			audit_security_capset,
//	.sysctl =			audit_security_sysctl,
//	.capable =			audit_security_capable,
//	.quotactl =			audit_security_quotactl,
//	.quota_on =			audit_security_quota_on,
//	.syslog =			audit_security_syslog,
//	.vm_enough_memory =		audit_security_vm_enough_memory,
//
//	.netlink_send =			audit_security_netlink_send,
//	.netlink_recv =			audit_security_netlink_recv,
//
//	.bprm_set_creds =		audit_security_bprm_set_creds,
//	.bprm_committing_creds =	audit_security_bprm_committing_creds,
//	.bprm_committed_creds =		audit_security_bprm_committed_creds,
//	.bprm_secureexec =		audit_security_bprm_secureexec,
//
//	.sb_alloc_security =		audit_security_sb_alloc_security,
//	.sb_free_security =		audit_security_sb_free_security,
//	.sb_copy_data =			audit_security_sb_copy_data,
//	.sb_kern_mount =		audit_security_sb_kern_mount,
//	.sb_show_options =		audit_security_sb_show_options,
//	.sb_statfs =			audit_security_sb_statfs,
//	.sb_mount =			audit_security_mount,
//	.sb_umount =			audit_security_umount,
//	.sb_set_mnt_opts =		audit_security_set_mnt_opts,
//	.sb_clone_mnt_opts =		audit_security_sb_clone_mnt_opts,
//	.sb_parse_opts_str = 		audit_security_parse_opts_str,
//
//
//	.inode_alloc_security =		audit_security_inode_alloc_security,
//	.inode_free_security =		audit_security_inode_free_security,
//	.inode_init_security =		audit_security_inode_init_security,
//	.inode_create =			audit_security_inode_create,
//	.inode_link =			audit_security_inode_link,
//	.inode_unlink =			audit_security_inode_unlink,
//	.inode_symlink =		audit_security_inode_symlink,
//	.inode_mkdir =			audit_security_inode_mkdir,
//	.inode_rmdir =			audit_security_inode_rmdir,
//	.inode_mknod =			audit_security_inode_mknod,
//	.inode_rename =			audit_security_inode_rename,
//	.inode_readlink =		audit_security_inode_readlink,
//	.inode_follow_link =		audit_security_inode_follow_link,
//	.inode_permission =		audit_security_inode_permission,
//	.inode_setattr =		audit_security_inode_setattr,
//	.inode_getattr =		audit_security_inode_getattr,
//	.inode_setxattr =		audit_security_inode_setxattr,
//	.inode_post_setxattr =		audit_security_inode_post_setxattr,
//	.inode_getxattr =		audit_security_inode_getxattr,
//	.inode_listxattr =		audit_security_inode_listxattr,
//	.inode_removexattr =		audit_security_inode_removexattr,
//	.inode_getsecurity =		audit_security_inode_getsecurity,
//	.inode_setsecurity =		audit_security_inode_setsecurity,
//	.inode_listsecurity =		audit_security_inode_listsecurity,
//	.inode_getsecid =		audit_security_inode_getsecid,
//
//	.file_permission =		audit_security_file_permission,
//	.file_alloc_security =		audit_security_file_alloc_security,
//	.file_free_security =		audit_security_file_free_security,
//	.file_ioctl =			audit_security_file_ioctl,
//	.file_mmap =			audit_security_file_mmap,
//	.file_mprotect =		audit_security_file_mprotect,
//	.file_lock =			audit_security_file_lock,
//	.file_fcntl =			audit_security_file_fcntl,
//	.file_set_fowner =		audit_security_file_set_fowner,
//	.file_send_sigiotask =		audit_security_file_send_sigiotask,
//	.file_receive =			audit_security_file_receive,
//
//	.dentry_open =			audit_security_dentry_open,
//
//	.task_create =			audit_security_task_create,
//	.cred_alloc_blank =		audit_security_cred_alloc_blank,
//	.cred_free =			audit_security_cred_free,
//	.cred_prepare =			audit_security_cred_prepare,
//	.cred_transfer =		audit_security_cred_transfer,
//	.kernel_act_as =		audit_security_kernel_act_as,
//	.kernel_create_files_as =	audit_security_kernel_create_files_as,
//	.kernel_module_request =	audit_security_kernel_module_request,
//	.task_setpgid =			audit_security_task_setpgid,
//	.task_getpgid =			audit_security_task_getpgid,
//	.task_getsid =			audit_security_task_getsid,
//	.task_getsecid =		audit_security_task_getsecid,
//	.task_setnice =			audit_security_task_setnice,
//	.task_setioprio =		audit_security_task_setioprio,
//	.task_getioprio =		audit_security_task_getioprio,
//	.task_setrlimit =		audit_security_task_setrlimit,
//	.task_setscheduler =		audit_security_task_setscheduler,
//	.task_getscheduler =		audit_security_task_getscheduler,
//	.task_movememory =		audit_security_task_movememory,
//	.task_kill =			audit_security_task_kill,
//	.task_wait =			audit_security_task_wait,
//	.task_to_inode =		audit_security_task_to_inode,
//
//	.ipc_permission =		audit_security_ipc_permission,
//	.ipc_getsecid =			audit_security_ipc_getsecid,
//
//	.msg_msg_alloc_security =	audit_security_msg_msg_alloc_security,
//	.msg_msg_free_security =	audit_security_msg_msg_free_security,
//
//	.msg_queue_alloc_security =	audit_security_msg_queue_alloc_security,
//	.msg_queue_free_security =	audit_security_msg_queue_free_security,
//	.msg_queue_associate =		audit_security_msg_queue_associate,
//	.msg_queue_msgctl =		audit_security_msg_queue_msgctl,
//	.msg_queue_msgsnd =		audit_security_msg_queue_msgsnd,
//	.msg_queue_msgrcv =		audit_security_msg_queue_msgrcv,
//
//	.shm_alloc_security =		audit_security_shm_alloc_security,
//	.shm_free_security =		audit_security_shm_free_security,
//	.shm_associate =		audit_security_shm_associate,
//	.shm_shmctl =			audit_security_shm_shmctl,
//	.shm_shmat =			audit_security_shm_shmat,
//
//	.sem_alloc_security =		audit_security_sem_alloc_security,
//	.sem_free_security =		audit_security_sem_free_security,
//	.sem_associate =		audit_security_sem_associate,
//	.sem_semctl =			audit_security_sem_semctl,
//	.sem_semop =			audit_security_sem_semop,
//
//	.d_instantiate =		audit_security_d_instantiate,
//
//	.getprocattr =			audit_security_getprocattr,
//	.setprocattr =			audit_security_setprocattr,
//
//	.secid_to_secctx =		audit_security_secid_to_secctx,
//	.secctx_to_secid =		audit_security_secctx_to_secid,
//	.release_secctx =		audit_security_release_secctx,
//	.inode_notifysecctx =		audit_security_inode_notifysecctx,
//	.inode_setsecctx =		audit_security_inode_setsecctx,
//	.inode_getsecctx =		audit_security_inode_getsecctx,
//
//	.unix_stream_connect =		audit_security_socket_unix_stream_connect,
//	.unix_may_send =		audit_security_socket_unix_may_send,
//
//	.socket_create =		audit_security_socket_create,
//	.socket_post_create =		audit_security_socket_post_create,
//	.socket_bind =			audit_security_socket_bind,
//	.socket_connect =		audit_security_socket_connect,
//	.socket_listen =		audit_security_socket_listen,
//	.socket_accept =		audit_security_socket_accept,
//	.socket_sendmsg =		audit_security_socket_sendmsg,
//	.socket_recvmsg =		audit_security_socket_recvmsg,
//	.socket_getsockname =		audit_security_socket_getsockname,
//	.socket_getpeername =		audit_security_socket_getpeername,
//	.socket_getsockopt =		audit_security_socket_getsockopt,
//	.socket_setsockopt =		audit_security_socket_setsockopt,
//	.socket_shutdown =		audit_security_socket_shutdown,
//	.socket_sock_rcv_skb =		audit_security_socket_sock_rcv_skb,
//	.socket_getpeersec_stream =	audit_security_socket_getpeersec_stream,
//	.socket_getpeersec_dgram =	audit_security_socket_getpeersec_dgram,
//	.sk_alloc_security =		audit_security_sk_alloc_security,
//	.sk_free_security =		audit_security_sk_free_security,
//	.sk_clone_security =		audit_security_sk_clone_security,
//	.sk_getsecid =			audit_security_sk_getsecid,
//	.sock_graft =			audit_security_sock_graft,
//	.inet_conn_request =		audit_security_inet_conn_request,
//	.inet_csk_clone =		audit_security_inet_csk_clone,
//	.inet_conn_established =	audit_security_inet_conn_established,
//	.req_classify_flow =		audit_security_req_classify_flow,
//	.tun_dev_create =		audit_security_tun_dev_create,
//	.tun_dev_post_create = 		audit_security_tun_dev_post_create,
//	.tun_dev_attach =		audit_security_tun_dev_attach,
//
// #ifdef CONFIG_SECURITY_NETWORK_XFRM
//	.xfrm_policy_alloc_security =	audit_security_xfrm_policy_alloc,
//	.xfrm_policy_clone_security =	audit_security_xfrm_policy_clone,
//	.xfrm_policy_free_security =	audit_security_xfrm_policy_free,
//	.xfrm_policy_delete_security =	audit_security_xfrm_policy_delete,
//	.xfrm_state_alloc_security =	audit_security_xfrm_state_alloc,
//	.xfrm_state_free_security =	audit_security_xfrm_state_free,
//	.xfrm_state_delete_security =	audit_security_xfrm_state_delete,
//	.xfrm_policy_lookup =		audit_security_xfrm_policy_lookup,
//	.xfrm_state_pol_flow_match =	audit_security_xfrm_state_pol_flow_match,
//	.xfrm_decode_session =		audit_security_xfrm_decode_session,
//#endif
//
//#ifdef CONFIG_KEYS
//	.key_alloc =			audit_security_key_alloc,
//	.key_free =			audit_security_key_free,
//	.key_permission =		audit_security_key_permission,
//	.key_getsecurity =		audit_security_key_getsecurity,
//#endif
//
//#ifdef CONFIG_AUDIT
//	.audit_rule_init =		audit_security_audit_rule_init,
//	.audit_rule_known =		audit_security_audit_rule_known,
//	.audit_rule_match =		audit_security_audit_rule_match,
//	.audit_rule_free =		audit_security_audit_rule_free,
//#endif
};

static struct security_operations audit_ops;

static __init int audit_security_init(void)
{
	if (!security_module_enable(&audit_ops)) {
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
