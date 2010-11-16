#include "hooks-func.h"

char * dentry_path_ (struct dentry *dentry) {
	struct dentry *parent = dentry;
	char *path = NULL;
	char *path_tmp = NULL;
	int nb = 0, n = 0;

	while (!IS_ROOT(parent)) {
		nb = strlen(parent->d_name.name);
		path_tmp = path
		path = vmalloc(n+nb);
		memcpy(path, path_tmp, n);
		vfree(path_tmp);
		memcpy(path+n, parent->d_name.name, nb);
		n += nb;
		parent = parent->d_parent;
	}
	return path;
}
