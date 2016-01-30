/**
 * \file x509.h
 *
 * \brief X.509 generic defines and structures
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
#ifndef POLARSSL_X509_H
#define POLARSSL_X509_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include "asn1.h"
#include "pk.h"

#if defined(POLARSSL_RSA_C)
#include "rsa.h"
#endif

/**
 * \addtogroup x509_module
 * \{
 */

/**
 * \name X509 Error codes
 * \{
 */
#define POLARSSL_ERR_X509_FEATURE_UNAVAILABLE              -0x2080  /**< Unavailable feature, e.g. RSA hashing/encryption combination. */
#define POLARSSL_ERR_X509_UNKNOWN_OID                      -0x2100  /**< Requested OID is unknown. */
#define POLARSSL_ERR_X509_INVALID_FORMAT                   -0x2180  /**< The CRT/CRL/CSR format is invalid, e.g. different type expected. */
#define POLARSSL_ERR_X509_INVALID_VERSION                  -0x2200  /**< The CRT/CRL/CSR version element is invalid. */
#define POLARSSL_ERR_X509_INVALID_SERIAL                   -0x2280  /**< The serial tag or value is invalid. */
#define POLARSSL_ERR_X509_INVALID_ALG                      -0x2300  /**< The algorithm tag or value is invalid. */
#define POLARSSL_ERR_X509_INVALID_NAME                     -0x2380  /**< The name tag or value is invalid. */
#define POLARSSL_ERR_X509_INVALID_DATE                     -0x2400  /**< The date tag or value is invalid. */
#define POLARSSL_ERR_X509_INVALID_SIGNATURE                -0x2480  /**< The signature tag or value invalid. */
#define POLARSSL_ERR_X509_INVALID_EXTENSIONS               -0x2500  /**< The extension tag or value is invalid. */
#define POLARSSL_ERR_X509_UNKNOWN_VERSION                  -0x2580  /**< CRT/CRL/CSR has an unsupported version number. */
#define POLARSSL_ERR_X509_UNKNOWN_SIG_ALG                  -0x2600  /**< Signature algorithm (oid) is unsupported. */
#define POLARSSL_ERR_X509_SIG_MISMATCH                     -0x2680  /**< Signature algorithms do not match. (see \c ::x509_crt sig_oid) */
#define POLARSSL_ERR_X509_CERT_VERIFY_FAILED               -0x2700  /**< Certificate verification failed, e.g. CRL, CA or signature check failed. */
#define POLARSSL_ERR_X509_CERT_UNKNOWN_FORMAT              -0x2780  /**< Format not recognized as DER or PEM. */
#define POLARSSL_ERR_X509_BAD_INPUT_DATA                   -0x2800  /**< Input invalid. */
#define POLARSSL_ERR_X509_MALLOC_FAILED                    -0x2880  /**< Allocation of memory failed. */
#define POLARSSL_ERR_X509_FILE_IO_ERROR                    -0x2900  /**< Read/write of file failed. */
/* \} name */

/**
 * \name X509 Verify codes
 * \{
 */
#define BADCERT_EXPIRED             0x01  /**< The certificate validity has expired. */
#define BADCERT_REVOKED             0x02  /**< The certificate has been revoked (is on a CRL). */
#define BADCERT_CN_MISMATCH         0x04  /**< The certificate Common Name (CN) does not match with the expected CN. */
#define BADCERT_NOT_TRUSTED         0x08  /**< The certificate is not correctly signed by the trusted CA. */
#define BADCRL_NOT_TRUSTED          0x10  /**< CRL is not correctly signed by the trusted CA. */
#define BADCRL_EXPIRED              0x20  /**< CRL is expired. */
#define BADCERT_MISSING             0x40  /**< Certificate was missing. */
#define BADCERT_SKIP_VERIFY         0x80  /**< Certificate verification was skipped. */
#define BADCERT_OTHER             0x0100  /**< Other reason (can be used by verify callback) */
#define BADCERT_FUTURE            0x0200  /**< The certificate validity starts in the future. */
#define BADCRL_FUTURE             0x0400  /**< The CRL is from the future */
/* \} name */
/* \} addtogroup x509_module */

