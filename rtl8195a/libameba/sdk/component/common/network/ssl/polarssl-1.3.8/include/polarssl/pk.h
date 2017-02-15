/**
 * \file pk.h
 *
 * \brief Public Key abstraction layer
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

#ifndef POLARSSL_PK_H
#define POLARSSL_PK_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include "md.h"

#if defined(POLARSSL_RSA_C)
#include "rsa.h"
#endif

#if defined(POLARSSL_ECP_C)
#include "ecp.h"
#endif

#if defined(POLARSSL_ECDSA_C)
#include "ecdsa.h"
#endif

#define POLARSSL_ERR_PK_MALLOC_FAILED       -0x2F80  /**< Memory alloation failed. */
#define POLARSSL_ERR_PK_TYPE_MISMATCH       -0x2F00  /**< Type mismatch, eg attempt to encrypt with an ECDSA key */
#define POLARSSL_ERR_PK_BAD_INPUT_DATA      -0x2E80  /**< Bad input parameters to function. */
#define POLARSSL_ERR_PK_FILE_IO_ERROR       -0x2E00  /**< Read/write of file failed. */
#define POLARSSL_ERR_PK_KEY_INVALID_VERSION -0x2D80  /**< Unsupported key version */
#define POLARSSL_ERR_PK_KEY_INVALID_FORMAT  -0x2D00  /**< Invalid key tag or value. */
#define POLARSSL_ERR_PK_UNKNOWN_PK_ALG      -0x2C80  /**< Key algorithm is unsupported (only RSA and EC are supported). */
#define POLARSSL_ERR_PK_PASSWORD_REQUIRED   -0x2C00  /**< Private key password can't be empty. */
#define POLARSSL_ERR_PK_PASSWORD_MISMATCH   -0x2B80  /**< Given private key password does not allow for correct decryption. */
#define POLARSSL_ERR_PK_INVALID_PUBKEY      -0x2B00  /**< The pubkey tag or value is invalid (only RSA and EC are supported). */
#define POLARSSL_ERR_PK_INVALID_ALG         -0x2A80  /**< The algorithm tag or value is invalid. */
#define POLARSSL_ERR_PK_UNKNOWN_NAMED_CURVE -0x2A00  /**< Elliptic curve is unsupported (only NIST curves are supported). */
#define POLARSSL_ERR_PK_FEATURE_UNAVAILABLE -0x2980  /**< Unavailable feature, e.g. RSA disabled for RSA key. */
#define POLARSSL_ERR_PK_SIG_LEN_MISMATCH    -0x2000  /**< The signature is valid but its length is less than expected. */


#if defined(POLARSSL_RSA_C)
/**
 * Quick access to an RSA context inside a PK context.
 *
 * \warning You must make sure the PK context actually holds an RSA context
 * before using this macro!
 */
#define pk_rsa( pk )        ( (rsa_context *) (pk).pk_ctx )
#endif /* POLARSSL_RSA_C */

#if defined(POLARSSL_ECP_C)
/**
 * Quick access to an EC context inside a PK context.
 *
 * \warning You must make sure the PK context actually holds an EC context
 * before using this macro!
 */
#define pk_ec( pk )         ( (ecp_keypair *) (pk).pk_ctx )
#endif /* POLARSSL_ECP_C */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Public key types
 */
typedef enum {
    POLARSSL_PK_NONE=0,
    POLARSSL_PK_RSA,
    POLARSSL_PK_ECKEY,
    POLARSSL_PK_ECKEY_DH,
    POLARSSL_PK_ECDSA,
    POLARSSL_PK_RSA_ALT,
    POLARSSL_PK_RSASSA_PSS,
} pk_type_t;

/**
 * \brief           Options for RSASSA-PSS signature verification.
 *                  See \c rsa_rsassa_pss_verify_ext()
 */
typedef struct
{
    md_type_t mgf1_hash_id;
    int expected_salt_len;

} pk_rsassa_pss_options;

/**
 * \brief           Types for interfacing with the debug module
 */
