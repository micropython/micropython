/**
 * \file x509_csr.h
 *
 * \brief X.509 certificate signing request parsing and writing
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
#ifndef POLARSSL_X509_CSR_H
#define POLARSSL_X509_CSR_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include "x509.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup x509_module
 * \{ */

/**
 * \name Structures and functions for X.509 Certificate Signing Requests (CSR)
 * \{
 */

/**
 * Certificate Signing Request (CSR) structure.
 */
typedef struct _x509_csr
{
    x509_buf raw;           /**< The raw CSR data (DER). */
    x509_buf cri;           /**< The raw CertificateRequestInfo body (DER). */

    int version;            /**< CSR version (1=v1). */

    x509_buf  subject_raw;  /**< The raw subject data (DER). */
    x509_name subject;      /**< The parsed subject data (named information object). */

    pk_context pk;          /**< Container for the public key context. */

    x509_buf sig_oid;
    x509_buf sig;
    md_type_t sig_md;       /**< Internal representation of the MD algorithm of the signature algorithm, e.g. POLARSSL_MD_SHA256 */
    pk_type_t sig_pk;       /**< Internal representation of the Public Key algorithm of the signature algorithm, e.g. POLARSSL_PK_RSA */
    void *sig_opts;         /**< Signature options to be passed to pk_verify_ext(), e.g. for RSASSA-PSS */
}
x509_csr;

/**
 * Container for writing a CSR
 */
typedef struct _x509write_csr
{
    pk_context *key;
    asn1_named_data *subject;
    md_type_t md_alg;
    asn1_named_data *extensions;
}
x509write_csr;

#if defined(POLARSSL_X509_CSR_PARSE_C)
/**
 * \brief          Load a Certificate Signing Request (CSR) in DER format
 *
 * \param csr      CSR context to fill
 * \param buf      buffer holding the CRL data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 error code
 */
int x509_csr_parse_der( x509_csr *csr,
                        const unsigned char *buf, size_t buflen );

/**
 * \brief          Load a Certificate Signing Request (CSR), DER or PEM format
 *
 * \param csr      CSR context to fill
 * \param buf      buffer holding the CRL data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509_csr_parse( x509_csr *csr, const unsigned char *buf, size_t buflen );

#if defined(POLARSSL_FS_IO)
/**
 * \brief          Load a Certificate Signing Request (CSR)
 *
 * \param csr      CSR context to fill
 * \param path     filename to read the CSR from
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509_csr_parse_file( x509_csr *csr, const char *path );
#endif /* POLARSSL_FS_IO */

/**
 * \brief          Returns an informational string about the
 *                 CSR.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param prefix   A line prefix
 * \param csr      The X509 CSR to represent
 *
 * \return         The length of the string written (exluding the terminating
 *                 null byte), or a negative value in case of an error.
 */
int x509_csr_info( char *buf, size_t size, const char *prefix,
                   const x509_csr *csr );

/**
 * \brief          Initialize a CSR
 *
 * \param csr      CSR to initialize
 */
void x509_csr_init( x509_csr *csr );

/**
 * \brief          Unallocate all CSR data
 *
 * \param csr      CSR to free
 */
void x509_csr_free( x509_csr *csr );
#endif /* POLARSSL_X509_CSR_PARSE_C */

/* \} name */
/* \} addtogroup x509_module */

#if defined(POLARSSL_X509_CSR_WRITE_C)
/**
 * \brief           Initialize a CSR context
 *
 * \param ctx       CSR context to initialize
 */
void x509write_csr_init( x509write_csr *ctx );

/**
 * \brief           Set the subject name for a CSR
 *                  Subject names should contain a comma-separated list
 *                  of OID types and values:
 *                  e.g. "C=NL,O=Offspark,CN=PolarSSL Server 1"
 *
 * \param ctx           CSR context to use
 * \param subject_name  subject name to set
 *
 * \return          0 if subject name was parsed successfully, or
 *                  a specific error code
 */
