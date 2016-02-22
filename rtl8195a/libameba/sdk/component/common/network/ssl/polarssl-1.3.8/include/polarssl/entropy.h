/**
 * \file entropy.h
 *
 * \brief Entropy accumulator implementation
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
#ifndef POLARSSL_ENTROPY_H
#define POLARSSL_ENTROPY_H

#include <string.h>

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_SHA512_C) && !defined(POLARSSL_ENTROPY_FORCE_SHA256)
#include "sha512.h"
#define POLARSSL_ENTROPY_SHA512_ACCUMULATOR
#else
#if defined(POLARSSL_SHA256_C)
#define POLARSSL_ENTROPY_SHA256_ACCUMULATOR
#include "sha256.h"
#endif
#endif

#if defined(POLARSSL_THREADING_C)
#include "threading.h"
#endif

#if defined(POLARSSL_HAVEGE_C)
#include "havege.h"
#endif

#define POLARSSL_ERR_ENTROPY_SOURCE_FAILED                 -0x003C  /**< Critical entropy source failure. */
#define POLARSSL_ERR_ENTROPY_MAX_SOURCES                   -0x003E  /**< No more sources can be added. */
#define POLARSSL_ERR_ENTROPY_NO_SOURCES_DEFINED            -0x0040  /**< No sources have been added to poll. */
#define POLARSSL_ERR_ENTROPY_FILE_IO_ERROR                 -0x0058  /**< Read/write error in file. */

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in config.h or define them on the compiler command line.
 * \{
 */

#if !defined(ENTROPY_MAX_SOURCES)
#define ENTROPY_MAX_SOURCES     20      /**< Maximum number of sources supported */
#endif

#if !defined(ENTROPY_MAX_GATHER)
#define ENTROPY_MAX_GATHER      128     /**< Maximum amount requested from entropy sources */
#endif

/* \} name SECTION: Module settings */

#if defined(POLARSSL_ENTROPY_SHA512_ACCUMULATOR)
#define ENTROPY_BLOCK_SIZE      64      /**< Block size of entropy accumulator (SHA-512) */
#else
#define ENTROPY_BLOCK_SIZE      32      /**< Block size of entropy accumulator (SHA-256) */
#endif

#define ENTROPY_MAX_SEED_SIZE   1024    /**< Maximum size of seed we read from seed file */
#define ENTROPY_SOURCE_MANUAL   ENTROPY_MAX_SOURCES

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Entropy poll callback pointer
 *
 * \param data      Callback-specific data pointer
 * \param output    Data to fill
 * \param len       Maximum size to provide
 * \param olen      The actual amount of bytes put into the buffer (Can be 0)
 *
 * \return          0 if no critical failures occurred,
 *                  POLARSSL_ERR_ENTROPY_SOURCE_FAILED otherwise
 */
typedef int (*f_source_ptr)(void *data, unsigned char *output, size_t len,
                            size_t *olen);

/**
 * \brief           Entropy source state
 */
typedef struct
{
    f_source_ptr    f_source;   /**< The entropy source callback */
    void *          p_source;   /**< The callback data pointer */
    size_t          size;       /**< Amount received */
    size_t          threshold;  /**< Minimum level required before release */
}
source_state;

/**
 * \brief           Entropy context structure
 */
typedef struct
{
#if defined(POLARSSL_ENTROPY_SHA512_ACCUMULATOR)
    sha512_context  accumulator;
#else
    sha256_context  accumulator;
#endif
    int             source_count;
    source_state    source[ENTROPY_MAX_SOURCES];
#if defined(POLARSSL_HAVEGE_C)
    havege_state    havege_data;
#endif
#if defined(POLARSSL_THREADING_C)
    threading_mutex_t mutex;    /*!< mutex                  */
#endif
}
entropy_context;

/**
 * \brief           Initialize the context
 *
 * \param ctx       Entropy context to initialize
 */
void entropy_init( entropy_context *ctx );

/**
 * \brief           Free the data in the context
 *
 * \param ctx       Entropy context to free
 */
void entropy_free( entropy_context *ctx );

/**
 * \brief           Adds an entropy source to poll
 *                  (Thread-safe if POLARSSL_THREADING_C is enabled)
 *
 * \param ctx       Entropy context
 * \param f_source  Entropy function
 * \param p_source  Function data
 * \param threshold Minimum required from source before entropy is released
 *                  ( with entropy_func() )
 *
 * \return          0 if successful or POLARSSL_ERR_ENTROPY_MAX_SOURCES
 */
int entropy_add_source( entropy_context *ctx,
                        f_source_ptr f_source, void *p_source,
                        size_t threshold );

/**
 * \brief           Trigger an extra gather poll for the accumulator
 *                  (Thread-safe if POLARSSL_THREADING_C is enabled)
 *
 * \param ctx       Entropy context
 *
 * \return          0 if successful, or POLARSSL_ERR_ENTROPY_SOURCE_FAILED
 */
int entropy_gather( entropy_context *ctx );

/**
 * \brief           Retrieve entropy from the accumulator
 *                  (Maximum length: ENTROPY_BLOCK_SIZE)
 *                  (Thread-safe if POLARSSL_THREADING_C is enabled)
 *
 * \param data      Entropy context
 * \param output    Buffer to fill
 * \param len       Number of bytes desired, must be at most ENTROPY_BLOCK_SIZE
 *
 * \return          0 if successful, or POLARSSL_ERR_ENTROPY_SOURCE_FAILED
 */
int entropy_func( void *data, unsigned char *output, size_t len );

/**
 * \brief           Add data to the accumulator manually
 *                  (Thread-safe if POLARSSL_THREADING_C is enabled)
 *
 * \param ctx       Entropy context
 * \param data      Data to add
 * \param len       Length of data
 *
 * \return          0 if successful
 */
int entropy_update_manual( entropy_context *ctx,
                           const unsigned char *data, size_t len );

#if defined(POLARSSL_FS_IO)
/**
 * \brief               Write a seed file
 *
 * \param ctx           Entropy context
 * \param path          Name of the file
 *
 * \return              0 if successful,
 *                      POLARSSL_ERR_ENTROPY_FILE_IO_ERROR on file error, or
 *                      POLARSSL_ERR_ENTROPY_SOURCE_FAILED
 */
int entropy_write_seed_file( entropy_context *ctx, const char *path );

/**
 * \brief               Read and update a seed file. Seed is added to this
 *                      instance. No more than ENTROPY_MAX_SEED_SIZE bytes are
 *                      read from the seed file. The rest is ignored.
 *
 * \param ctx           Entropy context
 * \param path          Name of the file
 *
 * \return              0 if successful,
 *                      POLARSSL_ERR_ENTROPY_FILE_IO_ERROR on file error,
 *                      POLARSSL_ERR_ENTROPY_SOURCE_FAILED
 */
int entropy_update_seed_file( entropy_context *ctx, const char *path );
#endif /* POLARSSL_FS_IO */

#if defined(POLARSSL_SELF_TEST)
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if a test failed
 */
int entropy_self_test( int verbose );
#endif /* POLARSSL_SELF_TEST */

#ifdef __cplusplus
}
#endif

#endif /* entropy.h */