typedef enum
{
    POLARSSL_PK_DEBUG_NONE = 0,
    POLARSSL_PK_DEBUG_MPI,
    POLARSSL_PK_DEBUG_ECP,
} pk_debug_type;

/**
 * \brief           Item to send to the debug module
 */
typedef struct
{
    pk_debug_type type;
    const char *name;
    void *value;
} pk_debug_item;

/** Maximum number of item send for debugging, plus 1 */
#define POLARSSL_PK_DEBUG_MAX_ITEMS 3

/**
 * \brief           Public key information and operations
 */
typedef struct
{
    /** Public key type */
    pk_type_t type;

    /** Type name */
    const char *name;

    /** Get key size in bits */
    size_t (*get_size)( const void * );

    /** Tell if the context implements this type (e.g. ECKEY can do ECDSA) */
    int (*can_do)( pk_type_t type );

    /** Verify signature */
    int (*verify_func)( void *ctx, md_type_t md_alg,
                        const unsigned char *hash, size_t hash_len,
                        const unsigned char *sig, size_t sig_len );

    /** Make signature */
    int (*sign_func)( void *ctx, md_type_t md_alg,
                      const unsigned char *hash, size_t hash_len,
                      unsigned char *sig, size_t *sig_len,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng );

    /** Decrypt message */
    int (*decrypt_func)( void *ctx, const unsigned char *input, size_t ilen,
                         unsigned char *output, size_t *olen, size_t osize,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng );

    /** Encrypt message */
    int (*encrypt_func)( void *ctx, const unsigned char *input, size_t ilen,
                         unsigned char *output, size_t *olen, size_t osize,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng );

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

    /** Interface with the debug module */
    void (*debug_func)( const void *ctx, pk_debug_item *items );

} pk_info_t;

/**
 * \brief           Public key container
 */
typedef struct
{
    const pk_info_t *   pk_info;    /**< Public key informations        */
    void *              pk_ctx;     /**< Underlying public key context  */
} pk_context;

/**
 * \brief           Types for RSA-alt abstraction
 */
typedef int (*pk_rsa_alt_decrypt_func)( void *ctx, int mode, size_t *olen,
                    const unsigned char *input, unsigned char *output,
                    size_t output_max_len );
typedef int (*pk_rsa_alt_sign_func)( void *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                    int mode, md_type_t md_alg, unsigned int hashlen,
                    const unsigned char *hash, unsigned char *sig );
typedef size_t (*pk_rsa_alt_key_len_func)( void *ctx );

/**
 * \brief           Return information associated with the given PK type
 *
 * \param pk_type   PK type to search for.
 *
 * \return          The PK info associated with the type or NULL if not found.
 */
const pk_info_t *pk_info_from_type( pk_type_t pk_type );

/**
 * \brief           Initialize a pk_context (as NONE)
 */
void pk_init( pk_context *ctx );

/**
 * \brief           Free a pk_context
 */
void pk_free( pk_context *ctx );

/**
 * \brief           Initialize a PK context with the information given
 *                  and allocates the type-specific PK subcontext.
 *
 * \param ctx       Context to initialize. Must be empty (type NONE).
 * \param info      Information to use
 *
 * \return          0 on success,
 *                  POLARSSL_ERR_PK_BAD_INPUT_DATA on invalid input,
 *                  POLARSSL_ERR_PK_MALLOC_FAILED on allocation failure.
 *
 * \note            For contexts holding an RSA-alt key, use
 *                  \c pk_init_ctx_rsa_alt() instead.
 */
int pk_init_ctx( pk_context *ctx, const pk_info_t *info );

/**
 * \brief           Initialize an RSA-alt context
 *
 * \param ctx       Context to initialize. Must be empty (type NONE).
 * \param key       RSA key pointer
 * \param decrypt_func  Decryption function
 * \param sign_func     Signing function
 * \param key_len_func  Function returning key length in bytes
 *
 * \return          0 on success, or POLARSSL_ERR_PK_BAD_INPUT_DATA if the
 *                  context wasn't already initialized as RSA_ALT.
 *
 * \note            This function replaces \c pk_init_ctx() for RSA-alt.
 */
