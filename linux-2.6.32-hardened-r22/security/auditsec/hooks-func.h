#ifndef __AUDITSEC_HOOKS_FUNC_H__
#define __AUDITSEC_HOOKS_FUNC_H__


#include <linux/file.h>
#include <linux/mount.h>
#include <linux/nfs_mount.h>
#include "share.h"


int file_path(struct file *, char *);

int dir_path(struct dentry *, char *);

int calculate_path(struct dentry *, char *, size_t);


#endif /* hooks-func.h */