/*
 *
 **/
#include <linux/pid.h>

static bool is_auth_started = false;
static bool is_priviledged = false;
static const pid_t daemon_pid = 0;

