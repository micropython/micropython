/****************************************************************************
 * tools/cxd56/mkspk.c
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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "mkspk.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct args
{
    int core;
    char *elffile;
    char *savename;
    char *outputfile;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint8_t vmk[16] =
    "\x27\xc0\xaf\x1b\x5d\xcb\xc6\xc5\x58\x22\x1c\xdd\xaf\xf3\x20\x21";

static struct args g_options =
{
    0
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static struct args *parse_args(int argc, char **argv) {
    int opt;
    int show_help;
    struct args *args = &g_options;
    char *endp;

    show_help = 0;

    if (argc < 2) {
        show_help = 1;
    }

    memset(args, 0, sizeof(*args));
    args->core = -1;

    while ((opt = getopt(argc, argv, "h:c:")) != -1) {
        switch (opt)
        {
            case 'c':
                args->core = strtol(optarg, &endp, 0);
                if (*endp) {
                    fprintf(stderr, "Invalid core number \"%s\"\n", optarg);
                    show_help = 1;
                }
                break;

            case 'h':
            default:
                show_help = 1;
        }
    }

    argc -= optind;
    argv += optind;

    args->elffile = argv[0];
    args->savename = argv[1];
    argc -= 2;
    argv += 2;

    if (argc > 0) {
        args->outputfile = strdup(argv[0]);
    } else {
        show_help = 1;
    }

    /* Sanity checks for options */

    if (show_help == 1) {
        fprintf(stderr,
            "mkspk [-c <number>] <filename> <save name> [<output file>]\n");
        exit(EXIT_FAILURE);
    }

    if (args->core < 0) {
        fprintf(stderr, "Core number is not set. Please use -c option.\n");
        exit(EXIT_FAILURE);
    }

    if (strlen(args->savename) > 63) {
        fprintf(stderr, "savename too long.\n");
        exit(EXIT_FAILURE);
    }

    return args;
}

static struct elf_file *load_elf(const char *filename) {
    size_t fsize;
    int pos;
    char *buf;
    FILE *fp;
    struct elf_file *ef;
    Elf32_Shdr *sh;
    uint16_t i;
    int ret;

    fp = fopen(filename, "rb");
    if (!fp) {
        return NULL;
    }

    ef = (struct elf_file *)malloc(sizeof(*ef));
    if (!ef) {
        return NULL;
    }

    pos = fseek(fp, 0, SEEK_END);
    fsize = (size_t)ftell(fp);
    fseek(fp, pos, SEEK_SET);

    buf = (char *)malloc(fsize);
    if (!buf) {
        return NULL;
    }

    ret = fread(buf, fsize, 1, fp);
    fclose(fp);
    if (ret != 1) {
        return NULL;
    }

    ef->data = buf;

    ef->ehdr = (Elf32_Ehdr *)buf;

    Elf32_Ehdr *h = (Elf32_Ehdr *)buf;

    if (!(h->e_ident[EI_MAG0] == 0x7f &&
          h->e_ident[EI_MAG1] == 'E' &&
          h->e_ident[EI_MAG2] == 'L' && h->e_ident[EI_MAG3] == 'F')) {
        free(ef);
        free(buf);
        return NULL;
    }

    ef->phdr = (Elf32_Phdr *)(buf + ef->ehdr->e_phoff);
    ef->shdr = (Elf32_Shdr *)(buf + ef->ehdr->e_shoff);
    ef->shstring = buf + ef->shdr[ef->ehdr->e_shstrndx].sh_offset;

    for (i = 0, sh = ef->shdr; i < ef->ehdr->e_shnum; i++, sh++)
    {
        if (sh->sh_type == SHT_SYMTAB) {
            ef->symtab = (Elf32_Sym *)(buf + sh->sh_offset);
            ef->nsyms = sh->sh_size / sh->sh_entsize;
            continue;
        }

        if (sh->sh_type == SHT_STRTAB) {
            if (!strcmp(".strtab", ef->shstring + sh->sh_name)) {
                ef->string = buf + sh->sh_offset;
            }
        }
    }

