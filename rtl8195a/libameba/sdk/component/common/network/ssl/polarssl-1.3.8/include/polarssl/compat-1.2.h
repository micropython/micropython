/**
 * \file compat-1.2.h
 *
 * \brief Backwards compatibility header for PolarSSL-1.2 from PolarSSL-1.3
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
#ifndef POLARSSL_COMPAT_1_2_H
#define POLARSSL_COMPAT_1_2_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

// Comment out to disable prototype change warnings
#define SHOW_PROTOTYPE_CHANGE_WARNINGS

#if defined(_MSC_VER) && !defined(inline)
#define inline _inline
#else
#if defined(__ARMCC_VERSION) && !defined(inline)
#define inline __inline
#endif /* __ARMCC_VERSION */
#endif /* _MSC_VER */

#if defined(_MSC_VER)
// MSVC does not support #warning
#undef SHOW_PROTOTYPE_CHANGE_WARNINGS
#endif

#if defined(SHOW_PROTOTYPE_CHANGE_WARNINGS)
#warning "You can disable these warnings by commenting SHOW_PROTOTYPE_CHANGE_WARNINGS in compat-1.2.h"
#endif

#if defined(POLARSSL_SHA256_C)
#define POLARSSL_SHA2_C
#include "sha256.h"

/*
 * SHA-2 -> SHA-256
 */
typedef sha256_context sha2_context;

static inline void sha2_starts( sha256_context *ctx, int is224 ) {
    sha256_starts( ctx, is224 );
}
static inline void sha2_update( sha256_context *ctx, const unsigned char *input,
                         size_t ilen ) {
    sha256_update( ctx, input, ilen );
}
static inline void sha2_finish( sha256_context *ctx, unsigned char output[32] ) {
    sha256_finish( ctx, output );
}
static inline int sha2_file( const char *path, unsigned char output[32], int is224 ) {
    return sha256_file( path, output, is224 );
}
static inline void sha2( const unsigned char *input, size_t ilen,
                  unsigned char output[32], int is224 ) {
    sha256( input, ilen, output, is224 );
}
static inline void sha2_hmac_starts( sha256_context *ctx, const unsigned char *key,
                              size_t keylen, int is224 ) {
    sha256_hmac_starts( ctx, key, keylen, is224 );
}
static inline void sha2_hmac_update( sha256_context *ctx, const unsigned char *input, size_t ilen ) {
    sha256_hmac_update( ctx, input, ilen );
}
static inline void sha2_hmac_finish( sha256_context *ctx, unsigned char output[32] ) {
    sha256_hmac_finish( ctx, output );
}
static inline void sha2_hmac_reset( sha256_context *ctx ) {
    sha256_hmac_reset( ctx );
}
static inline void sha2_hmac( const unsigned char *key, size_t keylen,
                       const unsigned char *input, size_t ilen,
                       unsigned char output[32], int is224 ) {
    sha256_hmac( key, keylen, input, ilen, output, is224 );
}
static inline int sha2_self_test( int verbose ) {
    return sha256_self_test( verbose );
}
#endif /* POLARSSL_SHA256_C */

#if defined(POLARSSL_SHA512_C)
#define POLARSSL_SHA4_C
#include "sha512.h"

/*
 * SHA-4 -> SHA-512
 */
typedef sha512_context sha4_context;

static inline void sha4_starts( sha512_context *ctx, int is384 ) {
    sha512_starts( ctx, is384 );
}
static inline void sha4_update( sha512_context *ctx, const unsigned char *input,
                         size_t ilen ) {
    sha512_update( ctx, input, ilen );
}
static inline void sha4_finish( sha512_context *ctx, unsigned char output[64] ) {
    sha512_finish( ctx, output );
}
static inline int sha4_file( const char *path, unsigned char output[64], int is384 ) {
    return sha512_file( path, output, is384 );
}
static inline void sha4( const unsigned char *input, size_t ilen,
                  unsigned char output[32], int is384 ) {
    sha512( input, ilen, output, is384 );
}
static inline void sha4_hmac_starts( sha512_context *ctx, const unsigned char *key,
                              size_t keylen, int is384 ) {
    sha512_hmac_starts( ctx, key, keylen, is384 );
}
static inline void sha4_hmac_update( sha512_context *ctx, const unsigned char *input, size_t ilen ) {
    sha512_hmac_update( ctx, input, ilen );
}
static inline void sha4_hmac_finish( sha512_context *ctx, unsigned char output[64] ) {
    sha512_hmac_finish( ctx, output );
}
static inline void sha4_hmac_reset( sha512_context *ctx ) {
    sha512_hmac_reset( ctx );
}
static inline void sha4_hmac( const unsigned char *key, size_t keylen,
                       const unsigned char *input, size_t ilen,
                       unsigned char output[64], int is384 ) {
    sha512_hmac( key, keylen, input, ilen, output, is384 );
}
static inline int sha4_self_test( int verbose ) {
    return sha512_self_test( verbose );
}
#endif /* POLARSSL_SHA512_C */

