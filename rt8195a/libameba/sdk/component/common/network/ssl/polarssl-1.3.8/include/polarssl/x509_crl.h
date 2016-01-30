/**
 * \file x509_crl.h
 *
 * \brief X.509 certificate revocation list parsing
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
#ifndef POLARSSL_X509_CRL_H
#define POLARSSL_X509_CRL_H

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
 * \name Structures and functions for parsing CRLs
 * \{
 */

/**
 * Certificate revocation list entry.
 * Contains the CA-specific serial numbers and revocation dates.
 */
typedef struct _x509_crl_entry
{
    x509_buf raw;

    x509_buf serial;

    x509_time revocation_date;

    x509_buf entry_ext;

    struct _x509_crl_entry *next;
}
x509_crl_entry;

/**
 * Certificate revocation list structure.
 * Every CRL may have multiple entries.
 */
typedef struct _x509_crl
{
    x509_buf raw;           /**< The raw certificate data (DER). */
    x509_buf tbs;           /**< The raw certificate body (DER). The part that is To Be Signed. */

    int version;            /**< CRL version (1=v1, 2=v2) */
    x509_buf sig_oid1;

    x509_buf issuer_raw;    /**< The raw issuer data (DER). */

    x509_name issuer;       /**< The parsed issuer data (named information object). */

    x509_time this_update;
    x509_time next_update;

    x509_crl_entry entry;   /**< The CRL entries containing the certificate revocation times for this CA. */

    x509_buf crl_ext;

    x509_buf sig_oid2;
    x509_buf sig;
    md_type_t sig_md;           /**< Internal representation of the MD algorithm of the signature algorithm, e.g. POLARSSL_MD_SHA256 */
    pk_type_t sig_pk;           /**< Internal representation of the Public Key algorithm of the signature algorithm, e.g. POLARSSL_PK_RSA */
    void *sig_opts;             /**< Signature options to be passed to pk_verify_ext(), e.g. for RSASSA-PSS */

    struct _x509_crl *next;
}
x509_crl;

/**
 * \brief          Parse one or more CRLs and add them
 *                 to the chained list
 *
 * \param chain    points to the start of the chain
 * \param buf      buffer holding the CRL data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509_crl_parse( x509_crl *chain, const unsigned char *buf, size_t buflen );

#if defined(POLARSSL_FS_IO)
/**
 * \brief          Load one or more CRLs and add them
 *                 to the chained list
 *
 * \param chain    points to the start of the chain
 * \param path     filename to read the CRLs from
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509_crl_parse_file( x509_crl *chain, const char *path );
#endif /* POLARSSL_FS_IO */

/**
 * \brief          Returns an informational string about the CRL.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param prefix   A line prefix
 * \param crl      The X509 CRL to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509_crl_info( char *buf, size_t size, const char *prefix,
                   const x509_crl *crl );

/**
 * \brief          Initialize a CRL (chain)
 *
 * \param crl      CRL chain to initialize
 */
void x509_crl_init( x509_crl *crl );

/**
 * \brief          Unallocate all CRL data
 *
 * \param crl      CRL chain to free
 */
void x509_crl_free( x509_crl *crl );

/* \} name */
/* \} addtogroup x509_module */

#ifdef __cplusplus
}
#endif

#endif /* x509_crl.h */