    return ef;
}

static void *create_image(struct elf_file *elf, int core, char *savename,
    int *image_size) {
    char *img;
    struct spk_header *header;
    struct spk_prog_info *pi;
    Elf32_Phdr *ph;
    Elf32_Sym *sym;
    char *name;
    int snlen;
    int nphs, psize, imgsize;
    int i;
    int j;
    uint32_t offset;
    uint32_t sp;

    snlen = alignup(strlen(savename) + 1, 16);

    nphs = 0;
    psize = 0;
    for (i = 0, ph = elf->phdr; i < elf->ehdr->e_phnum; i++, ph++)
    {
        if (ph->p_type != PT_LOAD || ph->p_filesz == 0) {
            continue;
        }

        nphs++;
        psize += alignup(ph->p_filesz, 16);
    }

    imgsize = sizeof(*header) + snlen + (nphs * 16) + psize;

    img = (char *)malloc(imgsize + 32);
    if (!img) {
        return NULL;
    }

    *image_size = imgsize;
    sym = elf->symtab;
    name = elf->string;
    sp = 0;

    for (j = 0; j < elf->nsyms; j++, sym++)
    {
        if (!strcmp("__stack", name + sym->st_name)) {
            sp = sym->st_value;
        }
    }

    memset(img, 0, imgsize);

    header = (struct spk_header *)img;
    header->magic[0] = 0xef;
    header->magic[1] = 'M';
    header->magic[2] = 'O';
    header->magic[3] = 'D';
    header->cpu = core;

    header->entry = elf->ehdr->e_entry;
    header->stack = sp;
    header->core = core;

    header->binaries = nphs;
    header->phoffs = sizeof(*header) + snlen;
    header->mode = 0777;

    strncpy(img + sizeof(*header), savename, 63);

    ph = elf->phdr;
    pi = (struct spk_prog_info *)(img + header->phoffs);
    offset = ((char *)pi - img) + (nphs * sizeof(*pi));
    for (i = 0; i < elf->ehdr->e_phnum; i++, ph++)
    {
        if (ph->p_type != PT_LOAD || ph->p_filesz == 0) {
            continue;
        }
        pi->load_address = ph->p_paddr;
        pi->offset = offset;
        pi->size = alignup(ph->p_filesz, 16);   /* need 16 bytes align for
                                                 * decryption */
        pi->memsize = ph->p_memsz;

        memcpy(img + pi->offset, elf->data + ph->p_offset, ph->p_filesz);

        offset += alignup(ph->p_filesz, 16);
        pi++;
    }

    return img;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char **argv) {
    struct args *args;
    struct elf_file *elf;
    struct cipher *c;
    uint8_t *spkimage;
    int size = 0;
    FILE *fp;
    char footer[16];

    args = parse_args(argc, argv);

    elf = load_elf(args->elffile);
    if (!elf) {
        fprintf(stderr, "Loading ELF %s failure.\n", args->elffile);
        exit(EXIT_FAILURE);
    }

    spkimage = create_image(elf, args->core, args->savename, &size);
    free(elf);

    c = cipher_init(vmk, NULL);
    cipher_calc_cmac(c, spkimage, size, (uint8_t *)spkimage + size);
    cipher_deinit(c);

    size += 16;                 /* Extend CMAC size */

    snprintf(footer, 16, "MKSPK_BN_HOOTER");
    footer[15] = '\0';

    fp = fopen(args->outputfile, "wb");
    if (!fp) {
        fprintf(stderr, "Output file open error.\n");
        free(spkimage);
        exit(EXIT_FAILURE);
    }

    fwrite(spkimage, size, 1, fp);
    fwrite(footer, 16, 1, fp);

    fclose(fp);

    printf("File %s is successfully created.\n", args->outputfile);
    free(args->outputfile);

    memset(spkimage, 0, size);
    free(spkimage);

    exit(EXIT_SUCCESS);
}
