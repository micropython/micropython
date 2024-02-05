/*

Routines in this file are based on:
Zlib (RFC1950 / RFC1951) compression for PuTTY.

PuTTY is copyright 1997-2014 Simon Tatham.

Portions copyright Robert de Bath, Joris van Rantwijk, Delian
Delchev, Andreas Schultz, Jeroen Massar, Wez Furlong, Nicolas Barry,
Justin Bradford, Ben Harris, Malcolm Smith, Ahmad Khalifa, Markus
Kuhn, Colin Watson, and CORE SDI S.A.

Optimised for MicroPython:
Copyright (c) 2023 by Jim Mussared

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

static void outbits(uzlib_lz77_state_t *state, unsigned long bits, int nbits)
{
    assert(state->noutbits + nbits <= 32);
    state->outbits |= bits << state->noutbits;
    state->noutbits += nbits;
    while (state->noutbits >= 8) {
        state->dest_write_cb(state->dest_write_data, state->outbits & 0xFF);
        state->outbits >>= 8;
        state->noutbits -= 8;
    }
}

static const unsigned char mirrornibbles[16] = {
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf,
};

static unsigned int mirrorbyte(unsigned int b) {
    return mirrornibbles[b & 0xf] << 4 | mirrornibbles[b >> 4];
}

static int int_log2(int x) {
    int r = 0;
    while (x >>= 1) {
        ++r;
    }
    return r;
}

static void uzlib_literal(uzlib_lz77_state_t *state, unsigned char c)
{
    if (c <= 143) {
        /* 0 through 143 are 8 bits long starting at 00110000. */
        outbits(state, mirrorbyte(0x30 + c), 8);
    } else {
        /* 144 through 255 are 9 bits long starting at 110010000. */
        outbits(state, 1 + 2 * mirrorbyte(0x90 - 144 + c), 9);
    }
}

static void uzlib_match(uzlib_lz77_state_t *state, int distance, int len)
{
    assert(distance >= 1 && distance <= 32768);
    distance -= 1;

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

        assert(thislen >= 3 && thislen <= 258);

        thislen -= 3;
        int lcode = 28;
        int x = int_log2(thislen);
        int y;
        if (thislen < 255) {
            if (x) {
                --x;
            }
            y = (thislen >> (x ? x - 1 : 0)) & 3;
            lcode = x * 4 + y;
        }

        /*
         * Transmit the length code. 256-279 are seven bits
         * starting at 0000000; 280-287 are eight bits starting at
         * 11000000.
         */
        if (lcode <= 22) {
            outbits(state, mirrorbyte((lcode + 1) * 2), 7);
        } else {
            outbits(state, mirrorbyte(lcode + 169), 8);
        }

        /*
         * Transmit the extra bits.
         */
        if (thislen < 255 && x > 1) {
            int extrabits = x - 1;
            int lmin = (thislen >> extrabits) << extrabits;
            outbits(state, thislen - lmin, extrabits);
        }

        x = int_log2(distance);
        y = (distance >> (x ? x - 1 : 0)) & 1;

        /*
         * Transmit the distance code. Five bits starting at 00000.
         */
        outbits(state, mirrorbyte((x * 2 + y) * 8), 5);

        /*
         * Transmit the extra bits.
         */
        if (x > 1) {
            int dextrabits = x - 1;
            int dmin = (distance >> dextrabits) << dextrabits;
            outbits(state, distance - dmin, dextrabits);
        }
    }
}

void uzlib_start_block(uzlib_lz77_state_t *state)
{
    // Final block (0b1)
    // Static huffman block (0b01)
    outbits(state, 3, 3);
}

void uzlib_finish_block(uzlib_lz77_state_t *state)
{
    // Close block (0b0000000)
    // Make sure all bits are flushed (0b0000000)
    outbits(state, 0, 14);
}
