/**
 * \file md2.h
 *
 * \brief MD2 message digest algorithm (hash function)
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
#ifndef POLARSSL_MD2_H
#define POLARSSL_MD2_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <string.h>

#define POLARSSL_ERR_MD2_FILE_IO_ERROR                 -0x0070  /**< Read/write error in file. */

#if !defined(POLARSSL_MD2_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD2 context structure
 */
typedef struct
{
    unsigned char cksum[16];    /*!< checksum of the data block */
    unsigned char state[48];    /*!< intermediate digest state  */
    unsigned char buffer[16];   /*!< data block being processed */

    unsigned char ipad[16];     /*!< HMAC: inner padding        */
    unsigned char opad[16];     /*!< HMAC: outer padding        */
    size_t left;                /*!< amount of data in buffer   */
}
md2_context;

/**
 * \brief          Initialize MD2 context
 *
 * \param ctx      MD2 context to be initialized
 */
void md2_init( md2_context *ctx );

/**
 * \brief          Clear MD2 context
 *
 * \param ctx      MD2 context to be cleared
 */
void md2_free( md2_context *ctx );

/**
 * \brief          MD2 context setup
 *
 * \param ctx      context to be initialized
 */
void md2_starts( md2_context *ctx );

/**
 * \brief          MD2 process buffer
 *
 * \param ctx      MD2 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void md2_update( md2_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          MD2 final digest
 *
 * \param ctx      MD2 context
 * \param output   MD2 checksum result
 */
void md2_finish( md2_context *ctx, unsigned char output[16] );

#ifdef __cplusplus
}
#endif

#else  /* POLARSSL_MD2_ALT */
#include "md2_alt.h"
#endif /* POLARSSL_MD2_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Output = MD2( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD2 checksum result
 */
void md2( const unsigned char *input, size_t ilen, unsigned char output[16] );

/**
 * \brief          Output = MD2( file contents )
 *
 * \param path     input file name
 * \param output   MD2 checksum result
 *
 * \return         0 if successful, or POLARSSL_ERR_MD2_FILE_IO_ERROR
 */
int md2_file( const char *path, unsigned char output[16] );

/**
 * \brief          MD2 HMAC context setup
 *
 * \param ctx      HMAC context to be initialized
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 */
void md2_hmac_starts( md2_context *ctx, const unsigned char *key,
                      size_t keylen );

/**
 * \brief          MD2 HMAC process buffer
 *
 * \param ctx      HMAC context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void md2_hmac_update( md2_context *ctx, const unsigned char *input,
                      size_t ilen );

/**
 * \brief          MD2 HMAC final digest
 *
 * \param ctx      HMAC context
 * \param output   MD2 HMAC checksum result
 */
void md2_hmac_finish( md2_context *ctx, unsigned char output[16] );

/**
 * \brief          MD2 HMAC context reset
 *
 * \param ctx      HMAC context to be reset
 */
void md2_hmac_reset( md2_context *ctx );

/**
 * \brief          Output = HMAC-MD2( hmac key, input buffer )
 *
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   HMAC-MD2 result
 */
void md2_hmac( const unsigned char *key, size_t keylen,
               const unsigned char *input, size_t ilen,
               unsigned char output[16] );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int md2_self_test( int verbose );

/* Internal use */
void md2_process( md2_context *ctx );

#ifdef __cplusplus
}
#endif

#endif /* md2.h */
