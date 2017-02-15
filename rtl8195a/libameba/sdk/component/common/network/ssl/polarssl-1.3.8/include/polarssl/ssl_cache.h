/**
 * \file ssl_cache.h
 *
 * \brief SSL session cache implementation
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
#ifndef POLARSSL_SSL_CACHE_H
#define POLARSSL_SSL_CACHE_H

#include "ssl.h"

#if defined(POLARSSL_THREADING_C)
#include "threading.h"
#endif

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in config.h or define them on the compiler command line.
 * \{
 */

#if !defined(SSL_CACHE_DEFAULT_TIMEOUT)
#define SSL_CACHE_DEFAULT_TIMEOUT       86400   /*!< 1 day  */
#endif

#if !defined(SSL_CACHE_DEFAULT_MAX_ENTRIES)
#define SSL_CACHE_DEFAULT_MAX_ENTRIES      50   /*!< Maximum entries in cache */
#endif

/* \} name SECTION: Module settings */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ssl_cache_context ssl_cache_context;
typedef struct _ssl_cache_entry ssl_cache_entry;

/**
 * \brief   This structure is used for storing cache entries
 */
struct _ssl_cache_entry
{
#if defined(POLARSSL_HAVE_TIME)
    time_t timestamp;           /*!< entry timestamp    */
#endif
    ssl_session session;        /*!< entry session      */
#if defined(POLARSSL_X509_CRT_PARSE_C)
    x509_buf peer_cert;         /*!< entry peer_cert    */
#endif
    ssl_cache_entry *next;      /*!< chain pointer      */
};

/**
 * \brief Cache context
 */
struct _ssl_cache_context
{
    ssl_cache_entry *chain;     /*!< start of the chain     */
    int timeout;                /*!< cache entry timeout    */
    int max_entries;            /*!< maximum entries        */
#if defined(POLARSSL_THREADING_C)
    threading_mutex_t mutex;    /*!< mutex                  */
#endif
};

/**
 * \brief          Initialize an SSL cache context
 *
 * \param cache    SSL cache context
 */
void ssl_cache_init( ssl_cache_context *cache );

/**
 * \brief          Cache get callback implementation
 *                 (Thread-safe if POLARSSL_THREADING_C is enabled)
 *
 * \param data     SSL cache context
 * \param session  session to retrieve entry for
 */
int ssl_cache_get( void *data, ssl_session *session );

/**
 * \brief          Cache set callback implementation
 *                 (Thread-safe if POLARSSL_THREADING_C is enabled)
 *
 * \param data     SSL cache context
 * \param session  session to store entry for
 */
int ssl_cache_set( void *data, const ssl_session *session );

#if defined(POLARSSL_HAVE_TIME)
/**
 * \brief          Set the cache timeout
 *                 (Default: SSL_CACHE_DEFAULT_TIMEOUT (1 day))
 *
 *                 A timeout of 0 indicates no timeout.
 *
 * \param cache    SSL cache context
 * \param timeout  cache entry timeout in seconds
 */
void ssl_cache_set_timeout( ssl_cache_context *cache, int timeout );
#endif /* POLARSSL_HAVE_TIME */

/**
 * \brief          Set the cache timeout
 *                 (Default: SSL_CACHE_DEFAULT_MAX_ENTRIES (50))
 *
 * \param cache    SSL cache context
 * \param max      cache entry maximum
 */
void ssl_cache_set_max_entries( ssl_cache_context *cache, int max );

/**
 * \brief          Free referenced items in a cache context and clear memory
 *
 * \param cache    SSL cache context
 */
void ssl_cache_free( ssl_cache_context *cache );

#ifdef __cplusplus
}
#endif

#endif /* ssl_cache.h */