/*
 * X.509 v3 Key Usage Extension flags
 */
#define KU_DIGITAL_SIGNATURE            (0x80)  /* bit 0 */
#define KU_NON_REPUDIATION              (0x40)  /* bit 1 */
#define KU_KEY_ENCIPHERMENT             (0x20)  /* bit 2 */
#define KU_DATA_ENCIPHERMENT            (0x10)  /* bit 3 */
#define KU_KEY_AGREEMENT                (0x08)  /* bit 4 */
#define KU_KEY_CERT_SIGN                (0x04)  /* bit 5 */
#define KU_CRL_SIGN                     (0x02)  /* bit 6 */

/*
 * Netscape certificate types
 * (http://www.mozilla.org/projects/security/pki/nss/tech-notes/tn3.html)
 */

#define NS_CERT_TYPE_SSL_CLIENT         (0x80)  /* bit 0 */
#define NS_CERT_TYPE_SSL_SERVER         (0x40)  /* bit 1 */
#define NS_CERT_TYPE_EMAIL              (0x20)  /* bit 2 */
#define NS_CERT_TYPE_OBJECT_SIGNING     (0x10)  /* bit 3 */
#define NS_CERT_TYPE_RESERVED           (0x08)  /* bit 4 */
#define NS_CERT_TYPE_SSL_CA             (0x04)  /* bit 5 */
#define NS_CERT_TYPE_EMAIL_CA           (0x02)  /* bit 6 */
#define NS_CERT_TYPE_OBJECT_SIGNING_CA  (0x01)  /* bit 7 */

/*
 * X.509 extension types
 *
 * Comments refer to the status for using certificates. Status can be
 * different for writing certificates or reading CRLs or CSRs.
 */
#define EXT_AUTHORITY_KEY_IDENTIFIER    (1 << 0)
#define EXT_SUBJECT_KEY_IDENTIFIER      (1 << 1)
#define EXT_KEY_USAGE                   (1 << 2)    /* Parsed but not used */
#define EXT_CERTIFICATE_POLICIES        (1 << 3)
#define EXT_POLICY_MAPPINGS             (1 << 4)
#define EXT_SUBJECT_ALT_NAME            (1 << 5)    /* Supported (DNS) */
#define EXT_ISSUER_ALT_NAME             (1 << 6)
#define EXT_SUBJECT_DIRECTORY_ATTRS     (1 << 7)
#define EXT_BASIC_CONSTRAINTS           (1 << 8)    /* Supported */
#define EXT_NAME_CONSTRAINTS            (1 << 9)
#define EXT_POLICY_CONSTRAINTS          (1 << 10)
#define EXT_EXTENDED_KEY_USAGE          (1 << 11)   /* Parsed but not used */
#define EXT_CRL_DISTRIBUTION_POINTS     (1 << 12)
#define EXT_INIHIBIT_ANYPOLICY          (1 << 13)
#define EXT_FRESHEST_CRL                (1 << 14)

#define EXT_NS_CERT_TYPE                (1 << 16)   /* Parsed (and then ?) */

/*
 * Storage format identifiers
 * Recognized formats: PEM and DER
 */