#if defined(POLARSSL_CIPHER_C)
#if defined(SHOW_PROTOTYPE_CHANGE_WARNINGS)
#warning "cipher_reset() prototype changed. Manual change required if used"
#endif
#endif

#if defined(POLARSSL_RSA_C)
#define SIG_RSA_RAW     POLARSSL_MD_NONE
#define SIG_RSA_MD2     POLARSSL_MD_MD2
#define SIG_RSA_MD4     POLARSSL_MD_MD4
#define SIG_RSA_MD5     POLARSSL_MD_MD5
#define SIG_RSA_SHA1    POLARSSL_MD_SHA1
#define SIG_RSA_SHA224  POLARSSL_MD_SHA224
#define SIG_RSA_SHA256  POLARSSL_MD_SHA256
#define SIG_RSA_SHA384  POLARSSL_MD_SHA384
#define SIG_RSA_SHA512  POLARSSL_MD_SHA512
#if defined(SHOW_PROTOTYPE_CHANGE_WARNINGS)
#warning "rsa_pkcs1_verify() prototype changed. Manual change required if used"
#warning "rsa_pkcs1_decrypt() prototype changed. Manual change required if used"
#endif
#endif /* POLARSSL_RSA_C */

#if defined(POLARSSL_DHM_C)
#if defined(SHOW_PROTOTYPE_CHANGE_WARNINGS)
#warning "dhm_calc_secret() prototype changed. Manual change required if used"
#endif
#endif

#if defined(POLARSSL_GCM_C)
#if defined(SHOW_PROTOTYPE_CHANGE_WARNINGS)
#warning "gcm_init() prototype changed. Manual change required if used"
#endif
#endif

#if defined(POLARSSL_SSL_CLI_C)
#if defined(SHOW_PROTOTYPE_CHANGE_WARNINGS)
#warning "ssl_set_own_cert() prototype changed. Change to ssl_set_own_cert_rsa(). Manual change required if used"
#endif
#endif

#if defined(POLARSSL_X509_USE_C) || defined(POLARSSL_X509_CREATE_C)
#include "x509.h"

#define POLARSSL_ERR_X509_CERT_INVALID_FORMAT POLARSSL_ERR_X509_INVALID_FORMAT
#define POLARSSL_ERR_X509_CERT_INVALID_VERSION POLARSSL_ERR_X509_INVALID_VERSION
#define POLARSSL_ERR_X509_CERT_INVALID_ALG POLARSSL_ERR_X509_INVALID_ALG
#define POLARSSL_ERR_X509_CERT_UNKNOWN_SIG_ALG POLARSSL_ERR_X509_UNKNOWN_SIG_ALG
#define POLARSSL_ERR_X509_CERT_INVALID_NAME POLARSSL_ERR_X509_INVALID_NAME
#define POLARSSL_ERR_X509_CERT_INVALID_DATE POLARSSL_ERR_X509_INVALID_DATE
#define POLARSSL_ERR_X509_CERT_INVALID_EXTENSIONS POLARSSL_ERR_X509_INVALID_EXTENSIONS
#define POLARSSL_ERR_X509_CERT_SIG_MISMATCH POLARSSL_ERR_X509_SIG_MISMATCH
#define POLARSSL_ERR_X509_CERT_INVALID_SIGNATURE POLARSSL_ERR_X509_INVALID_SIGNATURE
#define POLARSSL_ERR_X509_CERT_INVALID_SERIAL POLARSSL_ERR_X509_INVALID_SERIAL
#define POLARSSL_ERR_X509_CERT_UNKNOWN_VERSION POLARSSL_ERR_X509_UNKNOWN_VERSION

