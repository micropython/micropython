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

#include <assert.h>
#include "uzlib.h"

#define UZLIB_DUMP_ARRAY(heading, arr, size) \
    { \
        printf("%s", heading); \
        for (int i = 0; i < size; ++i) { \
            printf(" %d", (arr)[i]); \
        } \
        printf("\n"); \
    }

uint32_t tinf_get_le_uint32(uzlib_uncomp_t *d);
uint32_t tinf_get_be_uint32(uzlib_uncomp_t *d);

/* --------------------------------------------------- *
 * -- uninitialized global data (static structures) -- *
 * --------------------------------------------------- */

typedef struct {
    unsigned char length_bits : 3;
    unsigned short length_base : 9;
    unsigned char dist_bits : 4;
    unsigned short dist_base : 15;
} lookup_table_entry_t;

const lookup_table_entry_t lookup_table[30] = {
    {0, 3, 0, 1},
    {0, 4, 0, 2},
    {0, 5, 0, 3},
    {0, 6, 0, 4},
    {0, 7, 1, 5},
    {0, 8, 1, 7},
    {0, 9, 2, 9},
    {0, 10, 2, 13},
    {1, 11, 3, 17},
    {1, 13, 3, 25},
    {1, 15, 4, 33},
    {1, 17, 4, 49},
    {2, 19, 5, 65},
    {2, 23, 5, 97},
    {2, 27, 6, 129},
    {2, 31, 6, 193},
    {3, 35, 7, 257},
    {3, 43, 7, 385},
    {3, 51, 8, 513},
    {3, 59, 8, 769},
    {4, 67, 9, 1025},
    {4, 83, 9, 1537},
    {4, 99, 10, 2049},
    {4, 115, 10, 3073},
    {5, 131, 11, 4097},
    {5, 163, 11, 6145},
    {5, 195, 12, 8193},
    {5, 227, 12, 12289},
    {0, 258, 13, 16385},
    {0, 0, 13, 24577},
};

/* special ordering of code length codes */
const unsigned char clcidx[] = {
   16, 17, 18, 0, 8, 7, 9, 6,
   10, 5, 11, 4, 12, 3, 13, 2,
   14, 1, 15
};

/* ----------------------- *
 * -- utility functions -- *
 * ----------------------- */

/* build the fixed huffman trees */
static void tinf_build_fixed_trees(TINF_TREE *lt, TINF_TREE *dt)
{
   int i;

   /* build fixed length tree */
   for (i = 0; i < 7; ++i) lt->table[i] = 0;

   lt->table[7] = 24;
   lt->table[8] = 152;
   lt->table[9] = 112;

   for (i = 0; i < 24; ++i) lt->trans[i] = 256 + i;
   for (i = 0; i < 144; ++i) lt->trans[24 + i] = i;
   for (i = 0; i < 8; ++i) lt->trans[24 + 144 + i] = 280 + i;
   for (i = 0; i < 112; ++i) lt->trans[24 + 144 + 8 + i] = 144 + i;

   /* build fixed distance tree */
   for (i = 0; i < 5; ++i) dt->table[i] = 0;

   dt->table[5] = 32;

   for (i = 0; i < 32; ++i) dt->trans[i] = i;
}

/* given an array of code lengths, build a tree */
static void tinf_build_tree(TINF_TREE *t, const unsigned char *lengths, unsigned int num)
{
   unsigned short offs[16];
   unsigned int i, sum;

   /* clear code length count table */
   for (i = 0; i < 16; ++i) t->table[i] = 0;

   /* scan symbol lengths, and sum code length counts */
   for (i = 0; i < num; ++i) t->table[lengths[i]]++;

   #if UZLIB_CONF_DEBUG_LOG >= 2
   UZLIB_DUMP_ARRAY("codelen counts:", t->table, TINF_ARRAY_SIZE(t->table));
   #endif

   /* In the lengths array, 0 means unused code. So, t->table[0] now contains
      number of unused codes. But table's purpose is to contain # of codes of
      particular length, and there're 0 codes of length 0. */
   t->table[0] = 0;

   /* compute offset table for distribution sort */
   for (sum = 0, i = 0; i < 16; ++i)
   {
      offs[i] = sum;
      sum += t->table[i];
   }

   #if UZLIB_CONF_DEBUG_LOG >= 2
   UZLIB_DUMP_ARRAY("codelen offsets:", offs, TINF_ARRAY_SIZE(offs));
   #endif

   /* create code->symbol translation table (symbols sorted by code) */
   for (i = 0; i < num; ++i)
   {
      if (lengths[i]) t->trans[offs[lengths[i]]++] = i;
   }
}

