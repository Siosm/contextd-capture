/*  Kernel module help for x86.
    Copyright (C) 2001 Rusty Russell.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <linux/moduleloader.h>
#include <linux/elf.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/mm.h>

#include <asm/system.h>
#include <asm/page.h>
#include <asm/pgtable.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(fmt...)
#endif

static void *__module_alloc(unsigned long size, pgprot_t prot)
{
	struct vm_struct *area;

	if (!size)
		return NULL;
	size = PAGE_ALIGN(size);
	if (size > MODULES_LEN)
		return NULL;

	area = __get_vm_area(size, VM_ALLOC, MODULES_VADDR, MODULES_END);
	if (!area)
		return NULL;

	return __vmalloc_area(area, GFP_KERNEL | __GFP_HIGHMEM | __GFP_ZERO, prot);
}

void *module_alloc(unsigned long size)
{

#ifdef CONFIG_PAX_KERNEXEC
	return __module_alloc(size, PAGE_KERNEL);
#else
	return __module_alloc(size, PAGE_KERNEL_EXEC);
#endif

}

/* Free memory returned from module_alloc */
void module_free(struct module *mod, void *module_region)
{
	vfree(module_region);
}

#ifdef CONFIG_PAX_KERNEXEC
#ifdef CONFIG_X86_32
void *module_alloc_exec(unsigned long size)
{
	struct vm_struct *area;

	if (size == 0)
		return NULL;

	area = __get_vm_area(size, VM_ALLOC, (unsigned long)&MODULES_EXEC_VADDR, (unsigned long)&MODULES_EXEC_END);
	return area ? area->addr : NULL;
}
EXPORT_SYMBOL(module_alloc_exec);

void module_free_exec(struct module *mod, void *module_region)
{
	vunmap(module_region);
}
EXPORT_SYMBOL(module_free_exec);
#else
void module_free_exec(struct module *mod, void *module_region)
{
	module_free(mod, module_region);
}
EXPORT_SYMBOL(module_free_exec);

void *module_alloc_exec(unsigned long size)
{
	return __module_alloc(size, PAGE_KERNEL_RX);
}
EXPORT_SYMBOL(module_alloc_exec);
#endif
#endif

/* We don't need anything special. */
int module_frob_arch_sections(Elf_Ehdr *hdr,
			      Elf_Shdr *sechdrs,
			      char *secstrings,
			      struct module *mod)
{
	return 0;
}

#ifdef CONFIG_X86_32
int apply_relocate(Elf32_Shdr *sechdrs,
		   const char *strtab,
		   unsigned int symindex,
		   unsigned int relsec,
		   struct module *me)
{
	unsigned int i;
	Elf32_Rel *rel = (void *)sechdrs[relsec].sh_addr;
	Elf32_Sym *sym;
	uint32_t *plocation, location;

	DEBUGP("Applying relocate section %u to %u\n", relsec,
	       sechdrs[relsec].sh_info);
	for (i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++) {
		/* This is where to make the change */
		plocation = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr + rel[i].r_offset;
		location = (uint32_t)plocation;
		if (sechdrs[sechdrs[relsec].sh_info].sh_flags & SHF_EXECINSTR)
			plocation = ktla_ktva((void *)plocation);
		/* This is the symbol it is referring to.  Note that all
		   undefined symbols have been resolved.  */
		sym = (Elf32_Sym *)sechdrs[symindex].sh_addr
			+ ELF32_R_SYM(rel[i].r_info);

		switch (ELF32_R_TYPE(rel[i].r_info)) {
		case R_386_32:
			/* We add the value into the location given */
			pax_open_kernel();
			*plocation += sym->st_value;
			pax_close_kernel();
			break;
		case R_386_PC32:
			/* Add the value, subtract its postition */
			pax_open_kernel();
			*plocation += sym->st_value - location;
			pax_close_kernel();
			break;
		default:
			printk(KERN_ERR "module %s: Unknown relocation: %u\n",
			       me->name, ELF32_R_TYPE(rel[i].r_info));
			return -ENOEXEC;
		}
	}
	return 0;
}