#define X509_FORMAT_DER                 1
#define X509_FORMAT_PEM                 2

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup x509_module
 * \{ */

/**
 * \name Structures for parsing X.509 certificates, CRLs and CSRs
 * \{
 */

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef asn1_buf x509_buf;

/**
 * Container for ASN1 bit strings.
 */
typedef asn1_bitstring x509_bitstring;

/**
 * Container for ASN1 named information objects.
 * It allows for Relative Distinguished Names (e.g. cn=polarssl,ou=code,etc.).
 */
typedef asn1_named_data x509_name;

/**
 * Container for a sequence of ASN.1 items
 */
typedef asn1_sequence x509_sequence;

/** Container for date and time (precision in seconds). */
typedef struct _x509_time
{
    int year, mon, day;         /**< Date. */
    int hour, min, sec;         /**< Time. */
}
x509_time;

/** \} name Structures for parsing X.509 certificates, CRLs and CSRs */
/** \} addtogroup x509_module */

/**
 * \brief          Store the certificate DN in printable form into buf;
 *                 no more than size characters will be written.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param dn       The X509 name to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509_dn_gets( char *buf, size_t size, const x509_name *dn );

/**
 * \brief          Store the certificate serial in printable form into buf;
 *                 no more than size characters will be written.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param serial   The X509 serial to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509_serial_gets( char *buf, size_t size, const x509_buf *serial );

/**
 * \brief          Give an known OID, return its descriptive string.
 *                 (Deprecated. Use oid_get_extended_key_usage() instead.)
 *                 Warning: only works for extended_key_usage OIDs!
 *
 * \param oid      buffer containing the oid
 *
 * \return         Return a string if the OID is known,
 *                 or NULL otherwise.
 */
const char *x509_oid_get_description( x509_buf *oid );

/**
 * \brief          Give an OID, return a string version of its OID number.
 *                 (Deprecated. Use oid_get_numeric_string() instead)
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param oid      Buffer containing the OID
 *
 * \return         Length of the string written (excluding final NULL) or
 *                 POLARSSL_ERR_OID_BUF_TO_SMALL in case of error
 */
int x509_oid_get_numeric_string( char *buf, size_t size, x509_buf *oid );

/**
 * \brief          Check a given x509_time against the system time and check
 *                 if it is not expired.
 *
 * \param time     x509_time to check
 *
 * \return         0 if the x509_time is still valid,
 *                 1 otherwise.
 */
int x509_time_expired( const x509_time *time );

/**
 * \brief          Check a given x509_time against the system time and check
 *                 if it is not from the future.
 *
 * \param time     x509_time to check
 *
 * \return         0 if the x509_time is already valid,
 *                 1 otherwise.
 */
int x509_time_future( const x509_time *time );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int x509_self_test( int verbose );

/*
 * Internal module functions. You probably do not want to use these unless you
 * know you do.
 */
int x509_get_name( unsigned char **p, const unsigned char *end,
                   x509_name *cur );
int x509_get_alg_null( unsigned char **p, const unsigned char *end,
                       x509_buf *alg );
int x509_get_alg( unsigned char **p, const unsigned char *end,
                  x509_buf *alg, x509_buf *params );
#if defined(POLARSSL_X509_RSASSA_PSS_SUPPORT)
int x509_get_rsassa_pss_params( const x509_buf *params,
                                md_type_t *md_alg, md_type_t *mgf_md,
                                int *salt_len );
#endif
int x509_get_sig( unsigned char **p, const unsigned char *end, x509_buf *sig );
int x509_get_sig_alg( const x509_buf *sig_oid, const x509_buf *sig_params,
                      md_type_t *md_alg, pk_type_t *pk_alg,
                      void **sig_opts );
int x509_get_time( unsigned char **p, const unsigned char *end,
                   x509_time *time );
int x509_get_serial( unsigned char **p, const unsigned char *end,
                     x509_buf *serial );
int x509_get_ext( unsigned char **p, const unsigned char *end,
                  x509_buf *ext, int tag );
int x509_load_file( const char *path, unsigned char **buf, size_t *n );
int x509_sig_alg_gets( char *buf, size_t size, const x509_buf *sig_oid,
                       pk_type_t pk_alg, md_type_t md_alg,
                       const void *sig_opts );
int x509_key_size_helper( char *buf, size_t size, const char *name );
int x509_string_to_names( asn1_named_data **head, const char *name );
int x509_set_extension( asn1_named_data **head, const char *oid, size_t oid_len,
                        int critical, const unsigned char *val,
                        size_t val_len );
int x509_write_extensions( unsigned char **p, unsigned char *start,
                           asn1_named_data *first );
int x509_write_names( unsigned char **p, unsigned char *start,
                      asn1_named_data *first );
int x509_write_sig( unsigned char **p, unsigned char *start,
                    const char *oid, size_t oid_len,
                    unsigned char *sig, size_t size );

#ifdef __cplusplus
}
#endif

#endif /* x509.h */
