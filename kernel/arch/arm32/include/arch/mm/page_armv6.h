/*
 * Copyright (c) 2012 Jan Vesely
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup arm32mm
 * @{
 */
/** @file
 *  @brief Paging related declarations.
 */

#ifndef KERN_arm32_PAGE_armv7_H_
#define KERN_arm32_PAGE_armv7_H_

#ifndef KERN_arm32_PAGE_H_
#error "Do not include arch specific page.h directly use generic page.h instead"
#endif

/* Macros for querying the last-level PTE entries. */
#define PTE_VALID_ARCH(pte) \
	(*((uint32_t *) (pte)) != 0)
#define PTE_PRESENT_ARCH(pte) \
	(((pte_t *) (pte))->l0.descriptor_type != 0)
#define PTE_GET_FRAME_ARCH(pte) \
	(((pte_t *) (pte))->l1.frame_base_addr << FRAME_WIDTH)
#define PTE_WRITABLE_ARCH(pte) \
	(((pte_t *) (pte))->l1.access_permission_1 != PTE_AP1_RO)
#define PTE_EXECUTABLE_ARCH(pte) \
	(((pte_t *) (pte))->l1.descriptor_type != PTE_DESCRIPTOR_SMALL_PAGE_NX)

#ifndef __ASM__

/** Level 0 page table entry. */
typedef struct {
	/* 0b01 for coarse tables, see below for details */
	unsigned descriptor_type : 2;
	unsigned pxn : 1;
	unsigned ns : 1;
	unsigned should_be_zero_0 : 1;
	unsigned domain : 4;
	unsigned should_be_zero_1 : 1;

	/* Pointer to the coarse 2nd level page table (holding entries for small
	 * (4KB) or large (64KB) pages. ARM also supports fine 2nd level page
	 * tables that may hold even tiny pages (1KB) but they are bigger (4KB
	 * per table in comparison with 1KB per the coarse table)
	 */
	unsigned coarse_table_addr : 22;
} ATTRIBUTE_PACKED pte_level0_t;

/** Level 1 page table entry (small (4KB) pages used). */
typedef struct {

	/* 0b10 for small pages, 0b11 for NX small pages */
	unsigned descriptor_type : 2;
	unsigned bufferable : 1;
	unsigned cacheable : 1;
	unsigned access_permission_0 : 2;
	unsigned tex : 3;
	unsigned access_permission_1 : 1;
	unsigned shareable : 1;
	unsigned non_global : 1;
	unsigned frame_base_addr : 20;
} ATTRIBUTE_PACKED pte_level1_t;

typedef union {
	pte_level0_t l0;
	pte_level1_t l1;
} pte_t;

/* Level 1 page tables access permissions */

/** User mode: no access, privileged mode: no access. */
#define PTE_AP0_USER_NO_KERNEL_NO   0

/** User mode: no access, privileged mode: read/write. */
#define PTE_AP0_USER_NO_KERNEL_FULL   1

/** User mode: read only, privileged mode: read/write. */
#define PTE_AP0_USER_LIMITED_KERNEL_FULL   2

/** User mode: read/write, privileged mode: read/write. */
#define PTE_AP0_USER_FULL_KERNEL_FULL    3

/** Allow writes */
#define PTE_AP1_RO   1


/* pte_level0_t and pte_level1_t descriptor_type flags */

/** pte_level0_t and pte_level1_t "not present" flag (used in descriptor_type). */
#define PTE_DESCRIPTOR_NOT_PRESENT	0

/** pte_level0_t coarse page table flag (used in descriptor_type). */
#define PTE_DESCRIPTOR_COARSE_TABLE	1

/** pte_level1_t small page table flag (used in descriptor type). */
#define PTE_DESCRIPTOR_SMALL_PAGE	2

/** pte_level1_t small page table flag with NX (used in descriptor type). */
#define PTE_DESCRIPTOR_SMALL_PAGE_NX	3

/** Sets the address of level 0 page table.
 *
 * @param pt Pointer to the page table to set.
 *
 */
NO_TRACE static inline void set_ptl0_addr(pte_t *pt)
{
	asm volatile (
		"mcr p15, 0, %[pt], c2, c0, 0\n"
		:: [pt] "r" (pt)
	);
}


/** Returns level 0 page table entry flags.
 *
 * @param pt Level 0 page table.
 * @param i  Index of the entry to return.
 *
 */
NO_TRACE static inline int get_pt_level0_flags(pte_t *pt, size_t i)
{
	const pte_level0_t *p = &pt[i].l0;
	const unsigned np = (p->descriptor_type == PTE_DESCRIPTOR_NOT_PRESENT);
	
	return (np << PAGE_PRESENT_SHIFT) | (1 << PAGE_USER_SHIFT) |
	    (1 << PAGE_READ_SHIFT) | (1 << PAGE_WRITE_SHIFT) |
	    (1 << PAGE_EXEC_SHIFT) | (1 << PAGE_CACHEABLE_SHIFT);
}

/** Returns level 1 page table entry flags.
 *
 * @param pt Level 1 page table.
 * @param i  Index of the entry to return.
 *
 */
