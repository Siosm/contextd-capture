/* By Ross Biro 1/23/92 */
/*
 * Pentium III FXSR, SSE support
 *	Gareth Hughes <gareth@valinux.com>, May 2000
 *
 * BTS tracing
 *	Markus Metzger <markus.t.metzger@intel.com>, Dec 2007
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/regset.h>
#include <linux/tracehook.h>
#include <linux/user.h>
#include <linux/elf.h>
#include <linux/security.h>
#include <linux/audit.h>
#include <linux/seccomp.h>
#include <linux/signal.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/processor.h>
#include <asm/i387.h>
#include <asm/debugreg.h>
#include <asm/ldt.h>
#include <asm/desc.h>
#include <asm/prctl.h>
#include <asm/proto.h>
#include <asm/ds.h>

#include "tls.h"

#define CREATE_TRACE_POINTS
#include <trace/events/syscalls.h>

enum x86_regset {
	REGSET_GENERAL,
	REGSET_FP,
	REGSET_XFP,
	REGSET_IOPERM64 = REGSET_XFP,
	REGSET_TLS,
	REGSET_IOPERM32,
};

/*
 * does not yet catch signals sent when the child dies.
 * in exit.c or in signal.c.
 */

/*
 * Determines which flags the user has access to [1 = access, 0 = no access].
 */
#define FLAG_MASK_32		((unsigned long)			\
				 (X86_EFLAGS_CF | X86_EFLAGS_PF |	\
				  X86_EFLAGS_AF | X86_EFLAGS_ZF |	\
				  X86_EFLAGS_SF | X86_EFLAGS_TF |	\
				  X86_EFLAGS_DF | X86_EFLAGS_OF |	\
				  X86_EFLAGS_RF | X86_EFLAGS_AC))

/*
 * Determines whether a value may be installed in a segment register.
 */
static inline bool invalid_selector(u16 value)
{
	return unlikely(value != 0 && (value & SEGMENT_RPL_MASK) != USER_RPL);
}

#ifdef CONFIG_X86_32

#define FLAG_MASK		FLAG_MASK_32

static unsigned long *pt_regs_access(struct pt_regs *regs, unsigned long regno)
{
	BUILD_BUG_ON(offsetof(struct pt_regs, bx) != 0);
	return &regs->bx + (regno >> 2);
}

static u16 get_segment_reg(struct task_struct *task, unsigned long offset)
{
	/*
	 * Returning the value truncates it to 16 bits.
	 */
	unsigned int retval;
	if (offset != offsetof(struct user_regs_struct, gs))
		retval = *pt_regs_access(task_pt_regs(task), offset);
	else {
		if (task == current)
			retval = get_user_gs(task_pt_regs(task));
		else
			retval = task_user_gs(task);
	}
	return retval;
}

static int set_segment_reg(struct task_struct *task,
			   unsigned long offset, u16 value)
{
	/*
	 * The value argument was already truncated to 16 bits.
	 */
	if (invalid_selector(value))
		return -EIO;

	/*
	 * For %cs and %ss we cannot permit a null selector.
	 * We can permit a bogus selector as long as it has USER_RPL.
	 * Null selectors are fine for other segment registers, but
	 * we will never get back to user mode with invalid %cs or %ss
	 * and will take the trap in iret instead.  Much code relies
	 * on user_mode() to distinguish a user trap frame (which can
	 * safely use invalid selectors) from a kernel trap frame.
	 */
	switch (offset) {
	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ss):
		if (unlikely(value == 0))
			return -EIO;

	default:
		*pt_regs_access(task_pt_regs(task), offset) = value;
		break;

	case offsetof(struct user_regs_struct, gs):
		if (task == current)
			set_user_gs(task_pt_regs(task), value);
		else
			task_user_gs(task) = value;
	}

	return 0;
}

static unsigned long debugreg_addr_limit(struct task_struct *task)
{
	return TASK_SIZE - 3;
}

#else  /* CONFIG_X86_64 */

#define FLAG_MASK		(FLAG_MASK_32 | X86_EFLAGS_NT)

static unsigned long *pt_regs_access(struct pt_regs *regs, unsigned long offset)
{
	BUILD_BUG_ON(offsetof(struct pt_regs, r15) != 0);
	return &regs->r15 + (offset / sizeof(regs->r15));
}