int pk_init_ctx_rsa_alt( pk_context *ctx, void * key,
                         pk_rsa_alt_decrypt_func decrypt_func,
                         pk_rsa_alt_sign_func sign_func,
                         pk_rsa_alt_key_len_func key_len_func );

/**
 * \brief           Get the size in bits of the underlying key
 *
 * \param ctx       Context to use
 *
 * \return          Key size in bits, or 0 on error
 */
size_t pk_get_size( const pk_context *ctx );

/**
 * \brief           Get the length in bytes of the underlying key
 * \param ctx       Context to use
 *
 * \return          Key length in bytes, or 0 on error
 */
static inline size_t pk_get_len( const pk_context *ctx )
{
    return( ( pk_get_size( ctx ) + 7 ) / 8 );
}

/**
 * \brief           Tell if a context can do the operation given by type
 *
 * \param ctx       Context to test
 * \param type      Target type
 *
 * \return          0 if context can't do the operations,
 *                  1 otherwise.
 */
int pk_can_do( pk_context *ctx, pk_type_t type );

/**
 * \brief           Verify signature (including padding if relevant).
 *
 * \param ctx       PK context to use
 * \param md_alg    Hash algorithm used (see notes)
 * \param hash      Hash of the message to sign
 * \param hash_len  Hash length or 0 (see notes)
 * \param sig       Signature to verify
 * \param sig_len   Signature length
 *
 * \return          0 on success (signature is valid),
 *                  POLARSSL_ERR_PK_SIG_LEN_MISMATCH if the signature is
 *                  valid but its actual length is less than sig_len,
 *                  or a specific error code.
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *                  Use \c pk_verify_ext( POLARSSL_PK_RSASSA_PSS, ... )
 *                  to verify RSASSA_PSS signatures.
 *
 * \note            If hash_len is 0, then the length associated with md_alg
 *                  is used instead, or an error returned if it is invalid.
 *
 * \note            md_alg may be POLARSSL_MD_NONE, only if hash_len != 0
 */
int pk_verify( pk_context *ctx, md_type_t md_alg,
               const unsigned char *hash, size_t hash_len,
               const unsigned char *sig, size_t sig_len );

/**
 * \brief           Verify signature, with options.
 *                  (Includes verification of the padding depending on type.)
 *
 * \param type      Signature type (inc. possible padding type) to verify
 * \param options   Pointer to type-specific options, or NULL
 * \param ctx       PK context to use
 * \param md_alg    Hash algorithm used (see notes)
 * \param hash      Hash of the message to sign
 * \param hash_len  Hash length or 0 (see notes)
 * \param sig       Signature to verify
 * \param sig_len   Signature length
 *
 * \return          0 on success (signature is valid),
 *                  POLARSSL_ERR_PK_TYPE_MISMATCH if the PK context can't be
 *                  used for this type of signatures,
 *                  POLARSSL_ERR_PK_SIG_LEN_MISMATCH if the signature is
 *                  valid but its actual length is less than sig_len,
 *                  or a specific error code.
 *
 * \note            If hash_len is 0, then the length associated with md_alg
 *                  is used instead, or an error returned if it is invalid.
 *
 * \note            md_alg may be POLARSSL_MD_NONE, only if hash_len != 0
 *
 * \note            If type is POLARSSL_PK_RSASSA_PSS, then options must point
 *                  to a pk_rsassa_pss_options structure,
 *                  otherwise it must be NULL.
 */
int pk_verify_ext( pk_type_t type, const void *options,
                   pk_context *ctx, md_type_t md_alg,
                   const unsigned char *hash, size_t hash_len,
                   const unsigned char *sig, size_t sig_len );

