/*

Routines in this file are based on:
Zlib (RFC1950 / RFC1951) compression for PuTTY.

PuTTY is copyright 1997-2014 Simon Tatham.

Portions copyright Robert de Bath, Joris van Rantwijk, Delian
Delchev, Andreas Schultz, Jeroen Massar, Wez Furlong, Nicolas Barry,
Justin Bradford, Ben Harris, Malcolm Smith, Ahmad Khalifa, Markus
Kuhn, Colin Watson, and CORE SDI S.A.

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "uzlib.h"
#include "defl_static.h"

#define snew(type) ( (type *) gc_malloc(sizeof(type)) )
#define snewn(n, type) ( (type *) gc_malloc((n) * sizeof(type)) )
#define sresize(x, n, type) ( (type *) gc_realloc((x), (n) * sizeof(type), true) )
#define sfree(x) ( gc_free((x)) )

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

/* ----------------------------------------------------------------------
 * Zlib compression. We always use the static Huffman tree option.
 * Mostly this is because it's hard to scan a block in advance to
 * work out better trees; dynamic trees are great when you're
 * compressing a large file under no significant time constraint,
 * but when you're compressing little bits in real time, things get
 * hairier.
 * 
 * I suppose it's possible that I could compute Huffman trees based
 * on the frequencies in the _previous_ block, as a sort of
 * heuristic, but I'm not confident that the gain would balance out
 * having to transmit the trees.
 */

void outbits(struct uzlib_comp *out, unsigned long bits, int nbits)
{
    assert(out->noutbits + nbits <= 32);
    out->outbits |= bits << out->noutbits;
    out->noutbits += nbits;
    while (out->noutbits >= 8) {
        if (out->outlen >= out->outsize) {
            out->outsize = out->outlen + 64;
            out->outbuf = sresize(out->outbuf, out->outsize, unsigned char);
        }
        out->outbuf[out->outlen++] = (unsigned char) (out->outbits & 0xFF);
        out->outbits >>= 8;
        out->noutbits -= 8;
    }
}

