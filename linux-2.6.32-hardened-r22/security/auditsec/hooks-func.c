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
#include <linux/nfs_mount.h>
#include <net/ipv6.h>
#include <linux/hugetlb.h>
#include <linux/personality.h>
#include <linux/sysctl.h>
#include <linux/audit.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/posix-timers.h>
#include <linux/limits.h>

#include "hooks-func.h"


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
			} else {
				return -1;
			}
		} else {
			return -1;
		}
	}
} 

int file_path(struct file *file, char *path) 
{
	char * mnt_point = file->f_path.mnt->mnt_devname;
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

void pid_branch (struct task_struct *tsk) 
{
	pid_t *branch = NULL;
	pid_t *tmp = NULL;
	int n = 0, i = 0;

	while(*tsk != NULL) {
		tmp = branch;
		branch = (pid_t *) vmalloc((n+1)*sizeof(pid_t));
		for(i=0; i<n; i++) branch[i] = tmp[i];
		vfree(tmp);
		branch[n] = task_pid_nr(tsk);
		tsk = tsk->real_parent;
		n++;
	}
	tmp = branch;
	branch = (pid_t *) vmalloc((n+1)*sizeof(pid_t));
	for(i=0; i<n; i++) branch[i] = tmp[i];
	vfree(tmp);
	branch[n] = 0;

	*daemon_pid() = branch;
}


/*
int print_info_audit_file(struct inode *dir, struct dentry *dentry, int mask, char *hook_name)
{
	//char * path_buf = vmalloc(sizeof(char) * PATH_MAX);
	char * path_buf = NULL;
	//if(path_buf == NULL){
	//	printk(KERN_INFO "USA: %s: %s (vmalloc failed)", hook_name, dentry->d_name.name);
	//	return 1;
	//}
	path_buf = dentry_path_(dentry);
	//dentry_path(dentry, path_buf, PATH_MAX);
	printk(KERN_INFO "USA: %s: %s, mask: %d", hook_name, path_buf, mask);
	vfree(path_buf);

	return 0;
}
*/


/*
 * ligne 1285 security/selinux/hooks.c : recuperer un contexte selinux
 *

char * get_context(void)
{
	int rc = 0;
	int len = INITCONTEXTLEN;
	char * context = kmalloc(len+1, GFP_NOFS);
	if (!context) {
		rc = -ENOMEM;
		return NULL;
	}
	context[len] = '\0';
	rc = inode->i_op->getxattr(dentry, XATTR_NAME_SELINUX,
				   context, len);
	if (rc == -ERANGE) {
		kfree(context);

		// Need a larger buffer.  Query for the right size. 
		rc = inode->i_op->getxattr(dentry, XATTR_NAME_SELINUX,
					   NULL, 0);
		if (rc < 0) {
			return NULL;
		}
		len = rc;
		context = kmalloc(len+1, GFP_NOFS);
		if (!context) {
			rc = -ENOMEM;
			return NULL;
		}
		context[len] = '\0';
		rc = inode->i_op->getxattr(dentry,
					   XATTR_NAME_SELINUX,
					   context, len);
	}
	return context;
}
*/
