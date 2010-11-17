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
		vfree(path_tmp);
		path[nb] = '/';
		memcpy(path, parent->d_name.name, nb);
		n += nb;
		nb = 0;
		parent = parent->d_parent;
	}

	path_tmp = path;
	path = vmalloc(n+2);
	memcpy(path+1, path_tmp, n);
	vfree(path_tmp);
	path[0] = '/';
	path[n+1] = '\0';

	return path;
}
