/**
 * \file error.h
 *
 * \brief Error to string translation
 *
 *  Copyright (C) 2006-2013, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef POLARSSL_ERROR_H
#define POLARSSL_ERROR_H

#include <string.h>

/**
 * Error code layout.
 *
 * Currently we try to keep all error codes within the negative space of 16
 * bytes signed integers to support all platforms (-0x0000 - -0x8000). In
 * addition we'd like to give two layers of information on the error if
 * possible.
 *
 * For that purpose the error codes are segmented in the following manner:
 *
 * 16 bit error code bit-segmentation
 *
 * 1 bit  - Sign bit
 * 3 bits - High level module ID
 * 5 bits - Module-dependent error code
 * 7 bits - Low level module errors
 *
 * For historical reasons, low-level error codes are divided in even and odd,
 * even codes were assigned first, and -1 is reserved for other errors.
 *
 * Low-level module errors (0x0002-0x007E, 0x0003-0x007F)
 *
 * Module   Nr  Codes assigned
 * MPI       7  0x0002-0x0010
 * GCM       2  0x0012-0x0014
 * BLOWFISH  2  0x0016-0x0018
 * THREADING 3  0x001A-0x001E
 * AES       2  0x0020-0x0022
 * CAMELLIA  2  0x0024-0x0026
 * XTEA      1  0x0028-0x0028
 * BASE64    2  0x002A-0x002C
 * OID       1  0x002E-0x002E   0x000B-0x000B
 * PADLOCK   1  0x0030-0x0030
 * DES       1  0x0032-0x0032
 * CTR_DBRG  4  0x0034-0x003A
 * ENTROPY   3  0x003C-0x0040
 * NET      11  0x0042-0x0056
 * ENTROPY   1  0x0058-0x0058
 * ASN1      7  0x0060-0x006C
 * MD2       1  0x0070-0x0070
 * MD4       1  0x0072-0x0072
 * MD5       1  0x0074-0x0074
 * SHA1      1  0x0076-0x0076
 * SHA256    1  0x0078-0x0078
 * SHA512    1  0x007A-0x007A
 * PBKDF2    1  0x007C-0x007C
 * RIPEMD160 1  0x007E-0x007E
 * HMAC_DRBG 4  0x0003-0x0009
 * CCM       2                  0x000D-0x000F
 *
 * High-level module nr (3 bits - 0x0...-0x7...)
 * Name      ID  Nr of Errors
 * PEM       1   9
 * PKCS#12   1   4 (Started from top)
 * X509      2   18
 * PK        2   14 (Started from top, plus 0x2000)
 * DHM       3   9
 * PKCS5     3   4 (Started from top)
 * RSA       4   9
 * ECP       4   8 (Started from top)
 * MD        5   4
 * CIPHER    6   6
 * SSL       6   9 (Started from top)
 * SSL       7   31
 *
 * Module dependent error code (5 bits 0x.00.-0x.F8.)
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Translate a PolarSSL error code into a string representation,
 *        Result is truncated if necessary and always includes a terminating
 *        null byte.
 *
 * \param errnum    error code
 * \param buffer    buffer to place representation in
 * \param buflen    length of the buffer
 */
void polarssl_strerror( int errnum, char *buffer, size_t buflen );

#if defined(POLARSSL_ERROR_STRERROR_BC)
void error_strerror( int errnum, char *buffer, size_t buflen );
#endif

#ifdef __cplusplus
}
#endif

#endif /* error.h */