static u16 get_segment_reg(struct task_struct *task, unsigned long offset)
{
	/*
	 * Returning the value truncates it to 16 bits.
	 */
	unsigned int seg;

	switch (offset) {
	case offsetof(struct user_regs_struct, fs):
		if (task == current) {
			/* Older gas can't assemble movq %?s,%r?? */
			asm("movl %%fs,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.fsindex;
	case offsetof(struct user_regs_struct, gs):
		if (task == current) {
			asm("movl %%gs,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.gsindex;
	case offsetof(struct user_regs_struct, ds):
		if (task == current) {
			asm("movl %%ds,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.ds;
	case offsetof(struct user_regs_struct, es):
		if (task == current) {
			asm("movl %%es,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.es;

	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ss):
		break;
	}
	return *pt_regs_access(task_pt_regs(task), offset);
}

static int set_segment_reg(struct task_struct *task,
			   unsigned long offset, u16 value)
{
	/*
	 * The value argument was already truncated to 16 bits.
	 */
	if (invalid_selector(value))
		return -EIO;

	switch (offset) {
	case offsetof(struct user_regs_struct,fs):
		/*
		 * If this is setting fs as for normal 64-bit use but
		 * setting fs_base has implicitly changed it, leave it.
		 */
		if ((value == FS_TLS_SEL && task->thread.fsindex == 0 &&
		     task->thread.fs != 0) ||
		    (value == 0 && task->thread.fsindex == FS_TLS_SEL &&
		     task->thread.fs == 0))
			break;
		task->thread.fsindex = value;
		if (task == current)
			loadsegment(fs, task->thread.fsindex);
		break;
	case offsetof(struct user_regs_struct,gs):
		/*
		 * If this is setting gs as for normal 64-bit use but
		 * setting gs_base has implicitly changed it, leave it.
		 */
		if ((value == GS_TLS_SEL && task->thread.gsindex == 0 &&
		     task->thread.gs != 0) ||
		    (value == 0 && task->thread.gsindex == GS_TLS_SEL &&
		     task->thread.gs == 0))
			break;
		task->thread.gsindex = value;
		if (task == current)
			load_gs_index(task->thread.gsindex);
		break;
	case offsetof(struct user_regs_struct,ds):
		task->thread.ds = value;
		if (task == current)
			loadsegment(ds, task->thread.ds);
		break;
	case offsetof(struct user_regs_struct,es):
		task->thread.es = value;
		if (task == current)
			loadsegment(es, task->thread.es);
		break;

		/*
		 * Can't actually change these in 64-bit mode.
		 */
	case offsetof(struct user_regs_struct,cs):
		if (unlikely(value == 0))
			return -EIO;
#ifdef CONFIG_IA32_EMULATION
		if (test_tsk_thread_flag(task, TIF_IA32))
			task_pt_regs(task)->cs = value;
#endif
		break;
	case offsetof(struct user_regs_struct,ss):
		if (unlikely(value == 0))
			return -EIO;
#ifdef CONFIG_IA32_EMULATION
		if (test_tsk_thread_flag(task, TIF_IA32))
			task_pt_regs(task)->ss = value;
#endif
		break;
	}

	return 0;
}

static unsigned long debugreg_addr_limit(struct task_struct *task)
{
#ifdef CONFIG_IA32_EMULATION
	if (test_tsk_thread_flag(task, TIF_IA32))
		return IA32_PAGE_OFFSET - 3;
#endif
	return TASK_SIZE_MAX - 7;
}

#endif	/* CONFIG_X86_32 */

static unsigned long get_flags(struct task_struct *task)
{
	unsigned long retval = task_pt_regs(task)->flags;

	/*
	 * If the debugger set TF, hide it from the readout.
	 */
	if (test_tsk_thread_flag(task, TIF_FORCED_TF))
		retval &= ~X86_EFLAGS_TF;

	return retval;
}

static int set_flags(struct task_struct *task, unsigned long value)
{
	struct pt_regs *regs = task_pt_regs(task);

	/*
	 * If the user value contains TF, mark that
	 * it was not "us" (the debugger) that set it.
	 * If not, make sure it stays set if we had.
	 */
	if (value & X86_EFLAGS_TF)
		clear_tsk_thread_flag(task, TIF_FORCED_TF);
	else if (test_tsk_thread_flag(task, TIF_FORCED_TF))
		value |= X86_EFLAGS_TF;

	regs->flags = (regs->flags & ~FLAG_MASK) | (value & FLAG_MASK);

	return 0;
}

static int putreg(struct task_struct *child,
		  unsigned long offset, unsigned long value)
{
	switch (offset) {
	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ds):
	case offsetof(struct user_regs_struct, es):
	case offsetof(struct user_regs_struct, fs):
	case offsetof(struct user_regs_struct, gs):
	case offsetof(struct user_regs_struct, ss):
		return set_segment_reg(child, offset, value);

	case offsetof(struct user_regs_struct, flags):
		return set_flags(child, value);

#ifdef CONFIG_X86_64
	case offsetof(struct user_regs_struct,fs_base):
		if (value >= TASK_SIZE_OF(child))
			return -EIO;
		/*
		 * When changing the segment base, use do_arch_prctl
		 * to set either thread.fs or thread.fsindex and the
		 * corresponding GDT slot.
		 */
		if (child->thread.fs != value)
			return do_arch_prctl(child, ARCH_SET_FS, value);
		return 0;
	case offsetof(struct user_regs_struct,gs_base):
		/*
		 * Exactly the same here as the %fs handling above.
		 */
		if (value >= TASK_SIZE_OF(child))
			return -EIO;
		if (child->thread.gs != value)
			return do_arch_prctl(child, ARCH_SET_GS, value);
		return 0;
#endif
	}

	*pt_regs_access(task_pt_regs(child), offset) = value;
	return 0;
}

static unsigned long getreg(struct task_struct *task, unsigned long offset)
{
	switch (offset) {
	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ds):
	case offsetof(struct user_regs_struct, es):
	case offsetof(struct user_regs_struct, fs):
	case offsetof(struct user_regs_struct, gs):
	case offsetof(struct user_regs_struct, ss):
		return get_segment_reg(task, offset);

	case offsetof(struct user_regs_struct, flags):
		return get_flags(task);

#ifdef CONFIG_X86_64
	case offsetof(struct user_regs_struct, fs_base): {
		/*
		 * do_arch_prctl may have used a GDT slot instead of
		 * the MSR.  To userland, it appears the same either
		 * way, except the %fs segment selector might not be 0.
		 */
		unsigned int seg = task->thread.fsindex;
		if (task->thread.fs != 0)
			return task->thread.fs;
		if (task == current)
			asm("movl %%fs,%0" : "=r" (seg));
		if (seg != FS_TLS_SEL)
			return 0;
		return get_desc_base(&task->thread.tls_array[FS_TLS]);
	}
	case offsetof(struct user_regs_struct, gs_base): {
		/*
		 * Exactly the same here as the %fs handling above.
		 */
		unsigned int seg = task->thread.gsindex;
		if (task->thread.gs != 0)
			return task->thread.gs;
		if (task == current)
			asm("movl %%gs,%0" : "=r" (seg));
		if (seg != GS_TLS_SEL)
			return 0;
		return get_desc_base(&task->thread.tls_array[GS_TLS]);
	}
#endif
	}

	return *pt_regs_access(task_pt_regs(task), offset);
}

static int genregs_get(struct task_struct *target,
		       const struct user_regset *regset,
		       unsigned int pos, unsigned int count,
		       void *kbuf, void __user *ubuf)
{
	if (kbuf) {
		unsigned long *k = kbuf;
		while (count >= sizeof(*k)) {
			*k++ = getreg(target, pos);
			count -= sizeof(*k);
			pos += sizeof(*k);
		}
	} else {
		unsigned long __user *u = ubuf;
		while (count >= sizeof(*u)) {
			if (__put_user(getreg(target, pos), u++))
				return -EFAULT;
			count -= sizeof(*u);
			pos += sizeof(*u);
		}
	}

	return 0;
}

static int genregs_set(struct task_struct *target,
		       const struct user_regset *regset,
		       unsigned int pos, unsigned int count,
		       const void *kbuf, const void __user *ubuf)
{
	int ret = 0;
	if (kbuf) {
		const unsigned long *k = kbuf;
		while (count >= sizeof(*k) && !ret) {
			ret = putreg(target, pos, *k++);
			count -= sizeof(*k);
			pos += sizeof(*k);
		}
	} else {
		const unsigned long  __user *u = ubuf;
		while (count >= sizeof(*u) && !ret) {
			unsigned long word;
			ret = __get_user(word, u++);
			if (ret)
				break;
			ret = putreg(target, pos, word);
			count -= sizeof(*u);
			pos += sizeof(*u);
		}
	}
	return ret;
}

/*
 * This function is trivial and will be inlined by the compiler.
 * Having it separates the implementation details of debug
 * registers from the interface details of ptrace.
 */
static unsigned long ptrace_get_debugreg(struct task_struct *child, int n)
{
	switch (n) {
	case 0:		return child->thread.debugreg0;
	case 1:		return child->thread.debugreg1;
	case 2:		return child->thread.debugreg2;
	case 3:		return child->thread.debugreg3;
	case 6:		return child->thread.debugreg6;
	case 7:		return child->thread.debugreg7;
	}
	return 0;
}

static int ptrace_set_debugreg(struct task_struct *child,
			       int n, unsigned long data)
{
	int i;

	if (unlikely(n == 4 || n == 5))
		return -EIO;

	if (n < 4 && unlikely(data >= debugreg_addr_limit(child)))
		return -EIO;

	switch (n) {
	case 0:		child->thread.debugreg0 = data; break;
	case 1:		child->thread.debugreg1 = data; break;
	case 2:		child->thread.debugreg2 = data; break;
	case 3:		child->thread.debugreg3 = data; break;

	case 6:
		if ((data & ~0xffffffffUL) != 0)
			return -EIO;
		child->thread.debugreg6 = data;
		break;

	case 7:
		/*
		 * Sanity-check data. Take one half-byte at once with
		 * check = (val >> (16 + 4*i)) & 0xf. It contains the
		 * R/Wi and LENi bits; bits 0 and 1 are R/Wi, and bits
		 * 2 and 3 are LENi. Given a list of invalid values,
		 * we do mask |= 1 << invalid_value, so that
		 * (mask >> check) & 1 is a correct test for invalid
		 * values.
		 *
		 * R/Wi contains the type of the breakpoint /
		 * watchpoint, LENi contains the length of the watched
		 * data in the watchpoint case.
		 *
		 * The invalid values are:
		 * - LENi == 0x10 (undefined), so mask |= 0x0f00.	[32-bit]
		 * - R/Wi == 0x10 (break on I/O reads or writes), so
		 *   mask |= 0x4444.
		 * - R/Wi == 0x00 && LENi != 0x00, so we have mask |=
		 *   0x1110.
		 *
		 * Finally, mask = 0x0f00 | 0x4444 | 0x1110 == 0x5f54.
		 *
		 * See the Intel Manual "System Programming Guide",
		 * 15.2.4
		 *
		 * Note that LENi == 0x10 is defined on x86_64 in long
		 * mode (i.e. even for 32-bit userspace software, but
		 * 64-bit kernel), so the x86_64 mask value is 0x5454.
		 * See the AMD manual no. 24593 (AMD64 System Programming)
		 */
#ifdef CONFIG_X86_32
#define	DR7_MASK	0x5f54
#else
#define	DR7_MASK	0x5554
#endif
		data &= ~DR_CONTROL_RESERVED;
		for (i = 0; i < 4; i++)
			if ((DR7_MASK >> ((data >> (16 + 4*i)) & 0xf)) & 1)
				return -EIO;
		child->thread.debugreg7 = data;
		if (data)
			set_tsk_thread_flag(child, TIF_DEBUG);
		else
			clear_tsk_thread_flag(child, TIF_DEBUG);
		break;
	}

	return 0;
}

/*
 * These access the current or another (stopped) task's io permission
 * bitmap for debugging or core dump.
 */
static int ioperm_active(struct task_struct *target,
			 const struct user_regset *regset)
{
	return target->thread.io_bitmap_max / regset->size;
}

static int ioperm_get(struct task_struct *target,
		      const struct user_regset *regset,
		      unsigned int pos, unsigned int count,
		      void *kbuf, void __user *ubuf)
{
	if (!target->thread.io_bitmap_ptr)
		return -ENXIO;

	return user_regset_copyout(&pos, &count, &kbuf, &ubuf,
				   target->thread.io_bitmap_ptr,
				   0, IO_BITMAP_BYTES);
}

#ifdef CONFIG_X86_PTRACE_BTS
/*
 * A branch trace store context.
 *
 * Contexts may only be installed by ptrace_bts_config() and only for
 * ptraced tasks.
 *
 * Contexts are destroyed when the tracee is detached from the tracer.
 * The actual destruction work requires interrupts enabled, so the
 * work is deferred and will be scheduled during __ptrace_unlink().
 *
 * Contexts hold an additional task_struct reference on the traced
 * task, as well as a reference on the tracer's mm.
 *
 * Ptrace already holds a task_struct for the duration of ptrace operations,
 * but since destruction is deferred, it may be executed after both
 * tracer and tracee exited.
 */
struct bts_context {
	/* The branch trace handle. */
	struct bts_tracer	*tracer;

	/* The buffer used to store the branch trace and its size. */
	void			*buffer;
	unsigned int		size;

	/* The mm that paid for the above buffer. */
	struct mm_struct	*mm;

	/* The task this context belongs to. */
	struct task_struct	*task;

	/* The signal to send on a bts buffer overflow. */
	unsigned int		bts_ovfl_signal;

	/* The work struct to destroy a context. */
	struct work_struct	work;
};

static int alloc_bts_buffer(struct bts_context *context, unsigned int size)
{
	void *buffer = NULL;
	int err = -ENOMEM;

	err = account_locked_memory(current->mm, current->signal->rlim, size);
	if (err < 0)
		return err;

	buffer = kzalloc(size, GFP_KERNEL);
	if (!buffer)
		goto out_refund;

	context->buffer = buffer;
	context->size = size;
	context->mm = get_task_mm(current);

	return 0;

 out_refund:
	refund_locked_memory(current->mm, size);
	return err;
}

static inline void free_bts_buffer(struct bts_context *context)
{
	if (!context->buffer)
		return;

	kfree(context->buffer);
	context->buffer = NULL;

	refund_locked_memory(context->mm, context->size);
	context->size = 0;

	mmput(context->mm);
	context->mm = NULL;
}

static void free_bts_context_work(struct work_struct *w)
{
	struct bts_context *context;

	context = container_of(w, struct bts_context, work);

	ds_release_bts(context->tracer);
	put_task_struct(context->task);
	free_bts_buffer(context);
	kfree(context);
}

static inline void free_bts_context(struct bts_context *context)
{
	INIT_WORK(&context->work, free_bts_context_work);
	schedule_work(&context->work);
}

static inline struct bts_context *alloc_bts_context(struct task_struct *task)
{
	struct bts_context *context = kzalloc(sizeof(*context), GFP_KERNEL);
	if (context) {
		context->task = task;
		task->bts = context;

		get_task_struct(task);
	}

	return context;
}

static int ptrace_bts_read_record(struct task_struct *child, size_t index,
				  struct bts_struct __user *out)
{
	struct bts_context *context;
	const struct bts_trace *trace;
	struct bts_struct bts;
	const unsigned char *at;
	int error;

	context = child->bts;
	if (!context)
		return -ESRCH;

	trace = ds_read_bts(context->tracer);
	if (!trace)
		return -ESRCH;

	at = trace->ds.top - ((index + 1) * trace->ds.size);
	if ((void *)at < trace->ds.begin)
		at += (trace->ds.n * trace->ds.size);

	if (!trace->read)
		return -EOPNOTSUPP;

	error = trace->read(context->tracer, at, &bts);
	if (error < 0)
		return error;

	if (copy_to_user(out, &bts, sizeof(bts)))
		return -EFAULT;

	return sizeof(bts);
}

static int ptrace_bts_drain(struct task_struct *child,
			    long size,
			    struct bts_struct __user *out)
{
	struct bts_context *context;
	const struct bts_trace *trace;
	const unsigned char *at;
	int error, drained = 0;

	context = child->bts;
	if (!context)
		return -ESRCH;

	trace = ds_read_bts(context->tracer);
	if (!trace)
		return -ESRCH;

	if (!trace->read)
		return -EOPNOTSUPP;

	if (size < (trace->ds.top - trace->ds.begin))
		return -EIO;

	for (at = trace->ds.begin; (void *)at < trace->ds.top;
	     out++, drained++, at += trace->ds.size) {
		struct bts_struct bts;

		error = trace->read(context->tracer, at, &bts);
		if (error < 0)
			return error;

		if (copy_to_user(out, &bts, sizeof(bts)))
			return -EFAULT;
	}

	memset(trace->ds.begin, 0, trace->ds.n * trace->ds.size);

	error = ds_reset_bts(context->tracer);
	if (error < 0)
		return error;

	return drained;
}

static int ptrace_bts_config(struct task_struct *child,
			     long cfg_size,
			     const struct ptrace_bts_config __user *ucfg)
{
	struct bts_context *context;
	struct ptrace_bts_config cfg;
	unsigned int flags = 0;

	if (cfg_size < sizeof(cfg))
		return -EIO;

	if (copy_from_user(&cfg, ucfg, sizeof(cfg)))
		return -EFAULT;

	context = child->bts;
	if (!context)
		context = alloc_bts_context(child);
	if (!context)
		return -ENOMEM;

	if (cfg.flags & PTRACE_BTS_O_SIGNAL) {
		if (!cfg.signal)
			return -EINVAL;

		return -EOPNOTSUPP;
		context->bts_ovfl_signal = cfg.signal;
	}

	ds_release_bts(context->tracer);
	context->tracer = NULL;

	if ((cfg.flags & PTRACE_BTS_O_ALLOC) && (cfg.size != context->size)) {
		int err;

		free_bts_buffer(context);
		if (!cfg.size)
			return 0;

		err = alloc_bts_buffer(context, cfg.size);
		if (err < 0)
			return err;
	}

	if (cfg.flags & PTRACE_BTS_O_TRACE)
		flags |= BTS_USER;

	if (cfg.flags & PTRACE_BTS_O_SCHED)
		flags |= BTS_TIMESTAMPS;

	context->tracer =
		ds_request_bts_task(child, context->buffer, context->size,
				    NULL, (size_t)-1, flags);
	if (unlikely(IS_ERR(context->tracer))) {
		int error = PTR_ERR(context->tracer);

		free_bts_buffer(context);
		context->tracer = NULL;
		return error;
	}

	return sizeof(cfg);
}

static int ptrace_bts_status(struct task_struct *child,
			     long cfg_size,
			     struct ptrace_bts_config __user *ucfg)
{
	struct bts_context *context;
	const struct bts_trace *trace;
	struct ptrace_bts_config cfg;

	context = child->bts;
	if (!context)
		return -ESRCH;

	if (cfg_size < sizeof(cfg))
		return -EIO;

	trace = ds_read_bts(context->tracer);
	if (!trace)
		return -ESRCH;

	memset(&cfg, 0, sizeof(cfg));
	cfg.size	= trace->ds.end - trace->ds.begin;
	cfg.signal	= context->bts_ovfl_signal;
	cfg.bts_size	= sizeof(struct bts_struct);

	if (cfg.signal)
		cfg.flags |= PTRACE_BTS_O_SIGNAL;

	if (trace->ds.flags & BTS_USER)
		cfg.flags |= PTRACE_BTS_O_TRACE;

	if (trace->ds.flags & BTS_TIMESTAMPS)
		cfg.flags |= PTRACE_BTS_O_SCHED;

	if (copy_to_user(ucfg, &cfg, sizeof(cfg)))
		return -EFAULT;

	return sizeof(cfg);
}

static int ptrace_bts_clear(struct task_struct *child)
{
	struct bts_context *context;
	const struct bts_trace *trace;

	context = child->bts;
	if (!context)
		return -ESRCH;

	trace = ds_read_bts(context->tracer);
	if (!trace)
		return -ESRCH;

	memset(trace->ds.begin, 0, trace->ds.n * trace->ds.size);

	return ds_reset_bts(context->tracer);
}

static int ptrace_bts_size(struct task_struct *child)
{
	struct bts_context *context;
	const struct bts_trace *trace;

	context = child->bts;
	if (!context)
		return -ESRCH;

	trace = ds_read_bts(context->tracer);
	if (!trace)
		return -ESRCH;

	return (trace->ds.top - trace->ds.begin) / trace->ds.size;
}

/*
 * Called from __ptrace_unlink() after the child has been moved back
 * to its original parent.
 */
void ptrace_bts_untrace(struct task_struct *child)
{
	if (unlikely(child->bts)) {
		free_bts_context(child->bts);
		child->bts = NULL;
	}
}
#endif /* CONFIG_X86_PTRACE_BTS */

/*
 * Called by kernel/ptrace.c when detaching..
 *
 * Make sure the single step bit is not set.
 */
void ptrace_disable(struct task_struct *child)
{
	user_disable_single_step(child);
#ifdef TIF_SYSCALL_EMU
	clear_tsk_thread_flag(child, TIF_SYSCALL_EMU);
#endif
}

#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
static const struct user_regset_view user_x86_32_view; /* Initialized below. */
#endif

long arch_ptrace(struct task_struct *child, long request, long addr, long data)
{
	int ret;
	unsigned long __user *datap = (__force unsigned long __user *)data;

	switch (request) {
	/* read the word at location addr in the USER area. */
	case PTRACE_PEEKUSR: {
		unsigned long tmp;

		ret = -EIO;
		if ((addr & (sizeof(data) - 1)) || addr < 0 ||
		    addr >= sizeof(struct user))
			break;

		tmp = 0;  /* Default return condition */
		if (addr < sizeof(struct user_regs_struct))
			tmp = getreg(child, addr);
		else if (addr >= offsetof(struct user, u_debugreg[0]) &&
			 addr <= offsetof(struct user, u_debugreg[7])) {
			addr -= offsetof(struct user, u_debugreg[0]);
			tmp = ptrace_get_debugreg(child, addr / sizeof(data));
		}
		ret = put_user(tmp, datap);
		break;
	}

	case PTRACE_POKEUSR: /* write the word at location addr in the USER area */
		ret = -EIO;
		if ((addr & (sizeof(data) - 1)) || addr < 0 ||
		    addr >= sizeof(struct user))
			break;

		if (addr < sizeof(struct user_regs_struct))
			ret = putreg(child, addr, data);
		else if (addr >= offsetof(struct user, u_debugreg[0]) &&
			 addr <= offsetof(struct user, u_debugreg[7])) {
			addr -= offsetof(struct user, u_debugreg[0]);
			ret = ptrace_set_debugreg(child,
						  addr / sizeof(data), data);
		}
		break;

	case PTRACE_GETREGS:	/* Get all gp regs from the child. */
		return copy_regset_to_user(child,
					   task_user_regset_view(current),
					   REGSET_GENERAL,
					   0, sizeof(struct user_regs_struct),
					   datap);

	case PTRACE_SETREGS:	/* Set all gp regs in the child. */
		return copy_regset_from_user(child,
					     task_user_regset_view(current),
					     REGSET_GENERAL,
					     0, sizeof(struct user_regs_struct),
					     datap);

	case PTRACE_GETFPREGS:	/* Get the child FPU state. */
		return copy_regset_to_user(child,
					   task_user_regset_view(current),
					   REGSET_FP,
					   0, sizeof(struct user_i387_struct),
					   datap);

	case PTRACE_SETFPREGS:	/* Set the child FPU state. */
		return copy_regset_from_user(child,
					     task_user_regset_view(current),
					     REGSET_FP,
					     0, sizeof(struct user_i387_struct),
					     datap);

#ifdef CONFIG_X86_32
	case PTRACE_GETFPXREGS:	/* Get the child extended FPU state. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_XFP,
					   0, sizeof(struct user_fxsr_struct),
					   datap) ? -EIO : 0;

	case PTRACE_SETFPXREGS:	/* Set the child extended FPU state. */
		return copy_regset_from_user(child, &user_x86_32_view,
					     REGSET_XFP,
					     0, sizeof(struct user_fxsr_struct),
					     datap) ? -EIO : 0;
#endif

#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
	case PTRACE_GET_THREAD_AREA:
		if (addr < 0)
			return -EIO;
		ret = do_get_thread_area(child, addr,
					 (__force struct user_desc __user *) data);
		break;

	case PTRACE_SET_THREAD_AREA:
		if (addr < 0)
			return -EIO;
		ret = do_set_thread_area(child, addr,
					 (__force struct user_desc __user *) data, 0);
		break;
#endif

#ifdef CONFIG_X86_64
		/* normal 64bit interface to access TLS data.
		   Works just like arch_prctl, except that the arguments
		   are reversed. */
	case PTRACE_ARCH_PRCTL:
		ret = do_arch_prctl(child, data, addr);
		break;
#endif

	/*
	 * These bits need more cooking - not enabled yet:
	 */
#ifdef CONFIG_X86_PTRACE_BTS
	case PTRACE_BTS_CONFIG:
		ret = ptrace_bts_config
			(child, data, (__force struct ptrace_bts_config __user *)addr);
		break;

	case PTRACE_BTS_STATUS:
		ret = ptrace_bts_status
			(child, data, (__force struct ptrace_bts_config __user *)addr);
		break;

	case PTRACE_BTS_SIZE:
		ret = ptrace_bts_size(child);
		break;

	case PTRACE_BTS_GET:
		ret = ptrace_bts_read_record
			(child, data, (__force struct bts_struct __user *) addr);
		break;

	case PTRACE_BTS_CLEAR:
		ret = ptrace_bts_clear(child);
		break;

	case PTRACE_BTS_DRAIN:
		ret = ptrace_bts_drain
			(child, data, (__force struct bts_struct __user *) addr);
		break;
#endif /* CONFIG_X86_PTRACE_BTS */

	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

#ifdef CONFIG_IA32_EMULATION

#include <linux/compat.h>
#include <linux/syscalls.h>
#include <asm/ia32.h>
#include <asm/user32.h>

#define R32(l,q)							\
	case offsetof(struct user32, regs.l):				\
		regs->q = value; break

#define SEG32(rs)							\
	case offsetof(struct user32, regs.rs):				\
		return set_segment_reg(child,				\
				       offsetof(struct user_regs_struct, rs), \
				       value);				\
		break

static int putreg32(struct task_struct *child, unsigned regno, u32 value)
{
	struct pt_regs *regs = task_pt_regs(child);

	switch (regno) {

	SEG32(cs);
	SEG32(ds);
	SEG32(es);
	SEG32(fs);
	SEG32(gs);
	SEG32(ss);

	R32(ebx, bx);
	R32(ecx, cx);
	R32(edx, dx);
	R32(edi, di);
	R32(esi, si);
	R32(ebp, bp);
	R32(eax, ax);
	R32(eip, ip);
	R32(esp, sp);

	case offsetof(struct user32, regs.orig_eax):
		/*
		 * A 32-bit debugger setting orig_eax means to restore
		 * the state of the task restarting a 32-bit syscall.
		 * Make sure we interpret the -ERESTART* codes correctly
		 * in case the task is not actually still sitting at the
		 * exit from a 32-bit syscall with TS_COMPAT still set.
		 */
		regs->orig_ax = value;
		if (syscall_get_nr(child, regs) >= 0)
			task_thread_info(child)->status |= TS_COMPAT;
		break;

	case offsetof(struct user32, regs.eflags):
		return set_flags(child, value);

	case offsetof(struct user32, u_debugreg[0]) ...
		offsetof(struct user32, u_debugreg[7]):
		regno -= offsetof(struct user32, u_debugreg[0]);
		return ptrace_set_debugreg(child, regno / 4, value);

	default:
		if (regno > sizeof(struct user32) || (regno & 3))
			return -EIO;

		/*
		 * Other dummy fields in the virtual user structure
		 * are ignored
		 */
		break;
	}
	return 0;
}

#undef R32
#undef SEG32

#define R32(l,q)							\
	case offsetof(struct user32, regs.l):				\
		*val = regs->q; break

#define SEG32(rs)							\
	case offsetof(struct user32, regs.rs):				\
		*val = get_segment_reg(child,				\
				       offsetof(struct user_regs_struct, rs)); \
		break

static int getreg32(struct task_struct *child, unsigned regno, u32 *val)
{
	struct pt_regs *regs = task_pt_regs(child);

	switch (regno) {

	SEG32(ds);
	SEG32(es);
	SEG32(fs);
	SEG32(gs);

	R32(cs, cs);
	R32(ss, ss);
	R32(ebx, bx);
	R32(ecx, cx);
	R32(edx, dx);
	R32(edi, di);
	R32(esi, si);
	R32(ebp, bp);
	R32(eax, ax);
	R32(orig_eax, orig_ax);
	R32(eip, ip);
	R32(esp, sp);

	case offsetof(struct user32, regs.eflags):
		*val = get_flags(child);
		break;

	case offsetof(struct user32, u_debugreg[0]) ...
		offsetof(struct user32, u_debugreg[7]):
		regno -= offsetof(struct user32, u_debugreg[0]);
		*val = ptrace_get_debugreg(child, regno / 4);
		break;

	default:
		if (regno > sizeof(struct user32) || (regno & 3))
			return -EIO;

		/*
		 * Other dummy fields in the virtual user structure
		 * are ignored
		 */
		*val = 0;
		break;
	}
	return 0;
}

#undef R32
#undef SEG32

static int genregs32_get(struct task_struct *target,
			 const struct user_regset *regset,
			 unsigned int pos, unsigned int count,
			 void *kbuf, void __user *ubuf)
{
	if (kbuf) {
		compat_ulong_t *k = kbuf;
		while (count >= sizeof(*k)) {
			getreg32(target, pos, k++);
			count -= sizeof(*k);
			pos += sizeof(*k);
		}
	} else {
		compat_ulong_t __user *u = ubuf;
		while (count >= sizeof(*u)) {
			compat_ulong_t word;
			getreg32(target, pos, &word);
			if (__put_user(word, u++))
				return -EFAULT;
			count -= sizeof(*u);
			pos += sizeof(*u);
		}
	}

	return 0;
}

static int genregs32_set(struct task_struct *target,
			 const struct user_regset *regset,
			 unsigned int pos, unsigned int count,
			 const void *kbuf, const void __user *ubuf)
{
	int ret = 0;
	if (kbuf) {
		const compat_ulong_t *k = kbuf;
		while (count >= sizeof(*k) && !ret) {
			ret = putreg32(target, pos, *k++);
			count -= sizeof(*k);
			pos += sizeof(*k);
		}
	} else {
		const compat_ulong_t __user *u = ubuf;
		while (count >= sizeof(*u) && !ret) {
			compat_ulong_t word;
			ret = __get_user(word, u++);
			if (ret)
				break;
			ret = putreg32(target, pos, word);
			count -= sizeof(*u);
			pos += sizeof(*u);
		}
	}
	return ret;
}

long compat_arch_ptrace(struct task_struct *child, compat_long_t request,
			compat_ulong_t caddr, compat_ulong_t cdata)
{
	unsigned long addr = caddr;
	unsigned long data = cdata;
	void __user *datap = compat_ptr(data);
	int ret;
	__u32 val;

	switch (request) {
	case PTRACE_PEEKUSR:
		ret = getreg32(child, addr, &val);
		if (ret == 0)
			ret = put_user(val, (__u32 __user *)datap);
		break;

	case PTRACE_POKEUSR:
		ret = putreg32(child, addr, data);
		break;

	case PTRACE_GETREGS:	/* Get all gp regs from the child. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_GENERAL,
					   0, sizeof(struct user_regs_struct32),
					   datap);

	case PTRACE_SETREGS:	/* Set all gp regs in the child. */
		return copy_regset_from_user(child, &user_x86_32_view,
					     REGSET_GENERAL, 0,
					     sizeof(struct user_regs_struct32),
					     datap);

	case PTRACE_GETFPREGS:	/* Get the child FPU state. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_FP, 0,
					   sizeof(struct user_i387_ia32_struct),
					   datap);

	case PTRACE_SETFPREGS:	/* Set the child FPU state. */
		return copy_regset_from_user(
			child, &user_x86_32_view, REGSET_FP,
			0, sizeof(struct user_i387_ia32_struct), datap);

	case PTRACE_GETFPXREGS:	/* Get the child extended FPU state. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_XFP, 0,
					   sizeof(struct user32_fxsr_struct),
					   datap);

	case PTRACE_SETFPXREGS:	/* Set the child extended FPU state. */
		return copy_regset_from_user(child, &user_x86_32_view,
					     REGSET_XFP, 0,
					     sizeof(struct user32_fxsr_struct),
					     datap);

	case PTRACE_GET_THREAD_AREA:
	case PTRACE_SET_THREAD_AREA:
#ifdef CONFIG_X86_PTRACE_BTS
	case PTRACE_BTS_CONFIG:
	case PTRACE_BTS_STATUS:
	case PTRACE_BTS_SIZE:
	case PTRACE_BTS_GET:
	case PTRACE_BTS_CLEAR:
	case PTRACE_BTS_DRAIN:
#endif /* CONFIG_X86_PTRACE_BTS */
		return arch_ptrace(child, request, addr, data);

	default:
		return compat_ptrace_request(child, request, addr, data);
	}

	return ret;
}

#endif	/* CONFIG_IA32_EMULATION */

#ifdef CONFIG_X86_64

static const struct user_regset x86_64_regsets[] = {
	[REGSET_GENERAL] = {
		.core_note_type = NT_PRSTATUS,
		.n = sizeof(struct user_regs_struct) / sizeof(long),
		.size = sizeof(long), .align = sizeof(long),
		.get = genregs_get, .set = genregs_set
	},
	[REGSET_FP] = {
		.core_note_type = NT_PRFPREG,
		.n = sizeof(struct user_i387_struct) / sizeof(long),
		.size = sizeof(long), .align = sizeof(long),
		.active = xfpregs_active, .get = xfpregs_get, .set = xfpregs_set
	},
	[REGSET_IOPERM64] = {
		.core_note_type = NT_386_IOPERM,
		.n = IO_BITMAP_LONGS,
		.size = sizeof(long), .align = sizeof(long),
		.active = ioperm_active, .get = ioperm_get
	},
};

static const struct user_regset_view user_x86_64_view = {
	.name = "x86_64", .e_machine = EM_X86_64,
	.regsets = x86_64_regsets, .n = ARRAY_SIZE(x86_64_regsets)
};

#else  /* CONFIG_X86_32 */

#define user_regs_struct32	user_regs_struct
#define genregs32_get		genregs_get
#define genregs32_set		genregs_set

#define user_i387_ia32_struct	user_i387_struct
#define user32_fxsr_struct	user_fxsr_struct

#endif	/* CONFIG_X86_64 */

#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
static const struct user_regset x86_32_regsets[] = {
	[REGSET_GENERAL] = {
		.core_note_type = NT_PRSTATUS,
		.n = sizeof(struct user_regs_struct32) / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.get = genregs32_get, .set = genregs32_set
	},
	[REGSET_FP] = {
		.core_note_type = NT_PRFPREG,
		.n = sizeof(struct user_i387_ia32_struct) / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.active = fpregs_active, .get = fpregs_get, .set = fpregs_set
	},
	[REGSET_XFP] = {
		.core_note_type = NT_PRXFPREG,
		.n = sizeof(struct user32_fxsr_struct) / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.active = xfpregs_active, .get = xfpregs_get, .set = xfpregs_set
	},
	[REGSET_TLS] = {
		.core_note_type = NT_386_TLS,
		.n = GDT_ENTRY_TLS_ENTRIES, .bias = GDT_ENTRY_TLS_MIN,
		.size = sizeof(struct user_desc),
		.align = sizeof(struct user_desc),
		.active = regset_tls_active,
		.get = regset_tls_get, .set = regset_tls_set
	},
	[REGSET_IOPERM32] = {
		.core_note_type = NT_386_IOPERM,
		.n = IO_BITMAP_BYTES / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.active = ioperm_active, .get = ioperm_get
	},
};

static const struct user_regset_view user_x86_32_view = {
	.name = "i386", .e_machine = EM_386,
	.regsets = x86_32_regsets, .n = ARRAY_SIZE(x86_32_regsets)
};
#endif

const struct user_regset_view *task_user_regset_view(struct task_struct *task)
{
#ifdef CONFIG_IA32_EMULATION
	if (test_tsk_thread_flag(task, TIF_IA32))
#endif
#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
		return &user_x86_32_view;
#endif
#ifdef CONFIG_X86_64
	return &user_x86_64_view;
#endif
}

void send_sigtrap(struct task_struct *tsk, struct pt_regs *regs,
					 int error_code, int si_code)
{
	struct siginfo info;

	tsk->thread.trap_no = 1;
	tsk->thread.error_code = error_code;

	memset(&info, 0, sizeof(info));
	info.si_signo = SIGTRAP;
	info.si_code = si_code;

	/* User-mode ip? */
	info.si_addr = user_mode(regs) ? (__force void __user *) regs->ip : NULL;

	/* Send us the fake SIGTRAP */
	force_sig_info(SIGTRAP, &info, tsk);
}


#ifdef CONFIG_X86_32
# define IS_IA32	1
#elif defined CONFIG_IA32_EMULATION
# define IS_IA32	is_compat_task()
#else
# define IS_IA32	0
#endif

/*
 * We must return the syscall number to actually look up in the table.
 * This can be -1L to skip running any syscall at all.
 */
asmregparm long syscall_trace_enter(struct pt_regs *regs)
{
	long ret = 0;

	/*
	 * If we stepped into a sysenter/syscall insn, it trapped in
	 * kernel mode; do_debug() cleared TF and set TIF_SINGLESTEP.
	 * If user-mode had set TF itself, then it's still clear from
	 * do_debug() and we need to set it again to restore the user
	 * state.  If we entered on the slow path, TF was already set.
	 */
	if (test_thread_flag(TIF_SINGLESTEP))
		regs->flags |= X86_EFLAGS_TF;

	/* do the secure computing check first */
	secure_computing(regs->orig_ax);

	if (unlikely(test_thread_flag(TIF_SYSCALL_EMU)))
		ret = -1L;

	if ((ret || test_thread_flag(TIF_SYSCALL_TRACE)) &&
	    tracehook_report_syscall_entry(regs))
		ret = -1L;

	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACEPOINT)))
		trace_sys_enter(regs, regs->orig_ax);

	if (unlikely(current->audit_context)) {
		if (IS_IA32)
			audit_syscall_entry(AUDIT_ARCH_I386,
					    regs->orig_ax,
					    regs->bx, regs->cx,
					    regs->dx, regs->si);
#ifdef CONFIG_X86_64
		else
			audit_syscall_entry(AUDIT_ARCH_X86_64,
					    regs->orig_ax,
					    regs->di, regs->si,
					    regs->dx, regs->r10);
#endif
	}

	return ret ?: regs->orig_ax;
}

asmregparm void syscall_trace_leave(struct pt_regs *regs)
{
	if (unlikely(current->audit_context))
		audit_syscall_exit(AUDITSC_RESULT(regs->ax), regs->ax);

	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACEPOINT)))
		trace_sys_exit(regs, regs->ax);

	if (test_thread_flag(TIF_SYSCALL_TRACE))
		tracehook_report_syscall_exit(regs, 0);

	/*
	 * If TIF_SYSCALL_EMU is set, we only get here because of
	 * TIF_SINGLESTEP (i.e. this is PTRACE_SYSEMU_SINGLESTEP).
	 * We already reported this syscall instruction in
	 * syscall_trace_enter(), so don't do any more now.
	 */
	if (unlikely(test_thread_flag(TIF_SYSCALL_EMU)))
		return;

	/*
	 * If we are single-stepping, synthesize a trap to follow the
	 * system call instruction.
	 */
	if (test_thread_flag(TIF_SINGLESTEP) &&
	    tracehook_consider_fatal_signal(current, SIGTRAP))
		send_sigtrap(current, regs, 0, TRAP_BRKPT);
}
