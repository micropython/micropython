/**
 * \file cipher.h
 *
 * \brief Generic cipher wrapper.
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

#ifndef POLARSSL_CIPHER_H
#define POLARSSL_CIPHER_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_GCM_C) || defined(POLARSSL_CCM_C)
#define POLARSSL_CIPHER_MODE_AEAD
#endif

#if defined(POLARSSL_CIPHER_MODE_CBC)
#define POLARSSL_CIPHER_MODE_WITH_PADDING
#endif

#include <string.h>

#if defined(_MSC_VER) && !defined(inline)
#define inline _inline
#else
#if defined(__ARMCC_VERSION) && !defined(inline)
#define inline __inline
#endif /* __ARMCC_VERSION */
#endif /*_MSC_VER */

#define POLARSSL_ERR_CIPHER_FEATURE_UNAVAILABLE            -0x6080  /**< The selected feature is not available. */
#define POLARSSL_ERR_CIPHER_BAD_INPUT_DATA                 -0x6100  /**< Bad input parameters to function. */
#define POLARSSL_ERR_CIPHER_ALLOC_FAILED                   -0x6180  /**< Failed to allocate memory. */
#define POLARSSL_ERR_CIPHER_INVALID_PADDING                -0x6200  /**< Input data contains invalid padding and is rejected. */
#define POLARSSL_ERR_CIPHER_FULL_BLOCK_EXPECTED            -0x6280  /**< Decryption of block requires a full block. */
#define POLARSSL_ERR_CIPHER_AUTH_FAILED                    -0x6300  /**< Authentication failed (for AEAD modes). */

#define POLARSSL_CIPHER_VARIABLE_IV_LEN     0x01    /**< Cipher accepts IVs of variable length */
#define POLARSSL_CIPHER_VARIABLE_KEY_LEN    0x02    /**< Cipher accepts keys of variable length */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    POLARSSL_CIPHER_ID_NONE = 0,
    POLARSSL_CIPHER_ID_NULL,
    POLARSSL_CIPHER_ID_AES,
    POLARSSL_CIPHER_ID_DES,
    POLARSSL_CIPHER_ID_3DES,
    POLARSSL_CIPHER_ID_CAMELLIA,
    POLARSSL_CIPHER_ID_BLOWFISH,
    POLARSSL_CIPHER_ID_ARC4,
} cipher_id_t;

typedef enum {
    POLARSSL_CIPHER_NONE = 0,
    POLARSSL_CIPHER_NULL,
    POLARSSL_CIPHER_AES_128_ECB,
    POLARSSL_CIPHER_AES_192_ECB,
    POLARSSL_CIPHER_AES_256_ECB,
    POLARSSL_CIPHER_AES_128_CBC,
    POLARSSL_CIPHER_AES_192_CBC,
    POLARSSL_CIPHER_AES_256_CBC,
    POLARSSL_CIPHER_AES_128_CFB128,
    POLARSSL_CIPHER_AES_192_CFB128,
    POLARSSL_CIPHER_AES_256_CFB128,
    POLARSSL_CIPHER_AES_128_CTR,
    POLARSSL_CIPHER_AES_192_CTR,
    POLARSSL_CIPHER_AES_256_CTR,
    POLARSSL_CIPHER_AES_128_GCM,
    POLARSSL_CIPHER_AES_192_GCM,
    POLARSSL_CIPHER_AES_256_GCM,
    POLARSSL_CIPHER_CAMELLIA_128_ECB,
    POLARSSL_CIPHER_CAMELLIA_192_ECB,
    POLARSSL_CIPHER_CAMELLIA_256_ECB,
    POLARSSL_CIPHER_CAMELLIA_128_CBC,
    POLARSSL_CIPHER_CAMELLIA_192_CBC,
    POLARSSL_CIPHER_CAMELLIA_256_CBC,
    POLARSSL_CIPHER_CAMELLIA_128_CFB128,
    POLARSSL_CIPHER_CAMELLIA_192_CFB128,
    POLARSSL_CIPHER_CAMELLIA_256_CFB128,
    POLARSSL_CIPHER_CAMELLIA_128_CTR,
    POLARSSL_CIPHER_CAMELLIA_192_CTR,
    POLARSSL_CIPHER_CAMELLIA_256_CTR,
    POLARSSL_CIPHER_CAMELLIA_128_GCM,
    POLARSSL_CIPHER_CAMELLIA_192_GCM,
    POLARSSL_CIPHER_CAMELLIA_256_GCM,
    POLARSSL_CIPHER_DES_ECB,
    POLARSSL_CIPHER_DES_CBC,
    POLARSSL_CIPHER_DES_EDE_ECB,
    POLARSSL_CIPHER_DES_EDE_CBC,
    POLARSSL_CIPHER_DES_EDE3_ECB,
    POLARSSL_CIPHER_DES_EDE3_CBC,
    POLARSSL_CIPHER_BLOWFISH_ECB,
    POLARSSL_CIPHER_BLOWFISH_CBC,
    POLARSSL_CIPHER_BLOWFISH_CFB64,
    POLARSSL_CIPHER_BLOWFISH_CTR,
    POLARSSL_CIPHER_ARC4_128,
    POLARSSL_CIPHER_AES_128_CCM,
    POLARSSL_CIPHER_AES_192_CCM,
    POLARSSL_CIPHER_AES_256_CCM,
    POLARSSL_CIPHER_CAMELLIA_128_CCM,
    POLARSSL_CIPHER_CAMELLIA_192_CCM,
    POLARSSL_CIPHER_CAMELLIA_256_CCM,
} cipher_type_t;

