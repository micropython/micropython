/*
 * tinfzlib  -  tiny zlib decompressor
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

int uzlib_zlib_parse_header(TINF_DATA *d)
{
   unsigned char cmf, flg;

   /* -- get header bytes -- */

   cmf = uzlib_get_byte(d);
   flg = uzlib_get_byte(d);

   /* -- check format -- */

   /* check checksum */
   if ((256*cmf + flg) % 31) return TINF_DATA_ERROR;

   /* check method is deflate */
   if ((cmf & 0x0f) != 8) return TINF_DATA_ERROR;

   /* check window size is valid */
   if ((cmf >> 4) > 7) return TINF_DATA_ERROR;

   /* check there is no preset dictionary */
   if (flg & 0x20) return TINF_DATA_ERROR;

   /* initialize for adler32 checksum */
   d->checksum_type = TINF_CHKSUM_ADLER;
   d->checksum = 1;

   return cmf >> 4;
}
