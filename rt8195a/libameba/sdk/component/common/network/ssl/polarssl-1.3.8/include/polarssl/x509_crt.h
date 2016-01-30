/**
 * \file x509_crt.h
 *
 * \brief X.509 certificate parsing and writing
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
#ifndef POLARSSL_X509_CRT_H
#define POLARSSL_X509_CRT_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include "x509.h"

#include "x509_crl.h"

/**
 * \addtogroup x509_module
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name Structures and functions for parsing and writing X.509 certificates
 * \{
 */

/**
 * Container for an X.509 certificate. The certificate may be chained.
 */
typedef struct _x509_crt
{
    x509_buf raw;               /**< The raw certificate data (DER). */
    x509_buf tbs;               /**< The raw certificate body (DER). The part that is To Be Signed. */

    int version;                /**< The X.509 version. (1=v1, 2=v2, 3=v3) */
    x509_buf serial;            /**< Unique id for certificate issued by a specific CA. */
    x509_buf sig_oid1;          /**< Signature algorithm, e.g. sha1RSA */

    x509_buf issuer_raw;        /**< The raw issuer data (DER). Used for quick comparison. */
    x509_buf subject_raw;       /**< The raw subject data (DER). Used for quick comparison. */

    x509_name issuer;           /**< The parsed issuer data (named information object). */
    x509_name subject;          /**< The parsed subject data (named information object). */

    x509_time valid_from;       /**< Start time of certificate validity. */
    x509_time valid_to;         /**< End time of certificate validity. */

    pk_context pk;              /**< Container for the public key context. */

    x509_buf issuer_id;         /**< Optional X.509 v2/v3 issuer unique identifier. */
    x509_buf subject_id;        /**< Optional X.509 v2/v3 subject unique identifier. */
    x509_buf v3_ext;            /**< Optional X.509 v3 extensions.  */
    x509_sequence subject_alt_names;    /**< Optional list of Subject Alternative Names (Only dNSName supported). */

    int ext_types;              /**< Bit string containing detected and parsed extensions */
    int ca_istrue;              /**< Optional Basic Constraint extension value: 1 if this certificate belongs to a CA, 0 otherwise. */
    int max_pathlen;            /**< Optional Basic Constraint extension value: The maximum path length to the root certificate. Path length is 1 higher than RFC 5280 'meaning', so 1+ */

    unsigned char key_usage;    /**< Optional key usage extension value: See the values in x509.h */

    x509_sequence ext_key_usage; /**< Optional list of extended key usage OIDs. */

    unsigned char ns_cert_type; /**< Optional Netscape certificate type extension value: See the values in x509.h */

    x509_buf sig_oid2;          /**< Signature algorithm. Must match sig_oid1. */
    x509_buf sig;               /**< Signature: hash of the tbs part signed with the private key. */
    md_type_t sig_md;           /**< Internal representation of the MD algorithm of the signature algorithm, e.g. POLARSSL_MD_SHA256 */
    pk_type_t sig_pk;           /**< Internal representation of the Public Key algorithm of the signature algorithm, e.g. POLARSSL_PK_RSA */
    void *sig_opts;             /**< Signature options to be passed to pk_verify_ext(), e.g. for RSASSA-PSS */

    struct _x509_crt *next;     /**< Next certificate in the CA-chain. */
}
x509_crt;

#define X509_CRT_VERSION_1              0
#define X509_CRT_VERSION_2              1
#define X509_CRT_VERSION_3              2

#define X509_RFC5280_MAX_SERIAL_LEN 32
#define X509_RFC5280_UTC_TIME_LEN   15

/**
 * Container for writing a certificate (CRT)
 */
typedef struct _x509write_cert
{
    int version;
    mpi serial;
    pk_context *subject_key;
    pk_context *issuer_key;
    asn1_named_data *subject;
    asn1_named_data *issuer;
    md_type_t md_alg;
    char not_before[X509_RFC5280_UTC_TIME_LEN + 1];
    char not_after[X509_RFC5280_UTC_TIME_LEN + 1];
    asn1_named_data *extensions;
}
x509write_cert;

#if defined(POLARSSL_X509_CRT_PARSE_C)
/**
 * \brief          Parse a single DER formatted certificate and add it
 *                 to the chained list.
 *
 * \param chain    points to the start of the chain
 * \param buf      buffer holding the certificate DER data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509_crt_parse_der( x509_crt *chain, const unsigned char *buf,
                        size_t buflen );

/**
 * \brief          Parse one or more certificates and add them
 *                 to the chained list. Parses permissively. If some
 *                 certificates can be parsed, the result is the number
 *                 of failed certificates it encountered. If none complete
 *                 correctly, the first error is returned.
 *
 * \param chain    points to the start of the chain
 * \param buf      buffer holding the certificate data
 * \param buflen   size of the buffer
 *
 * \return         0 if all certificates parsed successfully, a positive number
 *                 if partly successful or a specific X509 or PEM error code
 */