/* ---------------------- *
 * -- decode functions -- *
 * ---------------------- */

unsigned char uzlib_get_byte(uzlib_uncomp_t *d)
{
    /* If end of source buffer is not reached, return next byte from source
       buffer. */
    if (d->source < d->source_limit) {
        return *d->source++;
    }

    /* Otherwise if there's callback and we haven't seen EOF yet, try to
       read next byte using it. (Note: the callback can also update ->source
       and ->source_limit). */
    if (d->source_read_cb && !d->eof) {
        int val = d->source_read_cb(d->source_read_data);
        if (val >= 0) {
            return (unsigned char)val;
        }
    }

    /* Otherwise, we hit EOF (either from ->source_read_cb() or from exhaustion
       of the buffer), and it will be "sticky", i.e. further calls to this
       function will end up here too. */
    d->eof = true;

    return 0;
}

uint32_t tinf_get_le_uint32(uzlib_uncomp_t *d)
{
    uint32_t val = 0;
    int i;
    for (i = 4; i--;) {
        val = val >> 8 | ((uint32_t)uzlib_get_byte(d)) << 24;
    }
    return val;
}

uint32_t tinf_get_be_uint32(uzlib_uncomp_t *d)
{
    uint32_t val = 0;
    int i;
    for (i = 4; i--;) {
        val = val << 8 | uzlib_get_byte(d);
    }
    return val;
}

/* get one bit from source stream */
static int tinf_getbit(uzlib_uncomp_t *d)
{
   unsigned int bit;

   /* check if tag is empty */
   if (!d->bitcount--)
   {
      /* load next tag */
      d->tag = uzlib_get_byte(d);
      d->bitcount = 7;
   }

   /* shift bit out of tag */
   bit = d->tag & 0x01;
   d->tag >>= 1;

   return bit;
}

/* read a num bit value from a stream and add base */
static unsigned int tinf_read_bits(uzlib_uncomp_t *d, int num, int base)
{
   unsigned int val = 0;

   /* read num bits */
   if (num)
   {
      unsigned int limit = 1 << (num);
      unsigned int mask;

      for (mask = 1; mask < limit; mask *= 2)
         if (tinf_getbit(d)) val += mask;
   }

   return val + base;
}

/* given a data stream and a tree, decode a symbol */
static int tinf_decode_symbol(uzlib_uncomp_t *d, TINF_TREE *t)
{
   int sum = 0, cur = 0, len = 0;

   /* get more bits while code value is above sum */
   do {

      cur = 2*cur + tinf_getbit(d);

      if (++len == TINF_ARRAY_SIZE(t->table)) {
         return UZLIB_DATA_ERROR;
      }

      sum += t->table[len];
      cur -= t->table[len];

   } while (cur >= 0);

   sum += cur;
   #if UZLIB_CONF_PARANOID_CHECKS
   if (sum < 0 || sum >= TINF_ARRAY_SIZE(t->trans)) {
      return UZLIB_DATA_ERROR;
   }
   #endif

   return t->trans[sum];
}

