/**
 * \file sha1.h
 *
 * \brief SHA-1 cryptographic hash function
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
#ifndef POLARSSL_SHA1_H
#define POLARSSL_SHA1_H

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

#define POLARSSL_ERR_SHA1_FILE_IO_ERROR                -0x0076  /**< Read/write error in file. */

#if !defined(POLARSSL_SHA1_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-1 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
}
sha1_context;

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void sha1_init( sha1_context *ctx );

/**
 * \brief          Clear SHA-1 context
 *
 * \param ctx      SHA-1 context to be cleared
 */
void sha1_free( sha1_context *ctx );

/**
 * \brief          SHA-1 context setup
 *
 * \param ctx      context to be initialized
 */
void sha1_starts( sha1_context *ctx );

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha1_update( sha1_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void sha1_finish( sha1_context *ctx, unsigned char output[20] );

/* Internal use */
void sha1_process( sha1_context *ctx, const unsigned char data[64] );

#ifdef __cplusplus
}
#endif

#else  /* POLARSSL_SHA1_ALT */
#include "sha1_alt.h"
#endif /* POLARSSL_SHA1_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Output = SHA-1( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-1 checksum result
 */
void sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );

/**
 * \brief          Output = SHA-1( file contents )
 *
 * \param path     input file name
 * \param output   SHA-1 checksum result
 *
 * \return         0 if successful, or POLARSSL_ERR_SHA1_FILE_IO_ERROR
 */
int sha1_file( const char *path, unsigned char output[20] );

/**
 * \brief          SHA-1 HMAC context setup
 *
 * \param ctx      HMAC context to be initialized
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 */
void sha1_hmac_starts( sha1_context *ctx, const unsigned char *key,
                       size_t keylen );

/**
 * \brief          SHA-1 HMAC process buffer
 *
 * \param ctx      HMAC context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha1_hmac_update( sha1_context *ctx, const unsigned char *input,
                       size_t ilen );

/**
 * \brief          SHA-1 HMAC final digest
 *
 * \param ctx      HMAC context
 * \param output   SHA-1 HMAC checksum result
 */
void sha1_hmac_finish( sha1_context *ctx, unsigned char output[20] );

/**
 * \brief          SHA-1 HMAC context reset
 *
 * \param ctx      HMAC context to be reset
 */
void sha1_hmac_reset( sha1_context *ctx );

/**
 * \brief          Output = HMAC-SHA-1( hmac key, input buffer )
 *
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   HMAC-SHA-1 result
 */
void sha1_hmac( const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char output[20] );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int sha1_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* sha1.h */