NO_TRACE static inline int get_pt_level1_flags(pte_t *pt, size_t i)
{
	const pte_level1_t *p = &pt[i].l1;
	
	const unsigned dt = p->descriptor_type;
	const unsigned ap0 = p->access_permission_0;
	const unsigned ap1 = p->access_permission_1;
	
	return ((dt == PTE_DESCRIPTOR_NOT_PRESENT) << PAGE_PRESENT_SHIFT) |
	    ((dt != PTE_DESCRIPTOR_SMALL_PAGE_NX) << PAGE_EXEC_SHIFT) |
	    ((ap0 == PTE_AP0_USER_LIMITED_KERNEL_FULL) << PAGE_READ_SHIFT) |
	    ((ap0 == PTE_AP0_USER_FULL_KERNEL_FULL) << PAGE_READ_SHIFT) |
	    ((ap0 == PTE_AP0_USER_NO_KERNEL_FULL) << PAGE_READ_SHIFT) |
	    ((ap0 != PTE_AP0_USER_NO_KERNEL_FULL) << PAGE_USER_SHIFT) |
	    (((ap1 != PTE_AP1_RO) && (ap0 == PTE_AP0_USER_FULL_KERNEL_FULL)) << PAGE_WRITE_SHIFT) |
	    (((ap1 != PTE_AP1_RO) && (ap0 == PTE_AP0_USER_NO_KERNEL_FULL)) << PAGE_WRITE_SHIFT) |
	    (p->bufferable << PAGE_CACHEABLE);
}

/** Sets flags of level 0 page table entry.
 *
 * @param pt    level 0 page table
 * @param i     index of the entry to be changed
 * @param flags new flags
 *
 */
NO_TRACE static inline void set_pt_level0_flags(pte_t *pt, size_t i, int flags)
{
	pte_level0_t *p = &pt[i].l0;
	
	if (flags & PAGE_NOT_PRESENT) {
		p->descriptor_type = PTE_DESCRIPTOR_NOT_PRESENT;
		/*
		 * Ensures that the entry will be recognized as valid when
		 * PTE_VALID_ARCH applied.
		 */
		p->should_be_zero_0 = 1;
		p->should_be_zero_1 = 1;
	} else {
		p->descriptor_type = PTE_DESCRIPTOR_COARSE_TABLE;
		p->should_be_zero_0 = 0;
		p->should_be_zero_1 = 0;
		p->domain = 0;
		p->ns = 0;
	}
}


/** Sets flags of level 1 page table entry.
 *
 * We use same access rights for the whole page. When page
 * is not preset we store 1 in acess_rigts_3 so that at least
 * one bit is 1 (to mark correct page entry, see #PAGE_VALID_ARCH).
 *
 * @param pt    Level 1 page table.
 * @param i     Index of the entry to be changed.
 * @param flags New flags.
 *
 */
NO_TRACE static inline void set_pt_level1_flags(pte_t *pt, size_t i, int flags)
{
	pte_level1_t *p = &pt[i].l1;
	
	if (flags & PAGE_NOT_PRESENT) {
		p->descriptor_type = PTE_DESCRIPTOR_NOT_PRESENT;
	} else {
		if (flags & PAGE_EXEC)
			p->descriptor_type = PTE_DESCRIPTOR_SMALL_PAGE;
		else
			p->descriptor_type = PTE_DESCRIPTOR_SMALL_PAGE_NX;
	}
	
	/* tex=0 buf=1 and cache=1 => normal memory
	 * tex=0 buf=1 and cache=0 => shareable device mmio
	 */
	p->cacheable = (flags & PAGE_CACHEABLE);
	p->bufferable = 1;
	p->tex = 0;
	
	/* Shareable is ignored for devices (non-cacheable),
	 * turn it on for normal memory. */
	p->shareable = 1;
	
	p->non_global = !(flags & PAGE_GLOBAL);
	
	/* default access permission: kernel only*/
	p->access_permission_0 = PTE_AP0_USER_NO_KERNEL_FULL;
	
	if (flags & PAGE_USER) {
		p->access_permission_0 = PTE_AP0_USER_FULL_KERNEL_FULL;
		// TODO Fix kernel to use PAGE_WRITE flag properly and
		// apply this for kernel pages as well.
		if (!(flags & PAGE_WRITE))
			p->access_permission_1 = PTE_AP1_RO;
	}
}

NO_TRACE static inline void set_pt_level0_present(pte_t *pt, size_t i)
{
	pte_level0_t *p = &pt[i].l0;

	p->should_be_zero_0 = 0;
	p->should_be_zero_1 = 0;
	write_barrier();
	p->descriptor_type = PTE_DESCRIPTOR_COARSE_TABLE;
}

NO_TRACE static inline void set_pt_level1_present(pte_t *pt, size_t i)
{
	pte_level1_t *p = &pt[i].l1;

	p->descriptor_type = PTE_DESCRIPTOR_SMALL_PAGE;
}


extern void page_arch_init(void);

#endif /* __ASM__ */

#endif

/** @}
 */