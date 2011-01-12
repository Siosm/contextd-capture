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


int dentry_path_(struct dentry *dentry, char * fullpath)
{
	struct dentry *parent = dentry;
	char *path = NULL;
	char *path_tmp = NULL;
	int nb = 0, n = 0;

	while(!IS_ROOT(parent)){
		nb = strlen(parent->d_name.name);
		path_tmp = path;
		path = vmalloc(n + nb + 1);
		memcpy(path + nb + 1, path_tmp, n);
		memcpy(path + 1, parent->d_name.name, nb);
		*path = '/';
		vfree(path_tmp);
		n += nb + 1;
		nb = 0;
		parent = parent->d_parent;
	}

	path_tmp = path;
	path = vmalloc(n + 1);
	memcpy(path, path_tmp, n);
	vfree(path_tmp);
	path[n] = '\0';
	//if(strncpy()){
	//	return -1;
	//}		
	vfree(path);

	return 0;
}


char * mount_point (struct file * file){
	char * mnt_point = file->f_path.mnt->mnt_devname;

	if (!strcmp("/dev/root", mnt_point)) {
		return NULL;
	}

	return mnt_point;
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
