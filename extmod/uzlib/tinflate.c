/*
 * tinflate  -  tiny inflate
 *
 * Copyright (c) 2003 by Joergen Ibsen / Jibz
 * All Rights Reserved
 * http://www.ibsensoftware.com/
 *
 * Copyright (c) 2014-2016 by Paul Sokolovsky
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
#include "tinf.h"

uint32_t tinf_get_le_uint32(TINF_DATA *d);
uint32_t tinf_get_be_uint32(TINF_DATA *d);

/* --------------------------------------------------- *
 * -- uninitialized global data (static structures) -- *
 * --------------------------------------------------- */

#ifdef RUNTIME_BITS_TABLES

/* extra bits and base tables for length codes */
unsigned char length_bits[30];
unsigned short length_base[30];

/* extra bits and base tables for distance codes */
unsigned char dist_bits[30];
unsigned short dist_base[30];

#else

const unsigned char length_bits[30] = {
   0, 0, 0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 2, 2, 2, 2,
   3, 3, 3, 3, 4, 4, 4, 4,
   5, 5, 5, 5
};
const unsigned short length_base[30] = {
   3, 4, 5, 6, 7, 8, 9, 10,
   11, 13, 15, 17, 19, 23, 27, 31,
   35, 43, 51, 59, 67, 83, 99, 115,
   131, 163, 195, 227, 258
};

const unsigned char dist_bits[30] = {
   0, 0, 0, 0, 1, 1, 2, 2,
   3, 3, 4, 4, 5, 5, 6, 6,
   7, 7, 8, 8, 9, 9, 10, 10,
   11, 11, 12, 12, 13, 13
};
const unsigned short dist_base[30] = {
   1, 2, 3, 4, 5, 7, 9, 13,
   17, 25, 33, 49, 65, 97, 129, 193,
   257, 385, 513, 769, 1025, 1537, 2049, 3073,
   4097, 6145, 8193, 12289, 16385, 24577
};

#endif

/* special ordering of code length codes */
const unsigned char clcidx[] = {
   16, 17, 18, 0, 8, 7, 9, 6,
   10, 5, 11, 4, 12, 3, 13, 2,
   14, 1, 15
};

/* ----------------------- *
 * -- utility functions -- *
 * ----------------------- */

#ifdef RUNTIME_BITS_TABLES
/* build extra bits and base tables */
static void tinf_build_bits_base(unsigned char *bits, unsigned short *base, int delta, int first)
{
   int i, sum;

   /* build bits table */
   for (i = 0; i < delta; ++i) bits[i] = 0;
   for (i = 0; i < 30 - delta; ++i) bits[i + delta] = i / delta;

   /* build base table */
   for (sum = first, i = 0; i < 30; ++i)
   {
      base[i] = sum;
      sum += 1 << bits[i];
   }
}
#endif

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

   t->table[0] = 0;

   /* compute offset table for distribution sort */
   for (sum = 0, i = 0; i < 16; ++i)
   {
      offs[i] = sum;
      sum += t->table[i];
   }

   /* create code->symbol translation table (symbols sorted by code) */
   for (i = 0; i < num; ++i)
   {
      if (lengths[i]) t->trans[offs[lengths[i]]++] = i;
   }
}

/* ---------------------- *
 * -- decode functions -- *
 * ---------------------- */

unsigned char uzlib_get_byte(TINF_DATA *d)
{
    if (d->source) {
        return *d->source++;
    }
    return d->readSource(d);
}

uint32_t tinf_get_le_uint32(TINF_DATA *d)
{
    uint32_t val = 0;
    int i;
    for (i = 4; i--;) {
        val = val >> 8 | uzlib_get_byte(d) << 24;
    }
    return val;
}

uint32_t tinf_get_be_uint32(TINF_DATA *d)
{
    uint32_t val = 0;
    int i;
    for (i = 4; i--;) {
        val = val << 8 | uzlib_get_byte(d);
    }
    return val;
}

/* get one bit from source stream */
static int tinf_getbit(TINF_DATA *d)
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
static unsigned int tinf_read_bits(TINF_DATA *d, int num, int base)
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
static int tinf_decode_symbol(TINF_DATA *d, TINF_TREE *t)
{
   int sum = 0, cur = 0, len = 0;

   /* get more bits while code value is above sum */
   do {

      cur = 2*cur + tinf_getbit(d);

      ++len;

      sum += t->table[len];
      cur -= t->table[len];

   } while (cur >= 0);

   return t->trans[sum + cur];
}

/* given a data stream, decode dynamic trees from it */
static void tinf_decode_trees(TINF_DATA *d, TINF_TREE *lt, TINF_TREE *dt)
{
   unsigned char lengths[288+32];
   unsigned int hlit, hdist, hclen;
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
   for (num = 0; num < hlit + hdist; )
   {
      int sym = tinf_decode_symbol(d, lt);

      switch (sym)
      {
      case 16:
         /* copy previous code length 3-6 times (read 2 bits) */
         {
            unsigned char prev = lengths[num - 1];
            for (length = tinf_read_bits(d, 2, 3); length; --length)
            {
               lengths[num++] = prev;
            }
         }
         break;
      case 17:
         /* repeat code length 0 for 3-10 times (read 3 bits) */
         for (length = tinf_read_bits(d, 3, 3); length; --length)
         {
            lengths[num++] = 0;
         }
         break;
      case 18:
         /* repeat code length 0 for 11-138 times (read 7 bits) */
         for (length = tinf_read_bits(d, 7, 11); length; --length)
         {
            lengths[num++] = 0;
         }
         break;
      default:
         /* values 0-15 represent the actual code lengths */
         lengths[num++] = sym;
         break;
      }
   }

   /* build dynamic trees */
   tinf_build_tree(lt, lengths, hlit);
   tinf_build_tree(dt, lengths + hlit, hdist);
}