int x509_crt_parse( x509_crt *chain, const unsigned char *buf, size_t buflen );

#if defined(POLARSSL_FS_IO)
/**
 * \brief          Load one or more certificates and add them
 *                 to the chained list. Parses permissively. If some
 *                 certificates can be parsed, the result is the number
 *                 of failed certificates it encountered. If none complete
 *                 correctly, the first error is returned.
 *
 * \param chain    points to the start of the chain
 * \param path     filename to read the certificates from
 *
 * \return         0 if all certificates parsed successfully, a positive number
 *                 if partly successful or a specific X509 or PEM error code
 */
int x509_crt_parse_file( x509_crt *chain, const char *path );

/**
 * \brief          Load one or more certificate files from a path and add them
 *                 to the chained list. Parses permissively. If some
 *                 certificates can be parsed, the result is the number
 *                 of failed certificates it encountered. If none complete
 *                 correctly, the first error is returned.
 *
 * \warning        This function is NOT thread-safe unless
 *                 POLARSSL_THREADING_PTHREADS is defined. If you're using an
 *                 alternative threading implementation, you should either use
 *                 this function only in the main thread, or mutex it.
 *
 * \param chain    points to the start of the chain
 * \param path     directory / folder to read the certificate files from
 *
 * \return         0 if all certificates parsed successfully, a positive number
 *                 if partly successful or a specific X509 or PEM error code
 */
int x509_crt_parse_path( x509_crt *chain, const char *path );
#endif /* POLARSSL_FS_IO */

/**
 * \brief          Returns an informational string about the
 *                 certificate.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param prefix   A line prefix
 * \param crt      The X509 certificate to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509_crt_info( char *buf, size_t size, const char *prefix,
                   const x509_crt *crt );

/**
 * \brief          Verify the certificate signature
 *
 *                 The verify callback is a user-supplied callback that
 *                 can clear / modify / add flags for a certificate. If set,
 *                 the verification callback is called for each
 *                 certificate in the chain (from the trust-ca down to the
 *                 presented crt). The parameters for the callback are:
 *                 (void *parameter, x509_crt *crt, int certificate_depth,
 *                 int *flags). With the flags representing current flags for
 *                 that specific certificate and the certificate depth from
 *                 the bottom (Peer cert depth = 0).
 *
 *                 All flags left after returning from the callback
 *                 are also returned to the application. The function should
 *                 return 0 for anything but a fatal error.
 *
 * \param crt      a certificate to be verified
 * \param trust_ca the trusted CA chain
 * \param ca_crl   the CRL chain for trusted CA's
 * \param cn       expected Common Name (can be set to
 *                 NULL if the CN must not be verified)
 * \param flags    result of the verification
 * \param f_vrfy   verification function
 * \param p_vrfy   verification parameter
 *
 * \return         0 if successful or POLARSSL_ERR_X509_SIG_VERIFY_FAILED,
 *                 in which case *flags will have one or more of
 *                 the following values set:
 *                      BADCERT_EXPIRED --
 *                      BADCERT_REVOKED --
 *                      BADCERT_CN_MISMATCH --
 *                      BADCERT_NOT_TRUSTED
 *                 or another error in case of a fatal error encountered
 *                 during the verification process.
 */
int x509_crt_verify( x509_crt *crt,
                     x509_crt *trust_ca,
                     x509_crl *ca_crl,
                     const char *cn, int *flags,
                     int (*f_vrfy)(void *, x509_crt *, int, int *),
                     void *p_vrfy );

#if defined(POLARSSL_X509_CHECK_KEY_USAGE)
/**
 * \brief          Check usage of certificate against keyUsage extension.
 *
 * \param crt      Leaf certificate used.
 * \param usage    Intended usage(s) (eg KU_KEY_ENCIPHERMENT before using the
 *                 certificate to perform an RSA key exchange).
 *
 * \return         0 is these uses of the certificate are allowed,
 *                 POLARSSL_ERR_X509_BAD_INPUT_DATA if the keyUsage extension
 *                 is present but does not contain all the bits set in the
 *                 usage argument.
 *
 * \note           You should only call this function on leaf certificates, on
 *                 (intermediate) CAs the keyUsage extension is automatically
 *                 checked by \c x509_crt_verify().
 */