typedef enum {
    POLARSSL_MODE_NONE = 0,
    POLARSSL_MODE_ECB,
    POLARSSL_MODE_CBC,
    POLARSSL_MODE_CFB,
    POLARSSL_MODE_OFB, /* Unused! */
    POLARSSL_MODE_CTR,
    POLARSSL_MODE_GCM,
    POLARSSL_MODE_STREAM,
    POLARSSL_MODE_CCM,
} cipher_mode_t;

typedef enum {
    POLARSSL_PADDING_PKCS7 = 0,     /**< PKCS7 padding (default)        */
    POLARSSL_PADDING_ONE_AND_ZEROS, /**< ISO/IEC 7816-4 padding         */
    POLARSSL_PADDING_ZEROS_AND_LEN, /**< ANSI X.923 padding             */
    POLARSSL_PADDING_ZEROS,         /**< zero padding (not reversible!) */
    POLARSSL_PADDING_NONE,          /**< never pad (full blocks only)   */
} cipher_padding_t;

typedef enum {
    POLARSSL_OPERATION_NONE = -1,
    POLARSSL_DECRYPT = 0,
    POLARSSL_ENCRYPT,
} operation_t;

enum {
    /** Undefined key length */
    POLARSSL_KEY_LENGTH_NONE = 0,
    /** Key length, in bits (including parity), for DES keys */
    POLARSSL_KEY_LENGTH_DES  = 64,
    /** Key length, in bits (including parity), for DES in two key EDE */
    POLARSSL_KEY_LENGTH_DES_EDE = 128,
    /** Key length, in bits (including parity), for DES in three-key EDE */
    POLARSSL_KEY_LENGTH_DES_EDE3 = 192,
};

/** Maximum length of any IV, in bytes */
#define POLARSSL_MAX_IV_LENGTH      16
/** Maximum block size of any cipher, in bytes */
#define POLARSSL_MAX_BLOCK_LENGTH   16

/**
 * Base cipher information. The non-mode specific functions and values.
 */
typedef struct {

    /** Base Cipher type (e.g. POLARSSL_CIPHER_ID_AES) */
    cipher_id_t cipher;

    /** Encrypt using ECB */
    int (*ecb_func)( void *ctx, operation_t mode,
                     const unsigned char *input, unsigned char *output );

    /** Encrypt using CBC */
    int (*cbc_func)( void *ctx, operation_t mode, size_t length,
                     unsigned char *iv, const unsigned char *input,
                     unsigned char *output );

    /** Encrypt using CFB (Full length) */
    int (*cfb_func)( void *ctx, operation_t mode, size_t length, size_t *iv_off,
                     unsigned char *iv, const unsigned char *input,
                     unsigned char *output );

    /** Encrypt using CTR */
    int (*ctr_func)( void *ctx, size_t length, size_t *nc_off,
                     unsigned char *nonce_counter, unsigned char *stream_block,
                     const unsigned char *input, unsigned char *output );

    /** Encrypt using STREAM */
    int (*stream_func)( void *ctx, size_t length,
                        const unsigned char *input, unsigned char *output );

    /** Set key for encryption purposes */
    int (*setkey_enc_func)( void *ctx, const unsigned char *key,
                            unsigned int key_length );

    /** Set key for decryption purposes */
    int (*setkey_dec_func)( void *ctx, const unsigned char *key,
                            unsigned int key_length);

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

} cipher_base_t;

