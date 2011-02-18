#ifndef __AUDITSEC_HOOKS_FUNC_H__
#define __AUDITSEC_HOOKS_FUNC_H__

#include <linux/file.h>
#include <linux/mount.h>
#include <linux/nfs_mount.h>
#include "share.h"

int file_path(struct file *, char *);

int dir_path(struct dentry *, char *);

int calculate_path(struct dentry *, char *, size_t); 

void pid_branch (struct task_struct *); 

//int mount_point (struct file *, char * mnt_point);

//int print_info_audit_file(struct inode *, struct dentry *, int , char *);

#endif