int x509_crt_check_key_usage( const x509_crt *crt, int usage );
#endif /* POLARSSL_X509_CHECK_KEY_USAGE) */

#if defined(POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE)
/**
 * \brief          Check usage of certificate against extentedJeyUsage.
 *
 * \param crt      Leaf certificate used.
 * \param usage_oid Intended usage (eg OID_SERVER_AUTH or OID_CLIENT_AUTH).
 * \param usage_len Length of usage_oid (eg given by OID_SIZE()).
 *
 * \return         0 is this use of the certificate is allowed,
 *                 POLARSSL_ERR_X509_BAD_INPUT_DATA if not.
 *
 * \note           Usually only makes sense on leaf certificates.
 */
int x509_crt_check_extended_key_usage( const x509_crt *crt,
                                       const char *usage_oid,
                                       size_t usage_len );
#endif /* POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE) */

#if defined(POLARSSL_X509_CRL_PARSE_C)
/**
 * \brief          Verify the certificate revocation status
 *
 * \param crt      a certificate to be verified
 * \param crl      the CRL to verify against
 *
 * \return         1 if the certificate is revoked, 0 otherwise
 *
 */
int x509_crt_revoked( const x509_crt *crt, const x509_crl *crl );
#endif /* POLARSSL_X509_CRL_PARSE_C */

/**
 * \brief          Initialize a certificate (chain)
 *
 * \param crt      Certificate chain to initialize
 */
void x509_crt_init( x509_crt *crt );

/**
 * \brief          Unallocate all certificate data
 *
 * \param crt      Certificate chain to free
 */
void x509_crt_free( x509_crt *crt );
#endif /* POLARSSL_X509_CRT_PARSE_C */

/* \} name */
/* \} addtogroup x509_module */

#if defined(POLARSSL_X509_CRT_WRITE_C)
/**
 * \brief           Initialize a CRT writing context
 *
 * \param ctx       CRT context to initialize
 */
void x509write_crt_init( x509write_cert *ctx );

/**
 * \brief           Set the verion for a Certificate
 *                  Default: X509_CRT_VERSION_3
 *
 * \param ctx       CRT context to use
 * \param version   version to set (X509_CRT_VERSION_1, X509_CRT_VERSION_2 or
 *                                  X509_CRT_VERSION_3)
 */
void x509write_crt_set_version( x509write_cert *ctx, int version );

/**
 * \brief           Set the serial number for a Certificate.
 *
 * \param ctx       CRT context to use
 * \param serial    serial number to set
 *
 * \return          0 if successful
 */
int x509write_crt_set_serial( x509write_cert *ctx, const mpi *serial );

/**
 * \brief           Set the validity period for a Certificate
 *                  Timestamps should be in string format for UTC timezone
 *                  i.e. "YYYYMMDDhhmmss"
 *                  e.g. "20131231235959" for December 31st 2013
 *                       at 23:59:59
 *
 * \param ctx       CRT context to use
 * \param not_before    not_before timestamp
 * \param not_after     not_after timestamp
 *
 * \return          0 if timestamp was parsed successfully, or
 *                  a specific error code
 */
int x509write_crt_set_validity( x509write_cert *ctx, const char *not_before,
                                const char *not_after );

/**
 * \brief           Set the issuer name for a Certificate
 *                  Issuer names should contain a comma-separated list
 *                  of OID types and values:
 *                  e.g. "C=NL,O=Offspark,CN=PolarSSL CA"
 *
 * \param ctx           CRT context to use
 * \param issuer_name   issuer name to set
 *
 * \return          0 if issuer name was parsed successfully, or
 *                  a specific error code
 */
int x509write_crt_set_issuer_name( x509write_cert *ctx,
                                   const char *issuer_name );

/**
 * \brief           Set the subject name for a Certificate
 *                  Subject names should contain a comma-separated list
 *                  of OID types and values:
 *                  e.g. "C=NL,O=Offspark,CN=PolarSSL Server 1"
 *
 * \param ctx           CRT context to use
 * \param subject_name  subject name to set
 *
 * \return          0 if subject name was parsed successfully, or
 *                  a specific error code
 */
int x509write_crt_set_subject_name( x509write_cert *ctx,
                                    const char *subject_name );

/**
 * \brief           Set the subject public key for the certificate
 *
 * \param ctx       CRT context to use
 * \param key       public key to include
 */
void x509write_crt_set_subject_key( x509write_cert *ctx, pk_context *key );

/**
 * \brief           Set the issuer key used for signing the certificate
 *
 * \param ctx       CRT context to use
 * \param key       private key to sign with
 */