static inline int x509parse_serial_gets( char *buf, size_t size, const x509_buf *serial ) {
    return x509_serial_gets( buf, size, serial );
}
static inline int x509parse_dn_gets( char *buf, size_t size, const x509_name *dn ) {
    return x509_dn_gets( buf, size, dn );
}
static inline int x509parse_time_expired( const x509_time *time ) {
    return x509_time_expired( time );
}
#endif /* POLARSSL_X509_USE_C || POLARSSL_X509_CREATE_C */

#if defined(POLARSSL_X509_CRT_PARSE_C)
#define POLARSSL_X509_PARSE_C
#include "x509_crt.h"
typedef x509_crt x509_cert;

static inline int x509parse_crt_der( x509_cert *chain, const unsigned char *buf,
                              size_t buflen ) {
    return x509_crt_parse_der( chain, buf, buflen );
}
static inline int x509parse_crt( x509_cert *chain, const unsigned char *buf, size_t buflen ) {
    return x509_crt_parse( chain, buf, buflen );
}
static inline int x509parse_crtfile( x509_cert *chain, const char *path ) {
    return x509_crt_parse_file( chain, path );
}
static inline int x509parse_crtpath( x509_cert *chain, const char *path ) {
    return x509_crt_parse_path( chain, path );
}
static inline int x509parse_cert_info( char *buf, size_t size, const char *prefix,
                                const x509_cert *crt ) {
    return x509_crt_info( buf, size, prefix, crt );
}
static inline int x509parse_verify( x509_cert *crt, x509_cert *trust_ca,
                             x509_crl *ca_crl, const char *cn, int *flags,
                             int (*f_vrfy)(void *, x509_cert *, int, int *),
                             void *p_vrfy ) {
    return x509_crt_verify( crt, trust_ca, ca_crl, cn, flags, f_vrfy, p_vrfy );
}
static inline int x509parse_revoked( const x509_cert *crt, const x509_crl *crl ) {
    return x509_crt_revoked( crt, crl );
}
static inline void x509_free( x509_cert *crt ) {
    x509_crt_free( crt );
}
#endif /* POLARSSL_X509_CRT_PARSE_C */

#if defined(POLARSSL_X509_CRL_PARSE_C)
#define POLARSSL_X509_PARSE_C
#include "x509_crl.h"
static inline int x509parse_crl( x509_crl *chain, const unsigned char *buf, size_t buflen ) {
    return x509_crl_parse( chain, buf, buflen );
}
static inline int x509parse_crlfile( x509_crl *chain, const char *path ) {
    return x509_crl_parse_file( chain, path );
}
static inline int x509parse_crl_info( char *buf, size_t size, const char *prefix,
                               const x509_crl *crl ) {
    return x509_crl_info( buf, size, prefix, crl );
}
#endif /* POLARSSL_X509_CRL_PARSE_C */

#if defined(POLARSSL_X509_CSR_PARSE_C)
#define POLARSSL_X509_PARSE_C
#include "x509_csr.h"
static inline int x509parse_csr( x509_csr *csr, const unsigned char *buf, size_t buflen ) {
    return x509_csr_parse( csr, buf, buflen );
}
static inline int x509parse_csrfile( x509_csr *csr, const char *path ) {
    return x509_csr_parse_file( csr, path );
}
static inline int x509parse_csr_info( char *buf, size_t size, const char *prefix,
                               const x509_csr *csr ) {
    return x509_csr_info( buf, size, prefix, csr );
}
#endif /* POLARSSL_X509_CSR_PARSE_C */

#if defined(POLARSSL_SSL_TLS_C)
#include "ssl_ciphersuites.h"

#define ssl_default_ciphersuites ssl_list_ciphersuites()
#endif

#if defined(POLARSSL_PK_PARSE_C) && defined(POLARSSL_RSA_C)
#include "rsa.h"
#include "pk.h"

