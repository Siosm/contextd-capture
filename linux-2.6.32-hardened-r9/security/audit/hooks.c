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

//	.ptrace_access_check =		selinux_ptrace_access_check,
//	.ptrace_traceme =		selinux_ptrace_traceme,
//	.capget =			selinux_capget,
//	.capset =			selinux_capset,
//	.sysctl =			selinux_sysctl,
//	.capable =			selinux_capable,
//	.quotactl =			selinux_quotactl,
//	.quota_on =			selinux_quota_on,
//	.syslog =			selinux_syslog,
//	.vm_enough_memory =		selinux_vm_enough_memory,
//
//	.netlink_send =			selinux_netlink_send,
//	.netlink_recv =			selinux_netlink_recv,
//
//	.bprm_set_creds =		selinux_bprm_set_creds,
//	.bprm_committing_creds =	selinux_bprm_committing_creds,
//	.bprm_committed_creds =		selinux_bprm_committed_creds,
//	.bprm_secureexec =		selinux_bprm_secureexec,
//
//	.sb_alloc_security =		selinux_sb_alloc_security,
//	.sb_free_security =		selinux_sb_free_security,
//	.sb_copy_data =			selinux_sb_copy_data,
//	.sb_kern_mount =		selinux_sb_kern_mount,
//	.sb_show_options =		selinux_sb_show_options,
//	.sb_statfs =			selinux_sb_statfs,
//	.sb_mount =			selinux_mount,
//	.sb_umount =			selinux_umount,
//	.sb_set_mnt_opts =		selinux_set_mnt_opts,
//	.sb_clone_mnt_opts =		selinux_sb_clone_mnt_opts,
//	.sb_parse_opts_str = 		selinux_parse_opts_str,
//
//
//	.inode_alloc_security =		selinux_inode_alloc_security,
//	.inode_free_security =		selinux_inode_free_security,
//	.inode_init_security =		selinux_inode_init_security,
//	.inode_create =			selinux_inode_create,
//	.inode_link =			selinux_inode_link,
//	.inode_unlink =			selinux_inode_unlink,
//	.inode_symlink =		selinux_inode_symlink,
//	.inode_mkdir =			selinux_inode_mkdir,
//	.inode_rmdir =			selinux_inode_rmdir,
//	.inode_mknod =			selinux_inode_mknod,
//	.inode_rename =			selinux_inode_rename,
//	.inode_readlink =		selinux_inode_readlink,
//	.inode_follow_link =		selinux_inode_follow_link,
//	.inode_permission =		selinux_inode_permission,
//	.inode_setattr =		selinux_inode_setattr,
//	.inode_getattr =		selinux_inode_getattr,
//	.inode_setxattr =		selinux_inode_setxattr,
//	.inode_post_setxattr =		selinux_inode_post_setxattr,
//	.inode_getxattr =		selinux_inode_getxattr,
//	.inode_listxattr =		selinux_inode_listxattr,
//	.inode_removexattr =		selinux_inode_removexattr,
//	.inode_getsecurity =		selinux_inode_getsecurity,
//	.inode_setsecurity =		selinux_inode_setsecurity,
//	.inode_listsecurity =		selinux_inode_listsecurity,
//	.inode_getsecid =		selinux_inode_getsecid,
//
//	.file_permission =		selinux_file_permission,
//	.file_alloc_security =		selinux_file_alloc_security,
//	.file_free_security =		selinux_file_free_security,
//	.file_ioctl =			selinux_file_ioctl,
//	.file_mmap =			selinux_file_mmap,
//	.file_mprotect =		selinux_file_mprotect,
//	.file_lock =			selinux_file_lock,
//	.file_fcntl =			selinux_file_fcntl,
//	.file_set_fowner =		selinux_file_set_fowner,
//	.file_send_sigiotask =		selinux_file_send_sigiotask,
//	.file_receive =			selinux_file_receive,
//
//	.dentry_open =			selinux_dentry_open,
//
//	.task_create =			selinux_task_create,
//	.cred_alloc_blank =		selinux_cred_alloc_blank,
//	.cred_free =			selinux_cred_free,
//	.cred_prepare =			selinux_cred_prepare,
//	.cred_transfer =		selinux_cred_transfer,
//	.kernel_act_as =		selinux_kernel_act_as,
//	.kernel_create_files_as =	selinux_kernel_create_files_as,
//	.kernel_module_request =	selinux_kernel_module_request,
//	.task_setpgid =			selinux_task_setpgid,
//	.task_getpgid =			selinux_task_getpgid,
//	.task_getsid =			selinux_task_getsid,
//	.task_getsecid =		selinux_task_getsecid,
//	.task_setnice =			selinux_task_setnice,
//	.task_setioprio =		selinux_task_setioprio,
//	.task_getioprio =		selinux_task_getioprio,
//	.task_setrlimit =		selinux_task_setrlimit,
//	.task_setscheduler =		selinux_task_setscheduler,
//	.task_getscheduler =		selinux_task_getscheduler,
//	.task_movememory =		selinux_task_movememory,
//	.task_kill =			selinux_task_kill,
//	.task_wait =			selinux_task_wait,
//	.task_to_inode =		selinux_task_to_inode,
//
//	.ipc_permission =		selinux_ipc_permission,
//	.ipc_getsecid =			selinux_ipc_getsecid,
//
//	.msg_msg_alloc_security =	selinux_msg_msg_alloc_security,
//	.msg_msg_free_security =	selinux_msg_msg_free_security,
//
//	.msg_queue_alloc_security =	selinux_msg_queue_alloc_security,
//	.msg_queue_free_security =	selinux_msg_queue_free_security,
//	.msg_queue_associate =		selinux_msg_queue_associate,
//	.msg_queue_msgctl =		selinux_msg_queue_msgctl,
//	.msg_queue_msgsnd =		selinux_msg_queue_msgsnd,
//	.msg_queue_msgrcv =		selinux_msg_queue_msgrcv,
//
//	.shm_alloc_security =		selinux_shm_alloc_security,
//	.shm_free_security =		selinux_shm_free_security,
//	.shm_associate =		selinux_shm_associate,
//	.shm_shmctl =			selinux_shm_shmctl,
//	.shm_shmat =			selinux_shm_shmat,
//
//	.sem_alloc_security =		selinux_sem_alloc_security,
//	.sem_free_security =		selinux_sem_free_security,
//	.sem_associate =		selinux_sem_associate,
//	.sem_semctl =			selinux_sem_semctl,
//	.sem_semop =			selinux_sem_semop,
//
//	.d_instantiate =		selinux_d_instantiate,
//
//	.getprocattr =			selinux_getprocattr,
//	.setprocattr =			selinux_setprocattr,
//
//	.secid_to_secctx =		selinux_secid_to_secctx,
//	.secctx_to_secid =		selinux_secctx_to_secid,
//	.release_secctx =		selinux_release_secctx,
//	.inode_notifysecctx =		selinux_inode_notifysecctx,
//	.inode_setsecctx =		selinux_inode_setsecctx,
//	.inode_getsecctx =		selinux_inode_getsecctx,
//
//	.unix_stream_connect =		selinux_socket_unix_stream_connect,
//	.unix_may_send =		selinux_socket_unix_may_send,
//
//	.socket_create =		selinux_socket_create,
//	.socket_post_create =		selinux_socket_post_create,
//	.socket_bind =			selinux_socket_bind,
//	.socket_connect =		selinux_socket_connect,
//	.socket_listen =		selinux_socket_listen,
//	.socket_accept =		selinux_socket_accept,
//	.socket_sendmsg =		selinux_socket_sendmsg,
//	.socket_recvmsg =		selinux_socket_recvmsg,
//	.socket_getsockname =		selinux_socket_getsockname,
//	.socket_getpeername =		selinux_socket_getpeername,
//	.socket_getsockopt =		selinux_socket_getsockopt,
//	.socket_setsockopt =		selinux_socket_setsockopt,
//	.socket_shutdown =		selinux_socket_shutdown,
//	.socket_sock_rcv_skb =		selinux_socket_sock_rcv_skb,
//	.socket_getpeersec_stream =	selinux_socket_getpeersec_stream,
//	.socket_getpeersec_dgram =	selinux_socket_getpeersec_dgram,
//	.sk_alloc_security =		selinux_sk_alloc_security,
//	.sk_free_security =		selinux_sk_free_security,
//	.sk_clone_security =		selinux_sk_clone_security,
//	.sk_getsecid =			selinux_sk_getsecid,
//	.sock_graft =			selinux_sock_graft,
//	.inet_conn_request =		selinux_inet_conn_request,
//	.inet_csk_clone =		selinux_inet_csk_clone,
//	.inet_conn_established =	selinux_inet_conn_established,
//	.req_classify_flow =		selinux_req_classify_flow,
//	.tun_dev_create =		selinux_tun_dev_create,
//	.tun_dev_post_create = 		selinux_tun_dev_post_create,
//	.tun_dev_attach =		selinux_tun_dev_attach,
//
// #ifdef CONFIG_SECURITY_NETWORK_XFRM
//	.xfrm_policy_alloc_security =	selinux_xfrm_policy_alloc,
//	.xfrm_policy_clone_security =	selinux_xfrm_policy_clone,
//	.xfrm_policy_free_security =	selinux_xfrm_policy_free,
//	.xfrm_policy_delete_security =	selinux_xfrm_policy_delete,
//	.xfrm_state_alloc_security =	selinux_xfrm_state_alloc,
//	.xfrm_state_free_security =	selinux_xfrm_state_free,
//	.xfrm_state_delete_security =	selinux_xfrm_state_delete,
//	.xfrm_policy_lookup =		selinux_xfrm_policy_lookup,
//	.xfrm_state_pol_flow_match =	selinux_xfrm_state_pol_flow_match,
//	.xfrm_decode_session =		selinux_xfrm_decode_session,
//#endif
//
//#ifdef CONFIG_KEYS
//	.key_alloc =			selinux_key_alloc,
//	.key_free =			selinux_key_free,
//	.key_permission =		selinux_key_permission,
//	.key_getsecurity =		selinux_key_getsecurity,
//#endif
//
//#ifdef CONFIG_AUDIT
//	.audit_rule_init =		selinux_audit_rule_init,
//	.audit_rule_known =		selinux_audit_rule_known,
//	.audit_rule_match =		selinux_audit_rule_match,
//	.audit_rule_free =		selinux_audit_rule_free,
//#endif
};

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
	if (register_security(&audit_ops))
		panic("Audit Security: Unable to register with kernel.\n");

	return 0;
}

security_initcall(audit_security_init);
