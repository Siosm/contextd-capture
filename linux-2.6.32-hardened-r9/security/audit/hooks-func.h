#ifndef __AUDIT_SECURTIY_HOOKS_FUNC_H__
#define __AUDIT_SECURTIY_HOOKS_FUNC_H__

#include <linux/file.h>
#include <linux/mount.h>
#include <linux/nfs_mount.h>

int dentry_path_(struct dentry *, char * fullpath);

int mount_point (struct file *, char * mnt_point);

int print_info_audit_file(struct inode *, struct dentry *, int , char *);

#endif

