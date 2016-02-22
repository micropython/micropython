/**
 * \file pem.h
 *
 * \brief Privacy Enhanced Mail (PEM) decoding
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
#ifndef POLARSSL_PEM_H
#define POLARSSL_PEM_H

#include <string.h>

/**
 * \name PEM Error codes
 * These error codes are returned in case of errors reading the
 * PEM data.
 * \{
 */
#define POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT          -0x1080  /**< No PEM header or footer found. */
#define POLARSSL_ERR_PEM_INVALID_DATA                      -0x1100  /**< PEM string is not as expected. */
#define POLARSSL_ERR_PEM_MALLOC_FAILED                     -0x1180  /**< Failed to allocate memory. */
#define POLARSSL_ERR_PEM_INVALID_ENC_IV                    -0x1200  /**< RSA IV is not in hex-format. */
#define POLARSSL_ERR_PEM_UNKNOWN_ENC_ALG                   -0x1280  /**< Unsupported key encryption algorithm. */
#define POLARSSL_ERR_PEM_PASSWORD_REQUIRED                 -0x1300  /**< Private key password can't be empty. */
#define POLARSSL_ERR_PEM_PASSWORD_MISMATCH                 -0x1380  /**< Given private key password does not allow for correct decryption. */
#define POLARSSL_ERR_PEM_FEATURE_UNAVAILABLE               -0x1400  /**< Unavailable feature, e.g. hashing/encryption combination. */
#define POLARSSL_ERR_PEM_BAD_INPUT_DATA                    -0x1480  /**< Bad input parameters to function. */
/* \} name */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(POLARSSL_PEM_PARSE_C)
/**
 * \brief       PEM context structure
 */
typedef struct
{
    unsigned char *buf;     /*!< buffer for decoded data             */
    size_t buflen;          /*!< length of the buffer                */
    unsigned char *info;    /*!< buffer for extra header information */
}
pem_context;

/**
 * \brief       PEM context setup
 *
 * \param ctx   context to be initialized
 */
void pem_init( pem_context *ctx );

/**
 * \brief       Read a buffer for PEM information and store the resulting
 *              data into the specified context buffers.
 *
 * \param ctx       context to use
 * \param header    header string to seek and expect
 * \param footer    footer string to seek and expect
 * \param data      source data to look in
 * \param pwd       password for decryption (can be NULL)
 * \param pwdlen    length of password
 * \param use_len   destination for total length used (set after header is
 *                  correctly read, so unless you get
 *                  POLARSSL_ERR_PEM_BAD_INPUT_DATA or
 *                  POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT, use_len is
 *                  the length to skip)
 *
 * \note            Attempts to check password correctness by verifying if
 *                  the decrypted text starts with an ASN.1 sequence of
 *                  appropriate length
 *
 * \return          0 on success, or a specific PEM error code
 */
int pem_read_buffer( pem_context *ctx, const char *header, const char *footer,
                     const unsigned char *data,
                     const unsigned char *pwd,
                     size_t pwdlen, size_t *use_len );

/**
 * \brief       PEM context memory freeing
 *
 * \param ctx   context to be freed
 */
void pem_free( pem_context *ctx );
#endif /* POLARSSL_PEM_PARSE_C */

#if defined(POLARSSL_PEM_WRITE_C)
/**
 * \brief           Write a buffer of PEM information from a DER encoded
 *                  buffer.
 *
 * \param header    header string to write
 * \param footer    footer string to write
 * \param der_data  DER data to write
 * \param der_len   length of the DER data
 * \param buf       buffer to write to
 * \param buf_len   length of output buffer
 * \param olen      total length written / required (if buf_len is not enough)
 *
 * \return          0 on success, or a specific PEM or BASE64 error code. On
 *                  POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL olen is the required
 *                  size.
 */
int pem_write_buffer( const char *header, const char *footer,
                      const unsigned char *der_data, size_t der_len,
                      unsigned char *buf, size_t buf_len, size_t *olen );
#endif /* POLARSSL_PEM_WRITE_C */

#ifdef __cplusplus
}
#endif

#endif /* pem.h */