/* given a data stream, decode dynamic trees from it */
static int tinf_decode_trees(uzlib_uncomp_t *d, TINF_TREE *lt, TINF_TREE *dt)
{
   /* code lengths for 288 literal/len symbols and 32 dist symbols */
   unsigned char lengths[288+32];
   unsigned int hlit, hdist, hclen, hlimit;
   unsigned int i, num, length;

   /* get 5 bits HLIT (257-286) */
   hlit = tinf_read_bits(d, 5, 257);

   /* get 5 bits HDIST (1-32) */
   hdist = tinf_read_bits(d, 5, 1);

   /* get 4 bits HCLEN (4-19) */
   hclen = tinf_read_bits(d, 4, 4);

   for (i = 0; i < 19; ++i) lengths[i] = 0;

   /* read code lengths for code length alphabet */
   for (i = 0; i < hclen; ++i)
   {
      /* get 3 bits code length (0-7) */
      unsigned int clen = tinf_read_bits(d, 3, 0);

      lengths[clcidx[i]] = clen;
   }

   /* build code length tree, temporarily use length tree */
   tinf_build_tree(lt, lengths, 19);

   /* decode code lengths for the dynamic trees */
   hlimit = hlit + hdist;
   for (num = 0; num < hlimit; )
   {
      int sym = tinf_decode_symbol(d, lt);
      unsigned char fill_value = 0;
      int lbits, lbase = 3;

      /* error decoding */
      if (sym < 0) return sym;

      switch (sym)
      {
      case 16:
         /* copy previous code length 3-6 times (read 2 bits) */
         if (num == 0) return UZLIB_DATA_ERROR;
         fill_value = lengths[num - 1];
         lbits = 2;
         break;
      case 17:
         /* repeat code length 0 for 3-10 times (read 3 bits) */
         lbits = 3;
         break;
      case 18:
         /* repeat code length 0 for 11-138 times (read 7 bits) */
         lbits = 7;
         lbase = 11;
         break;
      default:
         /* values 0-15 represent the actual code lengths */
         lengths[num++] = sym;
         /* continue the for loop */
         continue;
      }

      /* special code length 16-18 are handled here */
      length = tinf_read_bits(d, lbits, lbase);
      if (num + length > hlimit) return UZLIB_DATA_ERROR;
      for (; length; --length)
      {
         lengths[num++] = fill_value;
      }
   }

   #if UZLIB_CONF_DEBUG_LOG >= 2
   printf("lit code lengths (%d):", hlit);
   UZLIB_DUMP_ARRAY("", lengths, hlit);
   printf("dist code lengths (%d):", hdist);
   UZLIB_DUMP_ARRAY("", lengths + hlit, hdist);
   #endif

   #if UZLIB_CONF_PARANOID_CHECKS
   /* Check that there's "end of block" symbol */
   if (lengths[256] == 0) {
      return UZLIB_DATA_ERROR;
   }
   #endif

   /* build dynamic trees */
   tinf_build_tree(lt, lengths, hlit);
   tinf_build_tree(dt, lengths + hlit, hdist);

   return UZLIB_OK;
}

/* ----------------------------- *
 * -- block inflate functions -- *
 * ----------------------------- */

/* given a stream and two trees, inflate next byte of output */
static int tinf_inflate_block_data(uzlib_uncomp_t *d, TINF_TREE *lt, TINF_TREE *dt)
{
    if (d->curlen == 0) {
        unsigned int offs;
        int dist;
        int sym = tinf_decode_symbol(d, lt);
        //printf("huff sym: %02x\n", sym);

        if (d->eof) {
            return UZLIB_DATA_ERROR;
        }

        /* literal byte */
        if (sym < 256) {
            TINF_PUT(d, sym);
            return UZLIB_OK;
        }

        /* end of block */
        if (sym == 256) {
            return UZLIB_DONE;
        }

        /* substring from sliding dictionary */
        sym -= 257;
        if (sym >= 29) {
            return UZLIB_DATA_ERROR;
        }

        /* possibly get more bits from length code */
        d->curlen = tinf_read_bits(d, lookup_table[sym].length_bits, lookup_table[sym].length_base);

        dist = tinf_decode_symbol(d, dt);
        if (dist >= 30) {
            return UZLIB_DATA_ERROR;
        }

        /* possibly get more bits from distance code */
        offs = tinf_read_bits(d, lookup_table[dist].dist_bits, lookup_table[dist].dist_base);

        /* calculate and validate actual LZ offset to use */
        if (d->dict_ring) {
            if (offs > d->dict_size) {
                return UZLIB_DICT_ERROR;
            }
            /* Note: unlike full-dest-in-memory case below, we don't
               try to catch offset which points to not yet filled
               part of the dictionary here. Doing so would require
               keeping another variable to track "filled in" size
               of the dictionary. Appearance of such an offset cannot
               lead to accessing memory outside of the dictionary
               buffer, and clients which don't want to leak unrelated
               information, should explicitly initialize dictionary
               buffer passed to uzlib. */

            d->lzOff = d->dict_idx - offs;
            if (d->lzOff < 0) {
                d->lzOff += d->dict_size;
            }
        } else {
            /* catch trying to point before the start of dest buffer */
            if (offs > (unsigned int)(d->dest - d->dest_start)) {
                return UZLIB_DATA_ERROR;
            }
            d->lzOff = -offs;
        }
    }

    /* copy next byte from dict substring */
    if (d->dict_ring) {
        TINF_PUT(d, d->dict_ring[d->lzOff]);
        if ((unsigned)++d->lzOff == d->dict_size) {
            d->lzOff = 0;
        }
    } else {
        d->dest[0] = d->dest[d->lzOff];
        d->dest++;
    }
    d->curlen--;
    return UZLIB_OK;
}

