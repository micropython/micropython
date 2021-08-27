/****************************************************************************
 * tools/cxd56/mkspk.h
 *
 * Copyright (C) 2007, 2008 Sony Corporation
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
 *****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "clefia.h"
#include "elf32.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define EI_MAG0            0      /* File identification */
#define EI_MAG1            1
#define EI_MAG2            2
#define EI_MAG3            3

#define SHT_SYMTAB         2
#define SHT_STRTAB         3

#define PT_LOAD            1

#define alignup(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define swap(a, b) { (a) ^= (b); (b) ^= (a); (a) ^= (b); }

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct spk_header
{
    uint8_t magic[4];
    uint8_t cpu;
    uint8_t reserved[11];
    uint32_t entry;
    uint32_t stack;
    uint16_t core;
    uint16_t binaries;
    uint16_t phoffs;
    uint16_t mode;
};

struct spk_prog_info
{
    uint32_t load_address;
    uint32_t offset;
    uint32_t size;
    uint32_t memsize;
};

struct elf_file
{
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;
    Elf32_Shdr *shdr;
    Elf32_Sym *symtab;
    int nsyms;
    char *shstring;
    char *string;
    char *data;
};