#define POLARSSL_ERR_X509_PASSWORD_MISMATCH POLARSSL_ERR_PK_PASSWORD_MISMATCH
#define POLARSSL_ERR_X509_KEY_INVALID_FORMAT POLARSSL_ERR_PK_KEY_INVALID_FORMAT
#define POLARSSL_ERR_X509_UNKNOWN_PK_ALG POLARSSL_ERR_PK_UNKNOWN_PK_ALG
#define POLARSSL_ERR_X509_CERT_INVALID_PUBKEY POLARSSL_ERR_PK_INVALID_PUBKEY

#if defined(POLARSSL_FS_IO)
static inline int x509parse_keyfile( rsa_context *rsa, const char *path,
                              const char *pwd ) {
    int ret;
    pk_context pk;
    pk_init( &pk );
    ret = pk_parse_keyfile( &pk, path, pwd );
    if( ret == 0 && ! pk_can_do( &pk, POLARSSL_PK_RSA ) )
        ret = POLARSSL_ERR_PK_TYPE_MISMATCH;
    if( ret == 0 )
        rsa_copy( rsa, pk_rsa( pk ) );
    else
        rsa_free( rsa );
    pk_free( &pk );
    return( ret );
}
static inline int x509parse_public_keyfile( rsa_context *rsa, const char *path ) {
    int ret;
    pk_context pk;
    pk_init( &pk );
    ret = pk_parse_public_keyfile( &pk, path );
    if( ret == 0 && ! pk_can_do( &pk, POLARSSL_PK_RSA ) )
        ret = POLARSSL_ERR_PK_TYPE_MISMATCH;
    if( ret == 0 )
        rsa_copy( rsa, pk_rsa( pk ) );
    else
        rsa_free( rsa );
    pk_free( &pk );
    return( ret );
}
#endif /* POLARSSL_FS_IO */

static inline int x509parse_key( rsa_context *rsa, const unsigned char *key,
                          size_t keylen,
                          const unsigned char *pwd, size_t pwdlen ) {
    int ret;
    pk_context pk;
    pk_init( &pk );
    ret = pk_parse_key( &pk, key, keylen, pwd, pwdlen );
    if( ret == 0 && ! pk_can_do( &pk, POLARSSL_PK_RSA ) )
        ret = POLARSSL_ERR_PK_TYPE_MISMATCH;
    if( ret == 0 )
        rsa_copy( rsa, pk_rsa( pk ) );
    else
        rsa_free( rsa );
    pk_free( &pk );
    return( ret );
}

static inline int x509parse_public_key( rsa_context *rsa,
                                 const unsigned char *key, size_t keylen )
{
    int ret;
    pk_context pk;
    pk_init( &pk );
    ret = pk_parse_public_key( &pk, key, keylen );
    if( ret == 0 && ! pk_can_do( &pk, POLARSSL_PK_RSA ) )
        ret = POLARSSL_ERR_PK_TYPE_MISMATCH;
    if( ret == 0 )
        rsa_copy( rsa, pk_rsa( pk ) );
    else
        rsa_free( rsa );
    pk_free( &pk );
    return( ret );
}
#endif /* POLARSSL_PK_PARSE_C && POLARSSL_RSA_C */

#if defined(POLARSSL_PK_WRITE_C) && defined(POLARSSL_RSA_C)
#include "pk.h"
static inline int x509_write_pubkey_der( unsigned char *buf, size_t len, rsa_context *rsa ) {
    int ret;
    pk_context ctx;
    if( ( ret = pk_init_ctx( &ctx, pk_info_from_type( POLARSSL_PK_RSA ) ) ) != 0 ) return( ret );
    if( ( ret = rsa_copy( pk_rsa( ctx ), rsa ) ) != 0 ) return( ret );
    ret = pk_write_pubkey_der( &ctx, buf, len );
    pk_free( &ctx );
    return( ret );
}
static inline int x509_write_key_der( unsigned char *buf, size_t len, rsa_context *rsa ) {
    int ret;
    pk_context ctx;
    if( ( ret = pk_init_ctx( &ctx, pk_info_from_type( POLARSSL_PK_RSA ) ) ) != 0 ) return( ret );
    if( ( ret = rsa_copy( pk_rsa( ctx ), rsa ) ) != 0 ) return( ret );
    ret = pk_write_key_der( &ctx, buf, len );
    pk_free( &ctx );
    return( ret );
}
#endif /* POLARSSL_PK_WRITE_C && POLARSSL_RSA_C */
#endif /* compat-1.2.h */