/**
 * Cipher information. Allows cipher functions to be called in a generic way.
 */
typedef struct {
    /** Full cipher identifier (e.g. POLARSSL_CIPHER_AES_256_CBC) */
    cipher_type_t type;

    /** Cipher mode (e.g. POLARSSL_MODE_CBC) */
    cipher_mode_t mode;

    /** Cipher key length, in bits (default length for variable sized ciphers)
     *  (Includes parity bits for ciphers like DES) */
    unsigned int key_length;

    /** Name of the cipher */
    const char * name;

    /** IV/NONCE size, in bytes.
     *  For cipher that accept many sizes: recommended size */
    unsigned int iv_size;

    /** Flags for variable IV size, variable key size, etc. */
    int flags;

    /** block size, in bytes */
    unsigned int block_size;

    /** Base cipher information and functions */
    const cipher_base_t *base;

} cipher_info_t;

/**
 * Generic cipher context.
 */
typedef struct {
    /** Information about the associated cipher */
    const cipher_info_t *cipher_info;

    /** Key length to use */
    int key_length;

    /** Operation that the context's key has been initialised for */
    operation_t operation;

    /** Padding functions to use, if relevant for cipher mode */
    void (*add_padding)( unsigned char *output, size_t olen, size_t data_len );
    int (*get_padding)( unsigned char *input, size_t ilen, size_t *data_len );

    /** Buffer for data that hasn't been encrypted yet */
    unsigned char unprocessed_data[POLARSSL_MAX_BLOCK_LENGTH];

    /** Number of bytes that still need processing */
    size_t unprocessed_len;

    /** Current IV or NONCE_COUNTER for CTR-mode */
    unsigned char iv[POLARSSL_MAX_IV_LENGTH];

    /** IV size in bytes (for ciphers with variable-length IVs) */
    size_t iv_size;

    /** Cipher-specific context */
    void *cipher_ctx;
} cipher_context_t;

/**
 * \brief Returns the list of ciphers supported by the generic cipher module.
 *
 * \return              a statically allocated array of ciphers, the last entry
 *                      is 0.
 */
const int *cipher_list( void );

/**
 * \brief               Returns the cipher information structure associated
 *                      with the given cipher name.
 *
 * \param cipher_name   Name of the cipher to search for.
 *
 * \return              the cipher information structure associated with the
 *                      given cipher_name, or NULL if not found.
 */
const cipher_info_t *cipher_info_from_string( const char *cipher_name );

/**
 * \brief               Returns the cipher information structure associated
 *                      with the given cipher type.
 *
 * \param cipher_type   Type of the cipher to search for.
 *
 * \return              the cipher information structure associated with the
 *                      given cipher_type, or NULL if not found.
 */
const cipher_info_t *cipher_info_from_type( const cipher_type_t cipher_type );

/**
 * \brief               Returns the cipher information structure associated
 *                      with the given cipher id, key size and mode.
 *
 * \param cipher_id     Id of the cipher to search for
 *                      (e.g. POLARSSL_CIPHER_ID_AES)
 * \param key_length    Length of the key in bits
 * \param mode          Cipher mode (e.g. POLARSSL_MODE_CBC)
 *
 * \return              the cipher information structure associated with the
 *                      given cipher_type, or NULL if not found.
 */
const cipher_info_t *cipher_info_from_values( const cipher_id_t cipher_id,
                                              int key_length,
                                              const cipher_mode_t mode );

/**
 * \brief               Initialize a cipher_context (as NONE)
 */
void cipher_init( cipher_context_t *ctx );

/**
 * \brief               Free and clear the cipher-specific context of ctx.
 *                      Freeing ctx itself remains the responsibility of the
 *                      caller.
 */
void cipher_free( cipher_context_t *ctx );

/**
 * \brief               Initialises and fills the cipher context structure with
 *                      the appropriate values.
 *
 * \note                Currently also clears structure. In future versions you
 *                      will be required to call cipher_init() on the structure
 *                      first.
 *
 * \param ctx           context to initialise. May not be NULL.
 * \param cipher_info   cipher to use.
 *
 * \return              0 on success,
 *                      POLARSSL_ERR_CIPHER_BAD_INPUT_DATA on parameter failure,
 *                      POLARSSL_ERR_CIPHER_ALLOC_FAILED if allocation of the
 *                      cipher-specific context failed.
 */
int cipher_init_ctx( cipher_context_t *ctx, const cipher_info_t *cipher_info );

