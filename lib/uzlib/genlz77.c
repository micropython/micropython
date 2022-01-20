/*
 * genlz77  -  Generic LZ77 compressor
 *
 * Copyright (c) 2014 by Paul Sokolovsky
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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "uzlib.h"

#if 0
#define HASH_BITS 12
#else
#define HASH_BITS data->hash_bits
#endif

#define HASH_SIZE (1<<HASH_BITS)

/* Minimum and maximum length of matches to look for, inclusive */
#define MIN_MATCH 3
#define MAX_MATCH 258

/* Max offset of the match to look for, inclusive */
#if 0
#define MAX_OFFSET 32768
#else
#define MAX_OFFSET data->dict_size
#endif

/* Hash function can be defined as macro or as inline function */

/*#define HASH(p) (p[0] + p[1] + p[2])*/

/* This is hash function from liblzf */
static inline int HASH(struct uzlib_comp *data, const uint8_t *p) {
    int v = (p[0] << 16) | (p[1] << 8) | p[2];
    int hash = ((v >> (3*8 - HASH_BITS)) - v) & (HASH_SIZE - 1);
    return hash;
}

#ifdef DUMP_LZTXT

/* Counter for approximate compressed length in LZTXT mode. */
/* Literal is counted as 1, copy as 2 bytes. */
unsigned approx_compressed_len;

void literal(void *data, uint8_t val)
{
    printf("L%02x # %c\n", val, (val >= 0x20 && val <= 0x7e) ? val : '?');
    approx_compressed_len++;
}

void copy(void *data, unsigned offset, unsigned len)
{
    printf("C-%u,%u\n", offset, len);
    approx_compressed_len += 2;
}

#else

static inline void literal(void *data, uint8_t val)
{
    zlib_literal(data, val);
}

static inline void copy(void *data, unsigned offset, unsigned len)
{
    zlib_match(data, offset, len);
}

#endif


void uzlib_compress(struct uzlib_comp *data, const uint8_t *src, unsigned slen)
{
    const uint8_t *top = src + slen - MIN_MATCH;
    while (src < top) {
        int h = HASH(data, src);
        const uint8_t **bucket = &data->hash_table[h & (HASH_SIZE - 1)];
        const uint8_t *subs = *bucket;
        *bucket = src;
        if (subs && src > subs && (src - subs) <= MAX_OFFSET && !memcmp(src, subs, MIN_MATCH)) {
            src += MIN_MATCH;
            const uint8_t *m = subs + MIN_MATCH;
            int len = MIN_MATCH;
            while (*src == *m && len < MAX_MATCH && src < top) {
                src++; m++; len++;
            }
            copy(data, src - len - subs, len);
        } else {
            literal(data, *src++);
        }
    }
    // Process buffer tail, which is less than MIN_MATCH
    // (and so it doesn't make sense to look for matches there)
    top += MIN_MATCH;
    while (src < top) {
        literal(data, *src++);
    }
}
