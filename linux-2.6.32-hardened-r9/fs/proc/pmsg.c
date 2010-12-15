#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/io.h>

extern wait_queue_head_t log_wait;

extern int do_syslog(int type, char __user *bug, int count);

static int pmsg_open(struct inode * inode, struct file * file)
{
	return do_syslog(1,NULL,0);
}

static int pmsg_release(struct inode * inode, struct file * file)
{
	(void) do_syslog(0,NULL,0);
	return 0;
}

static ssize_t pmsg_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
	if ((file->f_flags & O_NONBLOCK) && !do_syslog(9, NULL, 0))
		return -EAGAIN;
	return do_syslog(2, buf, count);
}

static unsigned int pmsg_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, &log_wait, wait);
	if (do_syslog(9, NULL, 0))
		return POLLIN | POLLRDNORM;
	return 0;
}


static const struct file_operations proc_pmsg_operations = {
	.read		= pmsg_read,
	.poll		= pmsg_poll,
	.open		= pmsg_open,
	.release	= pmsg_release,
};

static int __init proc_pmsg_init(void)
{
	proc_create("pmsg", S_IRUSR, NULL, &proc_pmsg_operations);
	return 0;
}
module_init(proc_pmsg_init);