/* inflate next byte from uncompressed block of data */
static int tinf_inflate_uncompressed_block(uzlib_uncomp_t *d)
{
    if (d->curlen == 0) {
        unsigned int length, invlength;

        /* get length */
        length = uzlib_get_byte(d);
        length += 256 * uzlib_get_byte(d);
        /* get one's complement of length */
        invlength = uzlib_get_byte(d);
        invlength += 256 * uzlib_get_byte(d);
        /* check length */
        if (length != (~invlength & 0x0000ffff)) return UZLIB_DATA_ERROR;

        /* increment length to properly return UZLIB_DONE below, without
           producing data at the same time */
        d->curlen = length + 1;

        /* make sure we start next block on a byte boundary */
        d->bitcount = 0;
    }

    if (--d->curlen == 0) {
        return UZLIB_DONE;
    }

    unsigned char c = uzlib_get_byte(d);
    TINF_PUT(d, c);
    return UZLIB_OK;
}

/* ---------------------- *
 * -- public functions -- *
 * ---------------------- */

/* initialize decompression structure */
void uzlib_uncompress_init(uzlib_uncomp_t *d, void *dict, unsigned int dictLen)
{
   d->eof = 0;
   d->bitcount = 0;
   d->bfinal = 0;
   d->btype = -1;
   d->dict_size = dictLen;
   d->dict_ring = dict;
   d->dict_idx = 0;
   d->curlen = 0;
}

/* inflate next output bytes from compressed stream */
int uzlib_uncompress(uzlib_uncomp_t *d)
{
    do {
        int res;

        /* start a new block */
        if (d->btype == -1) {
next_blk:
            /* read final block flag */
            d->bfinal = tinf_getbit(d);
            /* read block type (2 bits) */
            d->btype = tinf_read_bits(d, 2, 0);

            #if UZLIB_CONF_DEBUG_LOG >= 1
            printf("Started new block: type=%d final=%d\n", d->btype, d->bfinal);
            #endif

            if (d->btype == 1) {
                /* build fixed huffman trees */
                tinf_build_fixed_trees(&d->ltree, &d->dtree);
            } else if (d->btype == 2) {
                /* decode trees from stream */
                res = tinf_decode_trees(d, &d->ltree, &d->dtree);
                if (res != UZLIB_OK) {
                    return res;
                }
            }
        }

        /* process current block */
        switch (d->btype)
        {
        case 0:
            /* decompress uncompressed block */
            res = tinf_inflate_uncompressed_block(d);
            break;
        case 1:
        case 2:
            /* decompress block with fixed/dynamic huffman trees */
            /* trees were decoded previously, so it's the same routine for both */
            res = tinf_inflate_block_data(d, &d->ltree, &d->dtree);
            break;
        default:
            return UZLIB_DATA_ERROR;
        }

        if (res == UZLIB_DONE && !d->bfinal) {
            /* the block has ended (without producing more data), but we
               can't return without data, so start procesing next block */
            goto next_blk;
        }

        if (res != UZLIB_OK) {
            return res;
        }

    } while (d->dest < d->dest_limit);

    return UZLIB_OK;
}

/* inflate next output bytes from compressed stream, updating
   checksum, and at the end of stream, verify it */
int uzlib_uncompress_chksum(uzlib_uncomp_t *d)
{
    int res;
    unsigned char *data = d->dest;

    res = uzlib_uncompress(d);

    if (res < 0) return res;

    switch (d->checksum_type) {

    case UZLIB_CHKSUM_ADLER:
        d->checksum = uzlib_adler32(data, d->dest - data, d->checksum);
        break;

    case UZLIB_CHKSUM_CRC:
        d->checksum = uzlib_crc32(data, d->dest - data, d->checksum);
        break;
    }

    if (res == UZLIB_DONE) {
        unsigned int val;

        switch (d->checksum_type) {

        case UZLIB_CHKSUM_ADLER:
            val = tinf_get_be_uint32(d);
            if (d->checksum != val) {
                return UZLIB_CHKSUM_ERROR;
            }
            break;

        case UZLIB_CHKSUM_CRC:
            val = tinf_get_le_uint32(d);
            if (~d->checksum != val) {
                return UZLIB_CHKSUM_ERROR;
            }
            // Uncompressed size. TODO: Check
            val = tinf_get_le_uint32(d);
            break;
        }
    }

    return res;
}