void x509write_crt_set_issuer_key( x509write_cert *ctx, pk_context *key );

/**
 * \brief           Set the MD algorithm to use for the signature
 *                  (e.g. POLARSSL_MD_SHA1)
 *
 * \param ctx       CRT context to use
 * \param md_alg    MD algorithm to use
 */
void x509write_crt_set_md_alg( x509write_cert *ctx, md_type_t md_alg );

/**
 * \brief           Generic function to add to or replace an extension in the
 *                  CRT
 *
 * \param ctx       CRT context to use
 * \param oid       OID of the extension
 * \param oid_len   length of the OID
 * \param critical  if the extension is critical (per the RFC's definition)
 * \param val       value of the extension OCTET STRING
 * \param val_len   length of the value data
 *
 * \return          0 if successful, or a POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_crt_set_extension( x509write_cert *ctx,
                                 const char *oid, size_t oid_len,
                                 int critical,
                                 const unsigned char *val, size_t val_len );

/**
 * \brief           Set the basicConstraints extension for a CRT
 *
 * \param ctx       CRT context to use
 * \param is_ca     is this a CA certificate
 * \param max_pathlen   maximum length of certificate chains below this
 *                      certificate (only for CA certificates, -1 is
 *                      inlimited)
 *
 * \return          0 if successful, or a POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_crt_set_basic_constraints( x509write_cert *ctx,
                                         int is_ca, int max_pathlen );

#if defined(POLARSSL_SHA1_C)
/**
 * \brief           Set the subjectKeyIdentifier extension for a CRT
 *                  Requires that x509write_crt_set_subject_key() has been
 *                  called before
 *
 * \param ctx       CRT context to use
 *
 * \return          0 if successful, or a POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_crt_set_subject_key_identifier( x509write_cert *ctx );

/**
 * \brief           Set the authorityKeyIdentifier extension for a CRT
 *                  Requires that x509write_crt_set_issuer_key() has been
 *                  called before
 *
 * \param ctx       CRT context to use
 *
 * \return          0 if successful, or a POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_crt_set_authority_key_identifier( x509write_cert *ctx );
#endif /* POLARSSL_SHA1_C */

/**
 * \brief           Set the Key Usage Extension flags
 *                  (e.g. KU_DIGITAL_SIGNATURE | KU_KEY_CERT_SIGN)
 *
 * \param ctx       CRT context to use
 * \param key_usage key usage flags to set
 *
 * \return          0 if successful, or POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_crt_set_key_usage( x509write_cert *ctx, unsigned char key_usage );

/**
 * \brief           Set the Netscape Cert Type flags
 *                  (e.g. NS_CERT_TYPE_SSL_CLIENT | NS_CERT_TYPE_EMAIL)
 *
 * \param ctx           CRT context to use
 * \param ns_cert_type  Netscape Cert Type flags to set
 *
 * \return          0 if successful, or POLARSSL_ERR_X509WRITE_MALLOC_FAILED
 */
int x509write_crt_set_ns_cert_type( x509write_cert *ctx,
                                    unsigned char ns_cert_type );

/**
 * \brief           Free the contents of a CRT write context
 *
 * \param ctx       CRT context to free
 */
void x509write_crt_free( x509write_cert *ctx );

/**
 * \brief           Write a built up certificate to a X509 DER structure
 *                  Note: data is written at the end of the buffer! Use the
 *                        return value to determine where you should start
 *                        using the buffer
 *
 * \param ctx       certificate to write away
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
int x509write_crt_der( x509write_cert *ctx, unsigned char *buf, size_t size,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng );

#if defined(POLARSSL_PEM_WRITE_C)
/**
 * \brief           Write a built up certificate to a X509 PEM string
 *
 * \param ctx       certificate to write away
 * \param buf       buffer to write to
 * \param size      size of the buffer
 * \param f_rng     RNG function (for signature, see note)
 * \param p_rng     RNG parameter
 *
 * \return          0 successful, or a specific error code
 *
 * \note            f_rng may be NULL if RSA is used for signature and the
 *                  signature is made offline (otherwise f_rng is desirable
 *                  for countermeasures against timing attacks).
 *                  ECDSA signatures always require a non-NULL f_rng.
 */
int x509write_crt_pem( x509write_cert *ctx, unsigned char *buf, size_t size,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng );
#endif /* POLARSSL_PEM_WRITE_C */
#endif /* POLARSSL_X509_CRT_WRITE_C */

#ifdef __cplusplus
}
#endif

#endif /* x509_crt.h */