/**
 * \brief               Free the cipher-specific context of ctx. Freeing ctx
 *                      itself remains the responsibility of the caller.
 *
 * \note                Deprecated: Redirects to cipher_free()
 *
 * \param ctx           Free the cipher-specific context
 *
 * \returns             0
 */
int cipher_free_ctx( cipher_context_t *ctx );

/**
 * \brief               Returns the block size of the given cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              size of the cipher's blocks, or 0 if ctx has not been
 *                      initialised.
 */
static inline unsigned int cipher_get_block_size( const cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return 0;

    return ctx->cipher_info->block_size;
}

/**
 * \brief               Returns the mode of operation for the cipher.
 *                      (e.g. POLARSSL_MODE_CBC)
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              mode of operation, or POLARSSL_MODE_NONE if ctx
 *                      has not been initialised.
 */
static inline cipher_mode_t cipher_get_cipher_mode( const cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return POLARSSL_MODE_NONE;

    return ctx->cipher_info->mode;
}

/**
 * \brief               Returns the size of the cipher's IV/NONCE in bytes.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              If IV has not been set yet: (recommended) IV size
 *                      (0 for ciphers not using IV/NONCE).
 *                      If IV has already been set: actual size.
 */
static inline int cipher_get_iv_size( const cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return 0;

    if( ctx->iv_size != 0 )
        return (int) ctx->iv_size;

    return ctx->cipher_info->iv_size;
}

/**
 * \brief               Returns the type of the given cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              type of the cipher, or POLARSSL_CIPHER_NONE if ctx has
 *                      not been initialised.
 */
static inline cipher_type_t cipher_get_type( const cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return POLARSSL_CIPHER_NONE;

    return ctx->cipher_info->type;
}

/**
 * \brief               Returns the name of the given cipher, as a string.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              name of the cipher, or NULL if ctx was not initialised.
 */
static inline const char *cipher_get_name( const cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return 0;

    return ctx->cipher_info->name;
}

/**
 * \brief               Returns the key length of the cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              cipher's key length, in bits, or
 *                      POLARSSL_KEY_LENGTH_NONE if ctx has not been
 *                      initialised.
 */
static inline int cipher_get_key_size ( const cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return POLARSSL_KEY_LENGTH_NONE;

    return ctx->cipher_info->key_length;
}

/**
 * \brief               Returns the operation of the given cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              operation (POLARSSL_ENCRYPT or POLARSSL_DECRYPT),
 *                      or POLARSSL_OPERATION_NONE if ctx has not been
 *                      initialised.
 */
static inline operation_t cipher_get_operation( const cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return POLARSSL_OPERATION_NONE;

    return ctx->operation;
}

/**
 * \brief               Set the key to use with the given context.
 *
 * \param ctx           generic cipher context. May not be NULL. Must have been
 *                      initialised using cipher_context_from_type or
 *                      cipher_context_from_string.
 * \param key           The key to use.
 * \param key_length    key length to use, in bits.
 * \param operation     Operation that the key will be used for, either
 *                      POLARSSL_ENCRYPT or POLARSSL_DECRYPT.
 *
 * \returns             0 on success, POLARSSL_ERR_CIPHER_BAD_INPUT_DATA if
 *                      parameter verification fails or a cipher specific
 *                      error code.
 */
int cipher_setkey( cipher_context_t *ctx, const unsigned char *key,
                   int key_length, const operation_t operation );

#if defined(POLARSSL_CIPHER_MODE_WITH_PADDING)
/**
 * \brief               Set padding mode, for cipher modes that use padding.
 *                      (Default: PKCS7 padding.)
 *
 * \param ctx           generic cipher context
 * \param mode          padding mode
 *
 * \returns             0 on success, POLARSSL_ERR_CIPHER_FEATURE_UNAVAILABLE
 *                      if selected padding mode is not supported, or
 *                      POLARSSL_ERR_CIPHER_BAD_INPUT_DATA if the cipher mode
 *                      does not support padding.
 */
int cipher_set_padding_mode( cipher_context_t *ctx, cipher_padding_t mode );
#endif /* POLARSSL_CIPHER_MODE_WITH_PADDING */

/**
 * \brief               Set the initialization vector (IV) or nonce
 *
 * \param ctx           generic cipher context
 * \param iv            IV to use (or NONCE_COUNTER for CTR-mode ciphers)
 * \param iv_len        IV length for ciphers with variable-size IV;
 *                      discarded by ciphers with fixed-size IV.
 *
 * \returns             0 on success, or POLARSSL_ERR_CIPHER_BAD_INPUT_DATA
 *
 * \note                Some ciphers don't use IVs nor NONCE. For these
 *                      ciphers, this function has no effect.
 */
