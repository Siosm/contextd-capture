#include "hooks-func.h"

char * dentry_path_(struct dentry *dentry)
{
	struct dentry *parent = dentry;
	char *path = NULL;
	char *path_tmp = NULL;
	int nb = 0, n = 0;

	while(!IS_ROOT(parent)){
		nb = strlen(parent->d_name.name);
		path_tmp = path;
		path = vmalloc(n+nb+1);
		memcpy(path+nb+1, path_tmp, n);
		memcpy(path+1, parent->d_name.name, nb);
		*path = '/';
		vfree(path_tmp);
		n += nb+1;
		nb = 0;
		parent = parent->d_parent;
	}

	path_tmp = path;
	path = vmalloc(n+1);
	memcpy(path, path_tmp, n);
	vfree(path_tmp);
	path[n] = '\0';

	return path;
}

int print_info_audit_file(struct inode *dir, struct dentry *dentry, int mask, char *hook_name)
{
	//char * path_buf = vmalloc(sizeof(char) * PATH_MAX);
	char * path_buf = NULL;
	/*if(path_buf == NULL){
		printk(KERN_INFO "USA: %s: %s (vmalloc failed)", hook_name, dentry->d_name.name);
		return 1;
	}*/
	path_buf = dentry_path_(dentry);
	//dentry_path(dentry, path_buf, PATH_MAX);
	printk(KERN_INFO "USA: %s: %s, mask: %d", hook_name, path_buf, mask);
	vfree(path_buf);

	return 0;
}

// 1285 : ligne context selinux
/*
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
