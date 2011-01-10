/*
 *
 **/
#include <linux/pid.h>

static bool auth_started = false;
static pid_t daemon_pid = 0;

enum ausec_type { AUSEC_FILE, AUSEC_SOCKET};

struct ausec_file {
	pid_t	pid;
	char*	full_path;
	char*	sc;
};

struct ausec_socket {
	pid_t	pid;
	char*	ip_dest;
	int		port;
};

struct ausec_info {
	enum ausec_type type;
	union
	{
		struct ausec_file;
		struct ausec_socket;
	} ausec_struct;
};