int cipher_set_iv( cipher_context_t *ctx,
                   const unsigned char *iv, size_t iv_len );

/**
 * \brief               Finish preparation of the given context
 *
 * \param ctx           generic cipher context
 *
 * \returns             0 on success, POLARSSL_ERR_CIPHER_BAD_INPUT_DATA
 *                      if parameter verification fails.
 */
int cipher_reset( cipher_context_t *ctx );

#if defined(POLARSSL_GCM_C)
/**
 * \brief               Add additional data (for AEAD ciphers).
 *                      Currently only supported with GCM.
 *                      Must be called exactly once, after cipher_reset().
 *
 * \param ctx           generic cipher context
 * \param ad            Additional data to use.
 * \param ad_len        Length of ad.
 *
 * \return              0 on success, or a specific error code.
 */
int cipher_update_ad( cipher_context_t *ctx,
                      const unsigned char *ad, size_t ad_len );
#endif /* POLARSSL_GCM_C */

/**
 * \brief               Generic cipher update function. Encrypts/decrypts
 *                      using the given cipher context. Writes as many block
 *                      size'd blocks of data as possible to output. Any data
 *                      that cannot be written immediately will either be added
 *                      to the next block, or flushed when cipher_final is
 *                      called.
 *                      Exception: for POLARSSL_MODE_ECB, expects single block
 *                                 in size (e.g. 16 bytes for AES)
 *
 * \param ctx           generic cipher context
 * \param input         buffer holding the input data
 * \param ilen          length of the input data
 * \param output        buffer for the output data. Should be able to hold at
 *                      least ilen + block_size. Cannot be the same buffer as
 *                      input!
 * \param olen          length of the output data, will be filled with the
 *                      actual number of bytes written.
 *
 * \returns             0 on success, POLARSSL_ERR_CIPHER_BAD_INPUT_DATA if
 *                      parameter verification fails,
 *                      POLARSSL_ERR_CIPHER_FEATURE_UNAVAILABLE on an
 *                      unsupported mode for a cipher or a cipher specific
 *                      error code.
 *
 * \note                If the underlying cipher is GCM, all calls to this
 *                      function, except the last one before cipher_finish(),
 *                      must have ilen a multiple of the block size.
 */
int cipher_update( cipher_context_t *ctx, const unsigned char *input,
                   size_t ilen, unsigned char *output, size_t *olen );

/**
 * \brief               Generic cipher finalisation function. If data still
 *                      needs to be flushed from an incomplete block, data
 *                      contained within it will be padded with the size of
 *                      the last block, and written to the output buffer.
 *
 * \param ctx           Generic cipher context
 * \param output        buffer to write data to. Needs block_size available.
 * \param olen          length of the data written to the output buffer.
 *
 * \returns             0 on success, POLARSSL_ERR_CIPHER_BAD_INPUT_DATA if
 *                      parameter verification fails,
 *                      POLARSSL_ERR_CIPHER_FULL_BLOCK_EXPECTED if decryption
 *                      expected a full block but was not provided one,
 *                      POLARSSL_ERR_CIPHER_INVALID_PADDING on invalid padding
 *                      while decrypting or a cipher specific error code.
 */
int cipher_finish( cipher_context_t *ctx,
                   unsigned char *output, size_t *olen );

#if defined(POLARSSL_GCM_C)
/**
 * \brief               Write tag for AEAD ciphers.
 *                      Currently only supported with GCM.
 *                      Must be called after cipher_finish().
 *
 * \param ctx           Generic cipher context
 * \param tag           buffer to write the tag
 * \param tag_len       Length of the tag to write
 *
 * \return              0 on success, or a specific error code.
 */
int cipher_write_tag( cipher_context_t *ctx,
                      unsigned char *tag, size_t tag_len );

/**
 * \brief               Check tag for AEAD ciphers.
 *                      Currently only supported with GCM.
 *                      Must be called after cipher_finish().
 *
 * \param ctx           Generic cipher context
 * \param tag           Buffer holding the tag
 * \param tag_len       Length of the tag to check
 *
 * \return              0 on success, or a specific error code.
 */
int cipher_check_tag( cipher_context_t *ctx,
                      const unsigned char *tag, size_t tag_len );