int apply_relocate_add(Elf32_Shdr *sechdrs,
		       const char *strtab,
		       unsigned int symindex,
		       unsigned int relsec,
		       struct module *me)
{
	printk(KERN_ERR "module %s: ADD RELOCATION unsupported\n",
	       me->name);
	return -ENOEXEC;
}
#else /*X86_64*/
int apply_relocate_add(Elf64_Shdr *sechdrs,
		   const char *strtab,
		   unsigned int symindex,
		   unsigned int relsec,
		   struct module *me)
{
	unsigned int i;
	Elf64_Rela *rel = (void *)sechdrs[relsec].sh_addr;
	Elf64_Sym *sym;
	void *loc;
	u64 val;

	DEBUGP("Applying relocate section %u to %u\n", relsec,
	       sechdrs[relsec].sh_info);
	for (i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++) {
		/* This is where to make the change */
		loc = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr
			+ rel[i].r_offset;

		/* This is the symbol it is referring to.  Note that all
		   undefined symbols have been resolved.  */
		sym = (Elf64_Sym *)sechdrs[symindex].sh_addr
			+ ELF64_R_SYM(rel[i].r_info);

		DEBUGP("type %d st_value %Lx r_addend %Lx loc %Lx\n",
			(int)ELF64_R_TYPE(rel[i].r_info),
			sym->st_value, rel[i].r_addend, (u64)loc);

		val = sym->st_value + rel[i].r_addend;

		switch (ELF64_R_TYPE(rel[i].r_info)) {
		case R_X86_64_NONE:
			break;
		case R_X86_64_64:
			pax_open_kernel();
			*(u64 *)loc = val;
			pax_close_kernel();
			break;
		case R_X86_64_32:
			pax_open_kernel();
			*(u32 *)loc = val;
			pax_close_kernel();
			if (val != *(u32 *)loc)
				goto overflow;
			break;
		case R_X86_64_32S:
			pax_open_kernel();
			*(s32 *)loc = val;
			pax_close_kernel();
			if ((s64)val != *(s32 *)loc)
				goto overflow;
			break;
		case R_X86_64_PC32:
			val -= (u64)loc;
			pax_open_kernel();
			*(u32 *)loc = val;
			pax_close_kernel();

#if 0
			if ((s64)val != *(s32 *)loc)
				goto overflow;
#endif
			break;
		default:
			printk(KERN_ERR "module %s: Unknown rela relocation: %llu\n",
			       me->name, ELF64_R_TYPE(rel[i].r_info));
			return -ENOEXEC;
		}
	}
	return 0;

overflow:
	printk(KERN_ERR "overflow in relocation type %d val %Lx\n",
	       (int)ELF64_R_TYPE(rel[i].r_info), val);
	printk(KERN_ERR "`%s' likely not compiled with -mcmodel=kernel\n",
	       me->name);
	return -ENOEXEC;
}

int apply_relocate(Elf_Shdr *sechdrs,
		   const char *strtab,
		   unsigned int symindex,
		   unsigned int relsec,
		   struct module *me)
{
	printk(KERN_ERR "non add relocation not supported\n");
	return -ENOSYS;
}

#endif

int module_finalize(const Elf_Ehdr *hdr,
		    const Elf_Shdr *sechdrs,
		    struct module *me)
{
	const Elf_Shdr *s, *text = NULL, *alt = NULL, *locks = NULL,
		*para = NULL;
	char *secstrings = (void *)hdr + sechdrs[hdr->e_shstrndx].sh_offset;

	for (s = sechdrs; s < sechdrs + hdr->e_shnum; s++) {
		if (!strcmp(".text", secstrings + s->sh_name))
			text = s;
		if (!strcmp(".altinstructions", secstrings + s->sh_name))
			alt = s;
		if (!strcmp(".smp_locks", secstrings + s->sh_name))
			locks = s;
		if (!strcmp(".parainstructions", secstrings + s->sh_name))
			para = s;
	}

	if (alt) {
		/* patch .altinstructions */
		void *aseg = (void *)alt->sh_addr;
		apply_alternatives(aseg, aseg + alt->sh_size);
	}
	if (locks && text) {
		void *lseg = (void *)locks->sh_addr;
		void *tseg = (void *)text->sh_addr;
		alternatives_smp_module_add(me, me->name,
					    lseg, lseg + locks->sh_size,
					    tseg, tseg + text->sh_size);
	}

	if (para) {
		void *pseg = (void *)para->sh_addr;
		apply_paravirt(pseg, pseg + para->sh_size);
	}

	return module_bug_finalize(hdr, sechdrs, me);
}

void module_arch_cleanup(struct module *mod)
{
	alternatives_smp_module_del(mod);
	module_bug_cleanup(mod);
}
