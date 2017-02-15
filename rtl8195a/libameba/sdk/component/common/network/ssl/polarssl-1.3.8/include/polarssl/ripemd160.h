/**
 * \file ripemd160.h
 *
 * \brief RIPE MD-160 message digest
 *
 *  Copyright (C) 2014-2014, Brainspark B.V.
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
#ifndef POLARSSL_RIPEMD160_H
#define POLARSSL_RIPEMD160_H

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

#define POLARSSL_ERR_RIPEMD160_FILE_IO_ERROR              -0x007E  /**< Read/write error in file. */

#if !defined(POLARSSL_RIPEMD160_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          RIPEMD-160 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
}
ripemd160_context;

/**
 * \brief          Initialize RIPEMD-160 context
 *
 * \param ctx      RIPEMD-160 context to be initialized
 */
void ripemd160_init( ripemd160_context *ctx );

/**
 * \brief          Clear RIPEMD-160 context
 *
 * \param ctx      RIPEMD-160 context to be cleared
 */
void ripemd160_free( ripemd160_context *ctx );

/**
 * \brief          RIPEMD-160 context setup
 *
 * \param ctx      context to be initialized
 */
void ripemd160_starts( ripemd160_context *ctx );

/**
 * \brief          RIPEMD-160 process buffer
 *
 * \param ctx      RIPEMD-160 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void ripemd160_update( ripemd160_context *ctx,
                       const unsigned char *input, size_t ilen );

/**
 * \brief          RIPEMD-160 final digest
 *
 * \param ctx      RIPEMD-160 context
 * \param output   RIPEMD-160 checksum result
 */
void ripemd160_finish( ripemd160_context *ctx, unsigned char output[20] );

/* Internal use */
void ripemd160_process( ripemd160_context *ctx, const unsigned char data[64] );

#ifdef __cplusplus
}
#endif

#else  /* POLARSSL_RIPEMD160_ALT */
#include "ripemd160.h"
#endif /* POLARSSL_RIPEMD160_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Output = RIPEMD-160( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   RIPEMD-160 checksum result
 */
void ripemd160( const unsigned char *input, size_t ilen,
                unsigned char output[20] );

#if defined(POLARSSL_FS_IO)
/**
 * \brief          Output = RIPEMD-160( file contents )
 *
 * \param path     input file name
 * \param output   RIPEMD-160 checksum result
 *
 * \return         0 if successful, or POLARSSL_ERR_RIPEMD160_FILE_IO_ERROR
 */
int ripemd160_file( const char *path, unsigned char output[20] );
#endif /* POLARSSL_FS_IO */

/**
 * \brief          RIPEMD-160 HMAC context setup
 *
 * \param ctx      HMAC context to be initialized
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 */
void ripemd160_hmac_starts( ripemd160_context *ctx,
                            const unsigned char *key, size_t keylen );

/**
 * \brief          RIPEMD-160 HMAC process buffer
 *
 * \param ctx      HMAC context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void ripemd160_hmac_update( ripemd160_context *ctx,
                            const unsigned char *input, size_t ilen );

/**
 * \brief          RIPEMD-160 HMAC final digest
 *
 * \param ctx      HMAC context
 * \param output   RIPEMD-160 HMAC checksum result
 */
void ripemd160_hmac_finish( ripemd160_context *ctx, unsigned char output[20] );

/**
 * \brief          RIPEMD-160 HMAC context reset
 *
 * \param ctx      HMAC context to be reset
 */
void ripemd160_hmac_reset( ripemd160_context *ctx );

/**
 * \brief          Output = HMAC-RIPEMD-160( hmac key, input buffer )
 *
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   HMAC-RIPEMD-160 result
 */
void ripemd160_hmac( const unsigned char *key, size_t keylen,
                     const unsigned char *input, size_t ilen,
                     unsigned char output[20] );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int ripemd160_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* ripemd160.h */