/* ----------------------------- *
 * -- block inflate functions -- *
 * ----------------------------- */

/* given a stream and two trees, inflate a block of data */
static int tinf_inflate_block_data(TINF_DATA *d, TINF_TREE *lt, TINF_TREE *dt)
{
    if (d->curlen == 0) {
        unsigned int offs;
        int dist;
        int sym = tinf_decode_symbol(d, lt);
        //printf("huff sym: %02x\n", sym);

        /* literal byte */
        if (sym < 256) {
            TINF_PUT(d, sym);
            return TINF_OK;
        }

        /* end of block */
        if (sym == 256) {
            return TINF_DONE;
        }

        /* substring from sliding dictionary */
        sym -= 257;
        /* possibly get more bits from length code */
        d->curlen = tinf_read_bits(d, length_bits[sym], length_base[sym]);

        dist = tinf_decode_symbol(d, dt);
        /* possibly get more bits from distance code */
        offs = tinf_read_bits(d, dist_bits[dist], dist_base[dist]);
        if (d->dict_ring) {
            d->lzOff = d->dict_idx - offs;
            if (d->lzOff < 0) {
                d->lzOff += d->dict_size;
            }
        } else {
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
    return TINF_OK;
}

/* inflate an uncompressed block of data */
static int tinf_inflate_uncompressed_block(TINF_DATA *d)
{
    if (d->curlen == 0) {
        unsigned int length, invlength;

        /* get length */
        length = uzlib_get_byte(d) + 256 * uzlib_get_byte(d);
        /* get one's complement of length */
        invlength = uzlib_get_byte(d) + 256 * uzlib_get_byte(d);
        /* check length */
        if (length != (~invlength & 0x0000ffff)) return TINF_DATA_ERROR;

        /* increment length to properly return TINF_DONE below, without
           producing data at the same time */
        d->curlen = length + 1;

        /* make sure we start next block on a byte boundary */
        d->bitcount = 0;
    }

    if (--d->curlen == 0) {
        return TINF_DONE;
    }

    unsigned char c = uzlib_get_byte(d);
    TINF_PUT(d, c);
    return TINF_OK;
}

/* ---------------------- *
 * -- public functions -- *
 * ---------------------- */

/* initialize global (static) data */
void uzlib_init(void)
{
#ifdef RUNTIME_BITS_TABLES
   /* build extra bits and base tables */
   tinf_build_bits_base(length_bits, length_base, 4, 3);
   tinf_build_bits_base(dist_bits, dist_base, 2, 1);

   /* fix a special case */
   length_bits[28] = 0;
   length_base[28] = 258;
#endif
}

/* initialize decompression structure */
void uzlib_uncompress_init(TINF_DATA *d, void *dict, unsigned int dictLen)
{
   d->bitcount = 0;
   d->bfinal = 0;
   d->btype = -1;
   d->dict_size = dictLen;
   d->dict_ring = dict;
   d->dict_idx = 0;
   d->curlen = 0;
}

/* inflate next byte of compressed stream */
int uzlib_uncompress(TINF_DATA *d)
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

            //printf("Started new block: type=%d final=%d\n", d->btype, d->bfinal);

            if (d->btype == 1) {
                /* build fixed huffman trees */
                tinf_build_fixed_trees(&d->ltree, &d->dtree);
            } else if (d->btype == 2) {
                /* decode trees from stream */
                tinf_decode_trees(d, &d->ltree, &d->dtree);
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
            /* decompress block with fixed/dyanamic huffman trees */
            /* trees were decoded previously, so it's the same routine for both */
            res = tinf_inflate_block_data(d, &d->ltree, &d->dtree);
            break;
        default:
            return TINF_DATA_ERROR;
        }

        if (res == TINF_DONE && !d->bfinal) {
            /* the block has ended (without producing more data), but we
               can't return without data, so start procesing next block */
            goto next_blk;
        }

        if (res != TINF_OK) {
            return res;
        }

    } while (--d->destSize);

    return TINF_OK;
}

int uzlib_uncompress_chksum(TINF_DATA *d)
{
    int res;
    unsigned char *data = d->dest;

    res = uzlib_uncompress(d);

    if (res < 0) return res;

    switch (d->checksum_type) {

    case TINF_CHKSUM_ADLER:
        d->checksum = uzlib_adler32(data, d->dest - data, d->checksum);
        break;

    case TINF_CHKSUM_CRC:
        d->checksum = uzlib_crc32(data, d->dest - data, d->checksum);
        break;
    }

    if (res == TINF_DONE) {
        unsigned int val;

        switch (d->checksum_type) {

        case TINF_CHKSUM_ADLER:
            val = tinf_get_be_uint32(d);
            if (d->checksum != val) {
                return TINF_CHKSUM_ERROR;
            }
            break;

        case TINF_CHKSUM_CRC:
            val = tinf_get_le_uint32(d);
            if (~d->checksum != val) {
                return TINF_CHKSUM_ERROR;
            }
            // Uncompressed size. TODO: Check
            val = tinf_get_le_uint32(d);
            break;
        }
    }

    return res;
}
