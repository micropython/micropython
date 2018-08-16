/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Owen Kirby
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#if MICROPY_FLASH_TARBALL

#include <stdio.h>
#include <string.h>

#include "py/mpconfig.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#if MICROPY_PY_UZLIB
#include "extmod/uzlib/tinf.h"
#endif

#include "storage.h"

static const uint8_t flash_tarball_data[] = {
#include "genhdr/flash_tarball.h"
};
static const size_t flash_tarball_size = sizeof(flash_tarball_data);

#define TAR_BLOCK_SIZE          512

/* Link formats. */
#define TAR_LINK_NORMAL         '0'
#define TAR_LINK_HARD           '1'
#define TAR_LINK_SYMBOLIC       '2'
#define TAR_LINK_CHAR_SPECIAL   '3'
#define TAR_LINK_BLOCK_SPECIAL  '4'
#define TAR_LINK_DIRECTORY      '5'
#define TAR_LINK_FIFO           '6'
#define TAR_LINK_CONTIGUOUS     '7'
#define TAR_LINK_GLOBAL_EXT     'g'
#define TAR_LINK_META_EXT       'x'

/* TAR Header format. */
struct tar_header {
    /* Pre-POSIX.1-1988 TAR file header. */
    char    filename[100];
    char    mode[8];
    char    uid[8];
    char    gid[8];
    char    filesize[12];
    char    modtime[12];
    char    chksum[8];
    char    type;
    char    link[100];
    /* POSIX IEEE P1003.1 - USTar header */
    char    indicator[6];   /* "ustar" + NULL */
    char    version[2];
    char    user[32];
    char    group[32];
    char    major[8];
    char    minor[8];
    char    fileprefix[155];
};

#define TAR_IS_USTAR(_hdr_) \
    (strncmp(((const struct tar_header *)_hdr_)->indicator, "ustar", 6) == 0)

/* Return whether the block is zero-filled. Two of which in a row indicates the end of file. */
static int
tar_is_eof(const void *ptr)
{
    const uint8_t *b = ptr;
    unsigned int sum = 0;
    int i;
    for (i = 0; i < TAR_BLOCK_SIZE; i++) {
        sum += b[i];
    }
    return (sum == 0);
} /* tar_is_eof */

/* Parse an octal length, while limited to a given number of characters. */
static unsigned long
tar_strtoul(const char *s, size_t maxlen)
{
    unsigned long x = 0;
    int i;
    for (i = 0; i < maxlen; i++) {
        char c = s[i];
        if ((c < '0') || (c >= '8')) break;
        x <<= 3;
        x += (c - '0');
    }
    return x;
} /* tar_stroul */

/* Processing state for TAR extraction. */
struct untar_state {
    FIL fp;
    unsigned long fsize;
    unsigned int numeof;
    uint8_t buf[TAR_BLOCK_SIZE];
};

static int
untar_process_block(fs_user_mount_t *vfs, struct untar_state *state)
{
    struct tar_header *hdr = (struct tar_header *)state->buf;
    UINT nbytes;

    /* Write data to files. */
    if (state->fsize > TAR_BLOCK_SIZE) {
        f_write(&state->fp, state->buf, TAR_BLOCK_SIZE, &nbytes);
        state->fsize -= TAR_BLOCK_SIZE;
    }
    else if (state->fsize > 0) {
        f_write(&state->fp, state->buf, state->fsize, &nbytes);
        f_close(&state->fp);
        state->fsize = 0;
    }
    /* Perform EOF detection. */
    else if (tar_is_eof(state->buf)) {
        state->fsize = 0;
        state->numeof++;
        if (state->numeof >= 2) {
            return false;
        }
    }
    /* Start a new file. */
    else if ((hdr->type == TAR_LINK_NORMAL) || (hdr->type == 0)) {
        f_open(&vfs->fatfs, &state->fp, hdr->filename, FA_WRITE | FA_CREATE_ALWAYS);
        state->fsize = tar_strtoul(hdr->filesize, sizeof(hdr->filesize));
        state->numeof = 0;
    }
    /* Create directories. */
    else if (hdr->type == TAR_LINK_DIRECTORY) {
        size_t namelen = strlen(hdr->filename);
        if (namelen && (hdr->filename[namelen-1] == '/')) {
            hdr->filename[--namelen] = '\0';
        }
        f_mkdir(&vfs->fatfs, hdr->filename);
        state->fsize = 0;
        state->numeof = 0;
    }

    /* Handle more blocks */
    return true;
}

void
pyb_flash_untar_vfs(fs_user_mount_t *vfs)
{
    struct untar_state state;
    memset(&state, 0, sizeof(state));

    /* GZIP-compressed TAR */
    if ((flash_tarball_data[0] == 0x1f) && (flash_tarball_data[1] == 0x8b)) {
#if MICROPY_PY_UZLIB
        TINF_DATA decomp;
        int dict_sz = 1<<15;
        uint8_t *dict;
        memset(&decomp, 0, sizeof(decomp));
        decomp.source = flash_tarball_data;

        uzlib_init();
        if (uzlib_gzip_parse_header(&decomp) != TINF_OK) {
            printf("PYB: can't parse gzip header\n");
            return;
        }

        dict = malloc(dict_sz);
        if (!dict) {
            printf("PYB: can't allocate gzip dictionary\n");
            return;
        }
        uzlib_uncompress_init(&decomp, dict, dict_sz);

        /* Process blocks of data from the tar file. */
        do {
            /* Decode a block */
            decomp.dest = state.buf;
            decomp.destSize = TAR_BLOCK_SIZE;
            uzlib_uncompress(&decomp);

            /* Check for overflows. */
            if (decomp.source >= &flash_tarball_data[flash_tarball_size]) {
                break;
            }
        } while (untar_process_block(vfs, &state));
        free(dict);
#endif
    }
    /* Uncompressed TAR */
    else {
        size_t offset;
        for (offset = 0; offset < flash_tarball_size; offset += TAR_BLOCK_SIZE) {
            memcpy(state.buf, &flash_tarball_data[flash_tarball_size], TAR_BLOCK_SIZE);
            if (!untar_process_block(vfs, &state)) break;
        }
    }

    if (state.fsize) {
        f_close(&state.fp);
    }
}
#endif
