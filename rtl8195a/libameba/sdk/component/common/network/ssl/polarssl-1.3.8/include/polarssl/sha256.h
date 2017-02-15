/**
 * \file sha256.h
 *
 * \brief SHA-224 and SHA-256 cryptographic hash function
 *
 *  Copyright (C) 2006-2014, Brainspark B.V.
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
#ifndef POLARSSL_SHA256_H
#define POLARSSL_SHA256_H

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

#define POLARSSL_ERR_SHA256_FILE_IO_ERROR              -0x0078  /**< Read/write error in file. */

#if !defined(POLARSSL_SHA256_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-256 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
    int is224;                  /*!< 0 => SHA-256, else SHA-224 */
}
sha256_context;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void sha256_init( sha256_context *ctx );

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void sha256_free( sha256_context *ctx );

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void sha256_starts( sha256_context *ctx, int is224 );

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha256_update( sha256_context *ctx, const unsigned char *input,
                    size_t ilen );

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void sha256_finish( sha256_context *ctx, unsigned char output[32] );

/* Internal use */
void sha256_process( sha256_context *ctx, const unsigned char data[64] );

#ifdef __cplusplus
}
#endif

#else  /* POLARSSL_SHA256_ALT */
#include "sha256_alt.h"
#endif /* POLARSSL_SHA256_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void sha256( const unsigned char *input, size_t ilen,
           unsigned char output[32], int is224 );

/**
 * \brief          Output = SHA-256( file contents )
 *
 * \param path     input file name
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 *
 * \return         0 if successful, or POLARSSL_ERR_SHA256_FILE_IO_ERROR
 */
int sha256_file( const char *path, unsigned char output[32], int is224 );

/**
 * \brief          SHA-256 HMAC context setup
 *
 * \param ctx      HMAC context to be initialized
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void sha256_hmac_starts( sha256_context *ctx, const unsigned char *key,
                         size_t keylen, int is224 );

/**
 * \brief          SHA-256 HMAC process buffer
 *
 * \param ctx      HMAC context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha256_hmac_update( sha256_context *ctx, const unsigned char *input,
                         size_t ilen );

/**
 * \brief          SHA-256 HMAC final digest
 *
 * \param ctx      HMAC context
 * \param output   SHA-224/256 HMAC checksum result
 */
void sha256_hmac_finish( sha256_context *ctx, unsigned char output[32] );

/**
 * \brief          SHA-256 HMAC context reset
 *
 * \param ctx      HMAC context to be reset
 */
void sha256_hmac_reset( sha256_context *ctx );

/**
 * \brief          Output = HMAC-SHA-256( hmac key, input buffer )
 *
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   HMAC-SHA-224/256 result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void sha256_hmac( const unsigned char *key, size_t keylen,
                  const unsigned char *input, size_t ilen,
                  unsigned char output[32], int is224 );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int sha256_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* sha256.h */
