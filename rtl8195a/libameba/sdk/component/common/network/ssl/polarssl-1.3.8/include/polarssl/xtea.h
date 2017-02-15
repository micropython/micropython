/**
 * \file xtea.h
 *
 * \brief XTEA block cipher (32-bit)
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
#ifndef POLARSSL_XTEA_H
#define POLARSSL_XTEA_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <string.h>

#if defined(_MSC_VER) && !defined(EFIX64) && !defined(EFI32)
#include <basetsd.h>
typedef UINT32 uint32_t;
#else
#include <inttypes.h>
#endif

#define XTEA_ENCRYPT     1
#define XTEA_DECRYPT     0

#define POLARSSL_ERR_XTEA_INVALID_INPUT_LENGTH             -0x0028  /**< The data input has an invalid length. */

#if !defined(POLARSSL_XTEA_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          XTEA context structure
 */
typedef struct
{
    uint32_t k[4];       /*!< key */
}
xtea_context;

/**
 * \brief          Initialize XTEA context
 *
 * \param ctx      XTEA context to be initialized
 */
void xtea_init( xtea_context *ctx );

/**
 * \brief          Clear XTEA context
 *
 * \param ctx      XTEA context to be cleared
 */
void xtea_free( xtea_context *ctx );

/**
 * \brief          XTEA key schedule
 *
 * \param ctx      XTEA context to be initialized
 * \param key      the secret key
 */
void xtea_setup( xtea_context *ctx, const unsigned char key[16] );

/**
 * \brief          XTEA cipher function
 *
 * \param ctx      XTEA context
 * \param mode     XTEA_ENCRYPT or XTEA_DECRYPT
 * \param input    8-byte input block
 * \param output   8-byte output block
 *
 * \return         0 if successful
 */
int xtea_crypt_ecb( xtea_context *ctx,
                    int mode,
                    const unsigned char input[8],
                    unsigned char output[8] );

#if defined(POLARSSL_CIPHER_MODE_CBC)
/**
 * \brief          XTEA CBC cipher function
 *
 * \param ctx      XTEA context
 * \param mode     XTEA_ENCRYPT or XTEA_DECRYPT
 * \param length   the length of input, multiple of 8
 * \param iv       initialization vector for CBC mode
 * \param input    input block
 * \param output   output block
 *
 * \return         0 if successful,
 *                 POLARSSL_ERR_XTEA_INVALID_INPUT_LENGTH if the length % 8 != 0
 */
int xtea_crypt_cbc( xtea_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[8],
                    const unsigned char *input,
                    unsigned char *output);
#endif /* POLARSSL_CIPHER_MODE_CBC */

#ifdef __cplusplus
}
#endif

#else  /* POLARSSL_XTEA_ALT */
#include "xtea_alt.h"
#endif /* POLARSSL_XTEA_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int xtea_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* xtea.h */
