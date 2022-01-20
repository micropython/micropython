/*
 * uzlib  -  tiny deflate/inflate library (deflate, gzip, zlib)
 *
 * Copyright (c) 2003 by Joergen Ibsen / Jibz
 * All Rights Reserved
 * http://www.ibsensoftware.com/
 *
 * Copyright (c) 2014-2018 by Paul Sokolovsky
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

#ifdef __cplusplus
extern "C" {
#endif

#include "uzlib_conf.h"
#if UZLIB_CONF_DEBUG_LOG
#include <stdio.h>
#endif

/* calling convention */
#ifndef TINFCC
 #ifdef __WATCOMC__
  #define TINFCC __cdecl
 #else
  #define TINFCC
 #endif
#endif

/* ok status, more data produced */
#define TINF_OK             0
/* end of compressed stream reached */
#define TINF_DONE           1
#define TINF_DATA_ERROR    (-3)
#define TINF_CHKSUM_ERROR  (-4)
#define TINF_DICT_ERROR    (-5)

/* checksum types */
#define TINF_CHKSUM_NONE  0
#define TINF_CHKSUM_ADLER 1
#define TINF_CHKSUM_CRC   2

/* helper macros */
#define TINF_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))

/* data structures */

typedef struct {
   unsigned short table[16];  /* table of code length counts */
   unsigned short trans[288]; /* code -> symbol translation table */
} TINF_TREE;

struct uzlib_uncomp {
    /* Pointer to the next byte in the input buffer */
    const unsigned char *source;
    /* Pointer to the next byte past the input buffer (source_limit = source + len) */
    const unsigned char *source_limit;
    /* If source_limit == NULL, or source >= source_limit, this function
       will be used to read next byte from source stream. The function may
       also return -1 in case of EOF (or irrecoverable error). Note that
       besides returning the next byte, it may also update source and
       source_limit fields, thus allowing for buffered operation. */
    int (*source_read_cb)(struct uzlib_uncomp *uncomp);

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
};

#include "tinf_compat.h"

#define TINF_PUT(d, c) \
    { \
        *d->dest++ = c; \
        if (d->dict_ring) { d->dict_ring[d->dict_idx++] = c; if (d->dict_idx == d->dict_size) d->dict_idx = 0; } \
    }

unsigned char TINFCC uzlib_get_byte(TINF_DATA *d);

/* Decompression API */

void TINFCC uzlib_init(void);
void TINFCC uzlib_uncompress_init(TINF_DATA *d, void *dict, unsigned int dictLen);
int  TINFCC uzlib_uncompress(TINF_DATA *d);
int  TINFCC uzlib_uncompress_chksum(TINF_DATA *d);

int TINFCC uzlib_zlib_parse_header(TINF_DATA *d);
int TINFCC uzlib_gzip_parse_header(TINF_DATA *d);

/* Compression API */

typedef const uint8_t *uzlib_hash_entry_t;

struct uzlib_comp {
    unsigned char *outbuf;
    int outlen, outsize;
    unsigned long outbits;
    int noutbits;
    int comp_disabled;

    uzlib_hash_entry_t *hash_table;
    unsigned int hash_bits;
    unsigned int dict_size;
};

void TINFCC uzlib_compress(struct uzlib_comp *c, const uint8_t *src, unsigned slen);

#include "defl_static.h"

/* Checksum API */

/* prev_sum is previous value for incremental computation, 1 initially */
uint32_t TINFCC uzlib_adler32(const void *data, unsigned int length, uint32_t prev_sum);
/* crc is previous value for incremental computation, 0xffffffff initially */
uint32_t TINFCC uzlib_crc32(const void *data, unsigned int length, uint32_t crc);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UZLIB_H_INCLUDED */