/**
 * \brief           Make signature, including padding if relevant.
 *
 * \param ctx       PK context to use
 * \param md_alg    Hash algorithm used (see notes)
 * \param hash      Hash of the message to sign
 * \param hash_len  Hash length or 0 (see notes)
 * \param sig       Place to write the signature
 * \param sig_len   Number of bytes written
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 on success, or a specific error code.
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *                  There is no interface in the PK module to make RSASSA-PSS
 *                  signatures yet.
 *
 * \note            If hash_len is 0, then the length associated with md_alg
 *                  is used instead, or an error returned if it is invalid.
 *
 * \note            md_alg may be POLARSSL_MD_NONE, only if hash_len != 0
 */
int pk_sign( pk_context *ctx, md_type_t md_alg,
             const unsigned char *hash, size_t hash_len,
             unsigned char *sig, size_t *sig_len,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Decrypt message (including padding if relevant).
 *
 * \param ctx       PK context to use
 * \param input     Input to decrypt
 * \param ilen      Input size
 * \param output    Decrypted output
 * \param olen      Decrypted message length
 * \param osize     Size of the output buffer
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *
 * \return          0 on success, or a specific error code.
 */
int pk_decrypt( pk_context *ctx,
                const unsigned char *input, size_t ilen,
                unsigned char *output, size_t *olen, size_t osize,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Encrypt message (including padding if relevant).
 *
 * \param ctx       PK context to use
 * \param input     Message to encrypt
 * \param ilen      Message size
 * \param output    Encrypted output
 * \param olen      Encrypted output length
 * \param osize     Size of the output buffer
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *
 * \return          0 on success, or a specific error code.
 */
int pk_encrypt( pk_context *ctx,
                const unsigned char *input, size_t ilen,
                unsigned char *output, size_t *olen, size_t osize,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Export debug information
 *
 * \param ctx       Context to use
 * \param items     Place to write debug items
 *
 * \return          0 on success or POLARSSL_ERR_PK_BAD_INPUT_DATA
 */
int pk_debug( const pk_context *ctx, pk_debug_item *items );

/**
 * \brief           Access the type name
 *
 * \param ctx       Context to use
 *
 * \return          Type name on success, or "invalid PK"
 */
const char * pk_get_name( const pk_context *ctx );

/**
 * \brief           Get the key type
 *
 * \param ctx       Context to use
 *
 * \return          Type on success, or POLARSSL_PK_NONE
 */
pk_type_t pk_get_type( const pk_context *ctx );

#if defined(POLARSSL_PK_PARSE_C)
/** \ingroup pk_module */
/**
 * \brief           Parse a private key
 *
 * \param ctx       key to be initialized
 * \param key       input buffer
 * \param keylen    size of the buffer
 * \param pwd       password for decryption (optional)
 * \param pwdlen    size of the password
 *
 * \note            On entry, ctx must be empty, either freshly initialised
 *                  with pk_init() or reset with pk_free(). If you need a
 *                  specific key type, check the result with pk_can_do().
 *
 * \note            The key is also checked for correctness.
 *
 * \return          0 if successful, or a specific PK or PEM error code
 */
int pk_parse_key( pk_context *ctx,
                  const unsigned char *key, size_t keylen,
                  const unsigned char *pwd, size_t pwdlen );

/** \ingroup pk_module */
/**
 * \brief           Parse a public key
 *
 * \param ctx       key to be initialized
 * \param key       input buffer
 * \param keylen    size of the buffer
 *
 * \note            On entry, ctx must be empty, either freshly initialised
 *                  with pk_init() or reset with pk_free(). If you need a
 *                  specific key type, check the result with pk_can_do().
 *
 * \note            The key is also checked for correctness.
 *
 * \return          0 if successful, or a specific PK or PEM error code
 */
int pk_parse_public_key( pk_context *ctx,
                         const unsigned char *key, size_t keylen );

#if defined(POLARSSL_FS_IO)
/** \ingroup pk_module */
/**
 * \brief           Load and parse a private key
 *
 * \param ctx       key to be initialized
 * \param path      filename to read the private key from
 * \param password  password to decrypt the file (can be NULL)
 *
 * \note            On entry, ctx must be empty, either freshly initialised
 *                  with pk_init() or reset with pk_free(). If you need a
 *                  specific key type, check the result with pk_can_do().
 *
 * \note            The key is also checked for correctness.
 *
 * \return          0 if successful, or a specific PK or PEM error code
 */
int pk_parse_keyfile( pk_context *ctx,
                      const char *path, const char *password );

/** \ingroup pk_module */
/**
 * \brief           Load and parse a public key
 *
 * \param ctx       key to be initialized
 * \param path      filename to read the private key from
 *
 * \note            On entry, ctx must be empty, either freshly initialised
 *                  with pk_init() or reset with pk_free(). If you need a
 *                  specific key type, check the result with pk_can_do().
 *
 * \note            The key is also checked for correctness.
 *
 * \return          0 if successful, or a specific PK or PEM error code
 */
int pk_parse_public_keyfile( pk_context *ctx, const char *path );
#endif /* POLARSSL_FS_IO */
#endif /* POLARSSL_PK_PARSE_C */

#if defined(POLARSSL_PK_WRITE_C)
/**
 * \brief           Write a private key to a PKCS#1 or SEC1 DER structure
 *                  Note: data is written at the end of the buffer! Use the
 *                        return value to determine where you should start
 *                        using the buffer
 *
 * \param ctx       private to write away
 * \param buf       buffer to write to
 * \param size      size of the buffer
 *
 * \return          length of data written if successful, or a specific
 *                  error code
 */
int pk_write_key_der( pk_context *ctx, unsigned char *buf, size_t size );

/**
 * \brief           Write a public key to a SubjectPublicKeyInfo DER structure
 *                  Note: data is written at the end of the buffer! Use the
 *                        return value to determine where you should start
 *                        using the buffer
 *
 * \param ctx       public key to write away
 * \param buf       buffer to write to
 * \param size      size of the buffer
 *
 * \return          length of data written if successful, or a specific
 *                  error code
 */
int pk_write_pubkey_der( pk_context *ctx, unsigned char *buf, size_t size );

#if defined(POLARSSL_PEM_WRITE_C)
/**
 * \brief           Write a public key to a PEM string
 *
 * \param ctx       public key to write away
 * \param buf       buffer to write to
 * \param size      size of the buffer
 *
 * \return          0 successful, or a specific error code
 */
int pk_write_pubkey_pem( pk_context *ctx, unsigned char *buf, size_t size );

/**
 * \brief           Write a private key to a PKCS#1 or SEC1 PEM string
 *
 * \param ctx       private to write away
 * \param buf       buffer to write to
 * \param size      size of the buffer
 *
 * \return          0 successful, or a specific error code
 */
int pk_write_key_pem( pk_context *ctx, unsigned char *buf, size_t size );
#endif /* POLARSSL_PEM_WRITE_C */
#endif /* POLARSSL_PK_WRITE_C */

/*
 * WARNING: Low-level functions. You probably do not want to use these unless
 *          you are certain you do ;)
 */

#if defined(POLARSSL_PK_PARSE_C)
/**
 * \brief           Parse a SubjectPublicKeyInfo DER structure
 *
 * \param p         the position in the ASN.1 data
 * \param end       end of the buffer
 * \param pk        the key to fill
 *
 * \return          0 if successful, or a specific PK error code
 */
int pk_parse_subpubkey( unsigned char **p, const unsigned char *end,
                        pk_context *pk );
#endif /* POLARSSL_PK_PARSE_C */

#if defined(POLARSSL_PK_WRITE_C)
/**
 * \brief           Write a subjectPublicKey to ASN.1 data
 *                  Note: function works backwards in data buffer
 *
 * \param p         reference to current position pointer
 * \param start     start of the buffer (for bounds-checking)
 * \param key       public key to write away
 *
 * \return          the length written or a negative error code
 */
int pk_write_pubkey( unsigned char **p, unsigned char *start,
                     const pk_context *key );
#endif /* POLARSSL_PK_WRITE_C */

#ifdef __cplusplus
}
#endif

#endif /* POLARSSL_PK_H */