#endif /* POLARSSL_GCM_C */

/**
 * \brief               Generic all-in-one encryption/decryption
 *                      (for all ciphers except AEAD constructs).
 *
 * \param ctx           generic cipher context
 * \param iv            IV to use (or NONCE_COUNTER for CTR-mode ciphers)
 * \param iv_len        IV length for ciphers with variable-size IV;
 *                      discarded by ciphers with fixed-size IV.
 * \param input         buffer holding the input data
 * \param ilen          length of the input data
 * \param output        buffer for the output data. Should be able to hold at
 *                      least ilen + block_size. Cannot be the same buffer as
 *                      input!
 * \param olen          length of the output data, will be filled with the
 *                      actual number of bytes written.
 *
 * \note                Some ciphers don't use IVs nor NONCE. For these
 *                      ciphers, use iv = NULL and iv_len = 0.
 *
 * \returns             0 on success, or
 *                      POLARSSL_ERR_CIPHER_BAD_INPUT_DATA, or
 *                      POLARSSL_ERR_CIPHER_FULL_BLOCK_EXPECTED if decryption
 *                      expected a full block but was not provided one, or
 *                      POLARSSL_ERR_CIPHER_INVALID_PADDING on invalid padding
 *                      while decrypting, or
 *                      a cipher specific error code.
 */
int cipher_crypt( cipher_context_t *ctx,
                  const unsigned char *iv, size_t iv_len,
                  const unsigned char *input, size_t ilen,
                  unsigned char *output, size_t *olen );

#if defined(POLARSSL_CIPHER_MODE_AEAD)
/**
 * \brief               Generic autenticated encryption (AEAD ciphers).
 *
 * \param ctx           generic cipher context
 * \param iv            IV to use (or NONCE_COUNTER for CTR-mode ciphers)
 * \param iv_len        IV length for ciphers with variable-size IV;
 *                      discarded by ciphers with fixed-size IV.
 * \param ad            Additional data to authenticate.
 * \param ad_len        Length of ad.
 * \param input         buffer holding the input data
 * \param ilen          length of the input data
 * \param output        buffer for the output data.
 *                      Should be able to hold at least ilen.
 * \param olen          length of the output data, will be filled with the
 *                      actual number of bytes written.
 * \param tag           buffer for the authentication tag
 * \param tag_len       desired tag length
 *
 * \returns             0 on success, or
 *                      POLARSSL_ERR_CIPHER_BAD_INPUT_DATA, or
 *                      a cipher specific error code.
 */
int cipher_auth_encrypt( cipher_context_t *ctx,
                         const unsigned char *iv, size_t iv_len,
                         const unsigned char *ad, size_t ad_len,
                         const unsigned char *input, size_t ilen,
                         unsigned char *output, size_t *olen,
                         unsigned char *tag, size_t tag_len );

/**
 * \brief               Generic autenticated decryption (AEAD ciphers).
 *
 * \param ctx           generic cipher context
 * \param iv            IV to use (or NONCE_COUNTER for CTR-mode ciphers)
 * \param iv_len        IV length for ciphers with variable-size IV;
 *                      discarded by ciphers with fixed-size IV.
 * \param ad            Additional data to be authenticated.
 * \param ad_len        Length of ad.
 * \param input         buffer holding the input data
 * \param ilen          length of the input data
 * \param output        buffer for the output data.
 *                      Should be able to hold at least ilen.
 * \param olen          length of the output data, will be filled with the
 *                      actual number of bytes written.
 * \param tag           buffer holding the authentication tag
 * \param tag_len       length of the authentication tag
 *
 * \returns             0 on success, or
 *                      POLARSSL_ERR_CIPHER_BAD_INPUT_DATA, or
 *                      POLARSSL_ERR_CIPHER_AUTH_FAILED if data isn't authentic,
 *                      or a cipher specific error code.
 *
 * \note                If the data is not authentic, then the output buffer
 *                      is zeroed out to prevent the unauthentic plaintext to
 *                      be used by mistake, making this interface safer.
 */
int cipher_auth_decrypt( cipher_context_t *ctx,
                         const unsigned char *iv, size_t iv_len,
                         const unsigned char *ad, size_t ad_len,
                         const unsigned char *input, size_t ilen,
                         unsigned char *output, size_t *olen,
                         const unsigned char *tag, size_t tag_len );
#endif /* POLARSSL_CIPHER_MODE_AEAD */

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int cipher_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* POLARSSL_CIPHER_H */