int x509write_csr_set_subject_name( x509write_csr *ctx,
                                    const char *subject_name );

/**
 * \brief           Set the key for a CSR (public key will be included,
 *                  private key used to sign the CSR when writing it)
 *
 * \param ctx       CSR context to use
 * \param key       Asymetric key to include
 */
void x509write_csr_set_key( x509write_csr *ctx, pk_context *key );

/**
 * \brief           Set the MD algorithm to use for the signature
 *                  (e.g. POLARSSL_MD_SHA1)
 *
 * \param ctx       CSR context to use
 * \param md_alg    MD algorithm to use
 */
void x509write_csr_set_md_alg( x509write_csr *ctx, md_type_t md_alg );

/**
 * \brief           Set the Key Usage Extension flags
 *                  (e.g. KU_DIGITAL_SIGNATURE | KU_KEY_CERT_SIGN)
 *
 * \param ctx       CSR context to use
 * \param key_usage key usage flags to set
 *
 * \return          0 if successful, or POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_csr_set_key_usage( x509write_csr *ctx, unsigned char key_usage );

/**
 * \brief           Set the Netscape Cert Type flags
 *                  (e.g. NS_CERT_TYPE_SSL_CLIENT | NS_CERT_TYPE_EMAIL)
 *
 * \param ctx           CSR context to use
 * \param ns_cert_type  Netscape Cert Type flags to set
 *
 * \return          0 if successful, or POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_csr_set_ns_cert_type( x509write_csr *ctx,
                                    unsigned char ns_cert_type );

/**
 * \brief           Generic function to add to or replace an extension in the
 *                  CSR
 *
 * \param ctx       CSR context to use
 * \param oid       OID of the extension
 * \param oid_len   length of the OID
 * \param val       value of the extension OCTET STRING
 * \param val_len   length of the value data
 *
 * \return          0 if successful, or a POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_csr_set_extension( x509write_csr *ctx,
                                 const char *oid, size_t oid_len,
                                 const unsigned char *val, size_t val_len );

/**
 * \brief           Free the contents of a CSR context
 *
 * \param ctx       CSR context to free
 */
void x509write_csr_free( x509write_csr *ctx );

/**
 * \brief           Write a CSR (Certificate Signing Request) to a
 *                  DER structure
 *                  Note: data is written at the end of the buffer! Use the
 *                        return value to determine where you should start
 *                        using the buffer
 *
 * \param ctx       CSR to write away
 * \param buf       buffer to write to
 * \param size      size of the buffer
 * \param f_rng     RNG function (for signature, see note)
 * \param p_rng     RNG parameter
 *
 * \return          length of data written if successful, or a specific
 *                  error code
 *
 * \note            f_rng may be NULL if RSA is used for signature and the
 *                  signature is made offline (otherwise f_rng is desirable
 *                  for countermeasures against timing attacks).
 *                  ECDSA signatures always require a non-NULL f_rng.
 */
int x509write_csr_der( x509write_csr *ctx, unsigned char *buf, size_t size,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng );

#if defined(POLARSSL_PEM_WRITE_C)
/**
 * \brief           Write a CSR (Certificate Signing Request) to a
 *                  PEM string
 *
 * \param ctx       CSR to write away
 * \param buf       buffer to write to
 * \param size      size of the buffer
 * \param f_rng     RNG function (for signature, see note)
 * \param p_rng     RNG parameter
 *
 * \return          0 successful, or a specific error code
 *
 * \note            f_rng may be NULL if RSA is used for signature and the
 *                  signature is made offline (otherwise f_rng is desirable
 *                  for couermeasures against timing attacks).
 *                  ECDSA signatures always require a non-NULL f_rng.
 */
int x509write_csr_pem( x509write_csr *ctx, unsigned char *buf, size_t size,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng );
#endif /* POLARSSL_PEM_WRITE_C */
#endif /* POLARSSL_X509_CSR_WRITE_C */

#ifdef __cplusplus
}
#endif

#endif /* x509_csr.h */
