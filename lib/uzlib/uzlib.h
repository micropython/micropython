/*
 * uzlib  -  tiny deflate/inflate library (deflate, gzip, zlib)
 *
 * Copyright (c) 2003 by Joergen Ibsen / Jibz
 * All Rights Reserved
 * http://www.ibsensoftware.com/
 *
 * Copyright (c) 2014-2018 by Paul Sokolovsky
 *
 * Optimised for MicroPython:
 * Copyright (c) 2023 by Jim Mussared
 *
 * This software is provided 'as-is', without any express
 * or implied warranty.  In no event will the authors be
 * held liable for any damages arising from the use of
 * this software.
 *
 * Permission is granted to anyone to use this software
 * for any purpose, including commercial applications,
 * and to alter it and redistribute it freely, subject to
 * the following restrictions:
 *
 * 1. The origin of this software must not be
 *    misrepresented; you must not claim that you
 *    wrote the original software. If you use this
 *    software in a product, an acknowledgment in
 *    the product documentation would be appreciated
 *    but is not required.
 *
 * 2. Altered source versions must be plainly marked
 *    as such, and must not be misrepresented as
 *    being the original software.
 *
 * 3. This notice may not be removed or altered from
 *    any source distribution.
 */

#ifndef UZLIB_H_INCLUDED
#define UZLIB_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "uzlib_conf.h"
#if UZLIB_CONF_DEBUG_LOG
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ok status, more data produced */
#define UZLIB_OK             0
/* end of compressed stream reached */
#define UZLIB_DONE           1
#define UZLIB_DATA_ERROR    (-3)
#define UZLIB_CHKSUM_ERROR  (-4)
#define UZLIB_DICT_ERROR    (-5)

/* checksum types */
#define UZLIB_CHKSUM_NONE  0
#define UZLIB_CHKSUM_ADLER 1
#define UZLIB_CHKSUM_CRC   2

/* helper macros */
#define TINF_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))

/* data structures */

typedef struct {
   unsigned short table[16];  /* table of code length counts */
   unsigned short trans[288]; /* code -> symbol translation table */
} TINF_TREE;

typedef struct _uzlib_uncomp_t {
    /* Pointer to the next byte in the input buffer */
    const unsigned char *source;
    /* Pointer to the next byte past the input buffer (source_limit = source + len) */
    const unsigned char *source_limit;
    /* If source_limit == NULL, or source >= source_limit, this function
       will be used to read next byte from source stream. The function may
       also return -1 in case of EOF (or irrecoverable error). Note that
       besides returning the next byte, it may also update source and
       source_limit fields, thus allowing for buffered operation. */
    void *source_read_data;
    int (*source_read_cb)(void *);

    unsigned int tag;
    unsigned int bitcount;

    /* Destination (output) buffer start */
    unsigned char *dest_start;
    /* Current pointer in dest buffer */
    unsigned char *dest;
    /* Pointer past the end of the dest buffer, similar to source_limit */
    unsigned char *dest_limit;

    /* Accumulating checksum */
    unsigned int checksum;
    char checksum_type;
    bool eof;

    int btype;
    int bfinal;
    unsigned int curlen;
    int lzOff;
    unsigned char *dict_ring;
    unsigned int dict_size;
    unsigned int dict_idx;

    TINF_TREE ltree; /* dynamic length/symbol tree */
    TINF_TREE dtree; /* dynamic distance tree */
} uzlib_uncomp_t;

#define TINF_PUT(d, c) \
    { \
        *d->dest++ = c; \
        if (d->dict_ring) { d->dict_ring[d->dict_idx++] = c; if (d->dict_idx == d->dict_size) d->dict_idx = 0; } \
    }

unsigned char uzlib_get_byte(uzlib_uncomp_t *d);

/* Decompression API */

void uzlib_uncompress_init(uzlib_uncomp_t *d, void *dict, unsigned int dictLen);
int  uzlib_uncompress(uzlib_uncomp_t *d);
int  uzlib_uncompress_chksum(uzlib_uncomp_t *d);

#define UZLIB_HEADER_ZLIB             0
#define UZLIB_HEADER_GZIP             1
int uzlib_parse_zlib_gzip_header(uzlib_uncomp_t *d, int *wbits);

/* Compression API */

typedef struct {
    void *dest_write_data;
    void (*dest_write_cb)(void *data, uint8_t byte);
    unsigned long outbits;
    int noutbits;
    uint8_t *hist_buf;
    size_t hist_max;
    size_t hist_start;
    size_t hist_len;
} uzlib_lz77_state_t;

void uzlib_lz77_init(uzlib_lz77_state_t *state, uint8_t *hist, size_t hist_max);
void uzlib_lz77_compress(uzlib_lz77_state_t *state, const uint8_t *src, unsigned len);

void uzlib_start_block(uzlib_lz77_state_t *state);
void uzlib_finish_block(uzlib_lz77_state_t *state);

/* Checksum API */

/* prev_sum is previous value for incremental computation, 1 initially */
uint32_t uzlib_adler32(const void *data, unsigned int length, uint32_t prev_sum);
/* crc is previous value for incremental computation, 0xffffffff initially */
uint32_t uzlib_crc32(const void *data, unsigned int length, uint32_t crc);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UZLIB_H_INCLUDED */
