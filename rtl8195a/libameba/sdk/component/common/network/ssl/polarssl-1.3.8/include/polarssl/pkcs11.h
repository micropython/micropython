/**
 * \file pkcs11.h
 *
 * \brief Wrapper for PKCS#11 library libpkcs11-helper
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
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
#ifndef POLARSSL_PKCS11_H
#define POLARSSL_PKCS11_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_PKCS11_C)

#include "x509_crt.h"

#include <pkcs11-helper-1.0/pkcs11h-certificate.h>

#if defined(_MSC_VER) && !defined(inline)
#define inline _inline
#else
#if defined(__ARMCC_VERSION) && !defined(inline)
#define inline __inline
#endif /* __ARMCC_VERSION */
#endif /*_MSC_VER */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context for PKCS #11 private keys.
 */
typedef struct {
        pkcs11h_certificate_t pkcs11h_cert;
        int len;
} pkcs11_context;

/**
 * Fill in a PolarSSL certificate, based on the given PKCS11 helper certificate.
 *
 * \param cert          X.509 certificate to fill
 * \param pkcs11h_cert  PKCS #11 helper certificate
 *
 * \return              0 on success.
 */
int pkcs11_x509_cert_init( x509_crt *cert, pkcs11h_certificate_t pkcs11h_cert );

/**
 * Initialise a pkcs11_context, storing the given certificate. Note that the
 * pkcs11_context will take over control of the certificate, freeing it when
 * done.
 *
 * \param priv_key      Private key structure to fill.
 * \param pkcs11_cert   PKCS #11 helper certificate
 *
 * \return              0 on success
 */
int pkcs11_priv_key_init( pkcs11_context *priv_key,
        pkcs11h_certificate_t pkcs11_cert );

/**
 * Free the contents of the given private key context. Note that the structure
 * itself is not freed.
 *
 * \param priv_key      Private key structure to cleanup
 */
void pkcs11_priv_key_free( pkcs11_context *priv_key );

/**
 * \brief          Do an RSA private key decrypt, then remove the message
 *                 padding
 *
 * \param ctx      PKCS #11 context
 * \param mode     must be RSA_PRIVATE, for compatibility with rsa.c's signature
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param olen     will contain the plaintext length
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an POLARSSL_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int pkcs11_decrypt( pkcs11_context *ctx,
                       int mode, size_t *olen,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t output_max_len );

/**
 * \brief          Do a private RSA to sign a message digest
 *
 * \param ctx      PKCS #11 context
 * \param mode     must be RSA_PRIVATE, for compatibility with rsa.c's signature
 * \param md_alg   a POLARSSL_MD_* (use POLARSSL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for POLARSSL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an POLARSSL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int pkcs11_sign( pkcs11_context *ctx,
                    int mode,
                    md_type_t md_alg,
                    unsigned int hashlen,
                    const unsigned char *hash,
                    unsigned char *sig );

/**
 * SSL/TLS wrappers for PKCS#11 functions
 */
static inline int ssl_pkcs11_decrypt( void *ctx, int mode, size_t *olen,
                        const unsigned char *input, unsigned char *output,
                        size_t output_max_len )
{
    return pkcs11_decrypt( (pkcs11_context *) ctx, mode, olen, input, output,
                           output_max_len );
}

static inline int ssl_pkcs11_sign( void *ctx,
                     int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                     int mode, md_type_t md_alg, unsigned int hashlen,
                     const unsigned char *hash, unsigned char *sig )
{
    ((void) f_rng);
    ((void) p_rng);
    return pkcs11_sign( (pkcs11_context *) ctx, mode, md_alg,
                        hashlen, hash, sig );
}

static inline size_t ssl_pkcs11_key_len( void *ctx )
{
    return ( (pkcs11_context *) ctx )->len;
}

#ifdef __cplusplus
}
#endif

#endif /* POLARSSL_PKCS11_C */

#endif /* POLARSSL_PKCS11_H */
