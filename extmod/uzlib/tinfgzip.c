/*
 * tinfgzip  -  tiny gzip decompressor
 *
 * Copyright (c) 2003 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
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

#include "tinf.h"

#define FTEXT    1
#define FHCRC    2
#define FEXTRA   4
#define FNAME    8
#define FCOMMENT 16

void tinf_skip_bytes(TINF_DATA *d, int num);
uint16_t tinf_get_uint16(TINF_DATA *d);

void tinf_skip_bytes(TINF_DATA *d, int num)
{
    while (num--) uzlib_get_byte(d);
}

uint16_t tinf_get_uint16(TINF_DATA *d)
{
    unsigned int v = uzlib_get_byte(d);
    v = (uzlib_get_byte(d) << 8) | v;
    return v;
}

int uzlib_gzip_parse_header(TINF_DATA *d)
{
    unsigned char flg;

    /* -- check format -- */

    /* check id bytes */
    if (uzlib_get_byte(d) != 0x1f || uzlib_get_byte(d) != 0x8b) return TINF_DATA_ERROR;

    /* check method is deflate */
    if (uzlib_get_byte(d) != 8) return TINF_DATA_ERROR;

    /* get flag byte */
    flg = uzlib_get_byte(d);

    /* check that reserved bits are zero */
    if (flg & 0xe0) return TINF_DATA_ERROR;

    /* -- find start of compressed data -- */

    /* skip rest of base header of 10 bytes */
    tinf_skip_bytes(d, 6);

    /* skip extra data if present */
    if (flg & FEXTRA)
    {
       unsigned int xlen = tinf_get_uint16(d);
       tinf_skip_bytes(d, xlen);
    }

    /* skip file name if present */
    if (flg & FNAME) { while (uzlib_get_byte(d)); }

    /* skip file comment if present */
    if (flg & FCOMMENT) { while (uzlib_get_byte(d)); }

    /* check header crc if present */
    if (flg & FHCRC)
    {
       /*unsigned int hcrc =*/ tinf_get_uint16(d);

        // TODO: Check!
//       if (hcrc != (tinf_crc32(src, start - src) & 0x0000ffff))
//          return TINF_DATA_ERROR;
    }

    /* initialize for crc32 checksum */
    d->checksum_type = TINF_CHKSUM_CRC;
    d->checksum = ~0;

    return TINF_OK;
}
