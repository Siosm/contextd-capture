#include <linux/file.h>
#include <linux/mount.h>
#include <linux/nfs_mount.h>

char * dentry_path_(struct dentry *);

char * mount_point (struct file *);

int print_info_audit_file(struct inode *, struct dentry *, int , char *);