static const unsigned char mirrorbytes[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

typedef struct {
    uint8_t extrabits;
    uint8_t min, max;
} len_coderecord;

typedef struct {
    uint8_t code, extrabits;
    uint16_t min, max;
} dist_coderecord;

#define TO_LCODE(x, y) x - 3, y - 3
#define FROM_LCODE(x) (x + 3)

static const len_coderecord lencodes[] = {
    {0, TO_LCODE(3, 3)},
    {0, TO_LCODE(4, 4)},
    {0, TO_LCODE(5, 5)},
    {0, TO_LCODE(6, 6)},
    {0, TO_LCODE(7, 7)},
    {0, TO_LCODE(8, 8)},
    {0, TO_LCODE(9, 9)},
    {0, TO_LCODE(10, 10)},
    {1, TO_LCODE(11, 12)},
    {1, TO_LCODE(13, 14)},
    {1, TO_LCODE(15, 16)},
    {1, TO_LCODE(17, 18)},
    {2, TO_LCODE(19, 22)},
    {2, TO_LCODE(23, 26)},
    {2, TO_LCODE(27, 30)},
    {2, TO_LCODE(31, 34)},
    {3, TO_LCODE(35, 42)},
    {3, TO_LCODE(43, 50)},
    {3, TO_LCODE(51, 58)},
    {3, TO_LCODE(59, 66)},
    {4, TO_LCODE(67, 82)},
    {4, TO_LCODE(83, 98)},
    {4, TO_LCODE(99, 114)},
    {4, TO_LCODE(115, 130)},
    {5, TO_LCODE(131, 162)},
    {5, TO_LCODE(163, 194)},
    {5, TO_LCODE(195, 226)},
    {5, TO_LCODE(227, 257)},
    {0, TO_LCODE(258, 258)},
};

static const dist_coderecord distcodes[] = {
    {0, 0, 1, 1},
    {1, 0, 2, 2},
    {2, 0, 3, 3},
    {3, 0, 4, 4},
    {4, 1, 5, 6},
    {5, 1, 7, 8},
    {6, 2, 9, 12},
    {7, 2, 13, 16},
    {8, 3, 17, 24},
    {9, 3, 25, 32},
    {10, 4, 33, 48},
    {11, 4, 49, 64},
    {12, 5, 65, 96},
    {13, 5, 97, 128},
    {14, 6, 129, 192},
    {15, 6, 193, 256},
    {16, 7, 257, 384},
    {17, 7, 385, 512},
    {18, 8, 513, 768},
    {19, 8, 769, 1024},
    {20, 9, 1025, 1536},
    {21, 9, 1537, 2048},
    {22, 10, 2049, 3072},
    {23, 10, 3073, 4096},
    {24, 11, 4097, 6144},
    {25, 11, 6145, 8192},
    {26, 12, 8193, 12288},
    {27, 12, 12289, 16384},
    {28, 13, 16385, 24576},
    {29, 13, 24577, 32768},
};

void zlib_literal(struct uzlib_comp *out, unsigned char c)
{
    if (out->comp_disabled) {
        /*
         * We're in an uncompressed block, so just output the byte.
         */
        outbits(out, c, 8);
        return;
    }

    if (c <= 143) {
        /* 0 through 143 are 8 bits long starting at 00110000. */
        outbits(out, mirrorbytes[0x30 + c], 8);
    } else {
        /* 144 through 255 are 9 bits long starting at 110010000. */
        outbits(out, 1 + 2 * mirrorbytes[0x90 - 144 + c], 9);
    }
}

void zlib_match(struct uzlib_comp *out, int distance, int len)
{
    const dist_coderecord *d;
    const len_coderecord *l;
    int i, j, k;
    int lcode;

    assert(!out->comp_disabled);

    while (len > 0) {
        int thislen;

        /*
         * We can transmit matches of lengths 3 through 258
         * inclusive. So if len exceeds 258, we must transmit in
         * several steps, with 258 or less in each step.
         * 
         * Specifically: if len >= 261, we can transmit 258 and be
         * sure of having at least 3 left for the next step. And if
         * len <= 258, we can just transmit len. But if len == 259
         * or 260, we must transmit len-3.
         */
        thislen = (len > 260 ? 258 : len <= 258 ? len : len - 3);
        len -= thislen;

        /*
         * Binary-search to find which length code we're
         * transmitting.
         */
        i = -1;
        j = sizeof(lencodes) / sizeof(*lencodes);
        while (1) {
            assert(j - i >= 2);
            k = (j + i) / 2;
            if (thislen < FROM_LCODE(lencodes[k].min))
                j = k;
            else if (thislen > FROM_LCODE(lencodes[k].max))
                i = k;
            else {
                l = &lencodes[k];
                break;                 /* found it! */
            }
        }

        lcode = l - lencodes + 257;

        /*
         * Transmit the length code. 256-279 are seven bits
         * starting at 0000000; 280-287 are eight bits starting at
         * 11000000.
         */
        if (lcode <= 279) {
            outbits(out, mirrorbytes[(lcode - 256) * 2], 7);
        } else {
            outbits(out, mirrorbytes[0xc0 - 280 + lcode], 8);
        }

        /*
         * Transmit the extra bits.
         */
        if (l->extrabits)
            outbits(out, thislen - FROM_LCODE(l->min), l->extrabits);

        /*
         * Binary-search to find which distance code we're
         * transmitting.
         */
        i = -1;
        j = sizeof(distcodes) / sizeof(*distcodes);
        while (1) {
            assert(j - i >= 2);
            k = (j + i) / 2;
            if (distance < distcodes[k].min)
                j = k;
            else if (distance > distcodes[k].max)
                i = k;
            else {
                d = &distcodes[k];
                break;                 /* found it! */
            }
        }

        /*
         * Transmit the distance code. Five bits starting at 00000.
         */
        outbits(out, mirrorbytes[d->code * 8], 5);

        /*
         * Transmit the extra bits.
         */
        if (d->extrabits)
            outbits(out, distance - d->min, d->extrabits);
    }
}

void zlib_start_block(struct uzlib_comp *out)
{
//    outbits(out, 0x9C78, 16);
    outbits(out, 1, 1); /* Final block */
    outbits(out, 1, 2); /* Static huffman block */
}

void zlib_finish_block(struct uzlib_comp *out)
{
    outbits(out, 0, 7); /* close block */
    outbits(out, 0, 7); /* Make sure all bits are flushed */
}
