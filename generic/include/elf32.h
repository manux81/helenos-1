/*
 * Copyright (C) 2006 Sergey Bondari
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

#ifndef __ELF32_H__
#define __ELF32_H__

#include <arch/types.h>
#include <mm/as.h>

/**
 * current ELF version
 */
#define	EV_CURRENT	1

/** 
 * ELF types 
 */
#define ET_NONE		0	// No type
#define ET_REL		1	// Relocatable file
#define ET_EXEC		2	// Executable
#define ET_DYN		3	// Shared object
#define ET_CORE		4	// Core
#define ET_LOPROC	0xff00	// Processor specific
#define ET_HIPROC	0xffff	// Processor specific

/** 
 * ELF machine types
 */
#define EM_NO		0	// No machine
#define EM_SPARC	2	// SPARC
#define EM_386		3	// i386
#define EM_MIPS		8	// MIPS RS3000
#define EM_MIPS_RS3_LE	10	// MIPS RS3000 LE
#define EM_PPC		20	// PPC32
#define EM_PPC64	21	// PPC64
#define EM_SPARCV9	43	// SPARC64
#define EM_IA_64	50	// IA-64
#define EM_X86_64	62	// AMD64/EMT64

/**
 * ELF identification indexes
 */
#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define EI_CLASS	4	// File class
#define EI_DATA		5	// Data encoding
#define EI_VERSION	6	// File version
#define EI_PAD		7	// Start of padding bytes
#define EI_NIDENT	16	// ELF identification table size

/**
 * ELF magic number
 */
#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

/**
 * ELF file classes
 */
#define ELFCLASSNONE	0
#define ELFCLASS32	1
#define ELFCLASS64	2

/**
 * ELF data encoding types
 */
#define ELFDATANONE	0
#define ELFDATA2LSB	1	// Least significant byte first (little endian)
#define ELFDATA2MSB	2	// Most signigicant byte first (Big endian)

/**
 * ELF error return codes
 */
#define EE_OK			0	// No error
#define EE_INVALID		1	// invalid ELF image
#define	EE_MEMORY		2	// cannot allocate address space
#define EE_INCOMPATIBLE		3	// ELF image is not compatible with current architecture
#define EE_UNSUPPORTED		4	// Non-supported ELF (e.g. dynamic ELFs)


/**
 * ELF section types
 */
#define SHT_NULL		0
#define SHT_PROGBITS		1
#define SHT_SYMTAB		2
#define SHT_STRTAB		3
#define SHT_RELA		4
#define SHT_HASH		5
#define SHT_DYNAMIC		6
#define SHT_NOTE		7
#define SHT_NOBITS		8
#define SHT_REL			9
#define SHT_SHLIB		10
#define SHT_DYNSYM		11
#define SHT_LOPROC		0x70000000
#define SHT_HIPROC		0x7fffffff
#define SHT_LOUSER		0x80000000
#define SHT_HIUSER		0xffffffff

/**
 * ELF section flags
 */
#define SHF_WRITE		0x1 
#define SHF_ALLOC		0x2
#define SHF_EXECINSTR		0x4
#define SHF_MASKPROC		0xf0000000


/**
 * Symbol binding
 */
#define STB_LOCAL		0
#define STB_GLOBAL		1
#define STB_WEAK		2
#define STB_LOPROC		13
#define STB_HIPROC		15

/**
 * Symbol types
 */
#define STT_NOTYPE		0
#define STT_OBJECT		1
#define STT_FUNC		2
#define STT_SECTION		3
#define STT_FILE		4
#define STT_LOPROC		13
#define STT_HIPROC		15


/**
 * Program segment types
 */
#define PT_NULL			0
#define PT_LOAD			1
#define PT_DYNAMIC		2
#define PT_INTERP		3
#define PT_NOTE			4
#define PT_SHLIB		5
#define PT_PHDR			6
#define PT_LOPROC		0x70000000
#define PT_HIPROC		0x7fffffff

/**
 * 32-bit ELF data types
 */
typedef __u32 elf32_addr;
typedef __u16 elf32_half;
typedef __u32 elf32_off;
typedef int elf32_sword;
typedef __u32 elf32_word;

/**
 * 32-bit ELF header
 */
struct elf32_header {
	__u8 e_ident[EI_NIDENT];
	elf32_half e_type;
	elf32_half e_machine;
	elf32_word e_version;
	elf32_addr e_entry;
	elf32_off e_phoff;
	elf32_off e_shoff;
	elf32_word e_flags;
	elf32_half e_ehsize;
	elf32_half e_phentsize;
	elf32_half e_phnum;
	elf32_half e_shentsize;
	elf32_half e_shnum;
	elf32_half e_shstrndx;
};


/*
 * 32-bit ELF section header
 */
struct elf32_section_header {
	elf32_word sh_name;
	elf32_word sh_type;
	elf32_word sh_flags;
	elf32_addr sh_addr;
	elf32_off sh_offset;
	elf32_word sh_size;
	elf32_word sh_link;
	elf32_word sh_info;
	elf32_word sh_addralign;
	elf32_word sh_entsize;
};


/*
 * 32-bit ELF symbol table entry
 */
struct elf32_symbol {
	elf32_word st_name;
	elf32_addr st_value;
	elf32_word st_size;
	__u8 st_info;
	__u8 st_other;
	elf32_half st_shndx;
};


/*
 * 32-bit ELF program header
 */
struct elf32_program_header {
	elf32_word p_type;
	elf32_off p_offset;
	elf32_addr pv_addr;
	elf32_addr pp_addr;
	elf32_word p_filesz;
	elf32_word p_memsz;
	elf32_word p_flags;
	elf32_word p_align;
};
 
typedef struct elf32_header elf32_header_t;
typedef struct elf32_section_header elf32_section_header_t;
typedef struct elf32_symbol elf32_symbol_t;
typedef struct elf32_program_header elf32_program_header_t;



extern int elf32_load(__address header, as_t * as);

#endif
