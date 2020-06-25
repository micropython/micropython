/****************************************************************************
 * include/elf32.h
 *
 *   Copyright (C) 2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Reference: System V Application Binary Interface, Edition 4.1, March 18,
 * 1997, The Santa Cruz Operation, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __INCLUDE_ELF32_H
#define __INCLUDE_ELF32_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define EI_NIDENT          16     /* Size of e_ident[] */

#define ELF32_ST_BIND(i)   ((i) >> 4)
#define ELF32_ST_TYPE(i)   ((i) & 0xf)
#define ELF32_ST_INFO(b,t) (((b) << 4) | ((t) & 0xf))

/* Definitions for Elf32_Rel*::r_info */

#define ELF32_R_SYM(i)    ((i) >> 8)
#define ELF32_R_TYPE(i)   ((i) & 0xff)
#define ELF32_R_INFO(s,t) (((s)<< 8) | ((t) & 0xff))

#define ELF_R_SYM(i)      ELF32_R_SYM(i)

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/* Figure 4.2: 32-Bit Data Types */

typedef uint32_t  Elf32_Addr;  /* Unsigned program address */
typedef uint16_t  Elf32_Half;  /* Unsigned medium integer */
typedef uint32_t  Elf32_Off;   /* Unsigned file offset */
typedef int32_t   Elf32_Sword; /* Signed large integer */
typedef uint32_t  Elf32_Word;  /* Unsigned large integer */

/* Figure 4-3: ELF Header */

typedef struct
{
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half    e_type;
  Elf32_Half    e_machine;
  Elf32_Word    e_version;
  Elf32_Addr    e_entry;
  Elf32_Off     e_phoff;
  Elf32_Off     e_shoff;
  Elf32_Word    e_flags;
  Elf32_Half    e_ehsize;
  Elf32_Half    e_phentsize;
  Elf32_Half    e_phnum;
  Elf32_Half    e_shentsize;
  Elf32_Half    e_shnum;
  Elf32_Half    e_shstrndx;
} Elf32_Ehdr;

/* Figure 4-8: Section Header */

typedef struct
{
  Elf32_Word    sh_name;
  Elf32_Word    sh_type;
  Elf32_Word    sh_flags;
  Elf32_Addr    sh_addr;
  Elf32_Off     sh_offset;
  Elf32_Word    sh_size;
  Elf32_Word    sh_link;
  Elf32_Word    sh_info;
  Elf32_Word    sh_addralign;
  Elf32_Word    sh_entsize;
} Elf32_Shdr;

/* Figure 4-15: Symbol Table Entry */

typedef struct
{
  Elf32_Word    st_name;
  Elf32_Addr    st_value;
  Elf32_Word    st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf32_Half    st_shndx;
} Elf32_Sym;

/* Figure 4-19: Relocation Entries */

typedef struct
{
  Elf32_Addr   r_offset;
  Elf32_Word   r_info;
} Elf32_Rel;

typedef struct
{
  Elf32_Addr   r_offset;
  Elf32_Word   r_info;
  Elf32_Sword  r_addend;
} Elf32_Rela;

/* Figure 5-1: Program Header */

typedef struct
{
  Elf32_Word   p_type;
  Elf32_Off    p_offset;
  Elf32_Addr   p_vaddr;
  Elf32_Addr   p_paddr;
  Elf32_Word   p_filesz;
  Elf32_Word   p_memsz;
  Elf32_Word   p_flags;
  Elf32_Word   p_align;
} Elf32_Phdr;

/* Figure 5-9: Dynamic Structure */

typedef struct
{
  Elf32_Sword  d_tag;
  union
  {
    Elf32_Word d_val;
    Elf32_Addr d_ptr;
  } d_un;
} Elf32_Dyn;

typedef Elf32_Addr  Elf_Addr;
typedef Elf32_Ehdr  Elf_Ehdr;
typedef Elf32_Rel   Elf_Rel;
typedef Elf32_Rela  Elf_Rela;
typedef Elf32_Sym   Elf_Sym;
typedef Elf32_Shdr  Elf_Shdr;
typedef Elf32_Word  Elf_Word;

#endif /* __INCLUDE_ELF32_H */
