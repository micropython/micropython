/**
 * \file blowfish.h
 *
 * \brief Blowfish block cipher
 *
 *  Copyright (C) 2012-2014, Brainspark B.V.
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
#ifndef POLARSSL_BLOWFISH_H
#define POLARSSL_BLOWFISH_H

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

#define BLOWFISH_ENCRYPT     1
#define BLOWFISH_DECRYPT     0
#define BLOWFISH_MAX_KEY     448
#define BLOWFISH_MIN_KEY     32
#define BLOWFISH_ROUNDS      16         /**< Rounds to use. When increasing this value, make sure to extend the initialisation vectors */
#define BLOWFISH_BLOCKSIZE   8          /* Blowfish uses 64 bit blocks */

#define POLARSSL_ERR_BLOWFISH_INVALID_KEY_LENGTH                -0x0016  /**< Invalid key length. */
#define POLARSSL_ERR_BLOWFISH_INVALID_INPUT_LENGTH              -0x0018  /**< Invalid data input length. */

#if !defined(POLARSSL_BLOWFISH_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Blowfish context structure
 */
typedef struct
{
    uint32_t P[BLOWFISH_ROUNDS + 2];    /*!<  Blowfish round keys    */
    uint32_t S[4][256];                 /*!<  key dependent S-boxes  */
}
blowfish_context;

/**
 * \brief          Initialize Blowfish context
 *
 * \param ctx      Blowfish context to be initialized
 */
void blowfish_init( blowfish_context *ctx );

/**
 * \brief          Clear Blowfish context
 *
 * \param ctx      Blowfish context to be cleared
 */
void blowfish_free( blowfish_context *ctx );

/**
 * \brief          Blowfish key schedule
 *
 * \param ctx      Blowfish context to be initialized
 * \param key      encryption key
 * \param keysize  must be between 32 and 448 bits
 *
 * \return         0 if successful, or POLARSSL_ERR_BLOWFISH_INVALID_KEY_LENGTH
 */
int blowfish_setkey( blowfish_context *ctx, const unsigned char *key,
                     unsigned int keysize );

/**
 * \brief          Blowfish-ECB block encryption/decryption
 *
 * \param ctx      Blowfish context
 * \param mode     BLOWFISH_ENCRYPT or BLOWFISH_DECRYPT
 * \param input    8-byte input block
 * \param output   8-byte output block
 *
 * \return         0 if successful
 */
int blowfish_crypt_ecb( blowfish_context *ctx,
                        int mode,
                        const unsigned char input[BLOWFISH_BLOCKSIZE],
                        unsigned char output[BLOWFISH_BLOCKSIZE] );

#if defined(POLARSSL_CIPHER_MODE_CBC)
/**
 * \brief          Blowfish-CBC buffer encryption/decryption
 *                 Length should be a multiple of the block
 *                 size (8 bytes)
 *
 * \param ctx      Blowfish context
 * \param mode     BLOWFISH_ENCRYPT or BLOWFISH_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful, or
 *                 POLARSSL_ERR_BLOWFISH_INVALID_INPUT_LENGTH
 */
int blowfish_crypt_cbc( blowfish_context *ctx,
                        int mode,
                        size_t length,
                        unsigned char iv[BLOWFISH_BLOCKSIZE],
                        const unsigned char *input,
                        unsigned char *output );
#endif /* POLARSSL_CIPHER_MODE_CBC */

#if defined(POLARSSL_CIPHER_MODE_CFB)
/**
 * \brief          Blowfish CFB buffer encryption/decryption.
 *
 * \param ctx      Blowfish context
 * \param mode     BLOWFISH_ENCRYPT or BLOWFISH_DECRYPT
 * \param length   length of the input data
 * \param iv_off   offset in IV (updated after use)
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int blowfish_crypt_cfb64( blowfish_context *ctx,
                          int mode,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[BLOWFISH_BLOCKSIZE],
                          const unsigned char *input,
                          unsigned char *output );
#endif /*POLARSSL_CIPHER_MODE_CFB */

#if defined(POLARSSL_CIPHER_MODE_CTR)
/**
 * \brief               Blowfish-CTR buffer encryption/decryption
 *
 * Warning: You have to keep the maximum use of your counter in mind!
 *
 * \param ctx           Blowfish context
 * \param length        The length of the data
 * \param nc_off        The offset in the current stream_block (for resuming
 *                      within current cipher stream). The offset pointer to
 *                      should be 0 at the start of a stream.
 * \param nonce_counter The 64-bit nonce and counter.
 * \param stream_block  The saved stream-block for resuming. Is overwritten
 *                      by the function.
 * \param input         The input data stream
 * \param output        The output data stream
 *
 * \return         0 if successful
 */
int blowfish_crypt_ctr( blowfish_context *ctx,
                        size_t length,
                        size_t *nc_off,
                        unsigned char nonce_counter[BLOWFISH_BLOCKSIZE],
                        unsigned char stream_block[BLOWFISH_BLOCKSIZE],
                        const unsigned char *input,
                        unsigned char *output );
#endif /* POLARSSL_CIPHER_MODE_CTR */

#ifdef __cplusplus
}
#endif

#else  /* POLARSSL_BLOWFISH_ALT */
#include "blowfish_alt.h"
#endif /* POLARSSL_BLOWFISH_ALT */

#endif /* blowfish.h */
