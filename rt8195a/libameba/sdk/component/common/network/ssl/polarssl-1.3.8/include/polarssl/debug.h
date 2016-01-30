/**
 * \file debug.h
 *
 * \brief Debug functions
 *
 *  Copyright (C) 2006-2011, Brainspark B.V.
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
#ifndef POLARSSL_DEBUG_H
#define POLARSSL_DEBUG_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif
#include "ssl.h"
#if defined(POLARSSL_ECP_C)
#include "ecp.h"
#endif

#if defined(POLARSSL_DEBUG_C)

#define POLARSSL_DEBUG_LOG_FULL         0 /**< Include file:line in log lines */
#define POLARSSL_DEBUG_LOG_RAW          1 /**< Only log raw debug lines */

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in config.h or define them on the compiler command line.
 * \{
 */

#if !defined(POLARSSL_DEBUG_DFL_MODE)
#define POLARSSL_DEBUG_DFL_MODE POLARSSL_DEBUG_LOG_FULL /**< Default log: Full or Raw */
#endif

/* \} name SECTION: Module settings */


#define SSL_DEBUG_MSG( level, args )                    \
    debug_print_msg( ssl, level, __FILE__, __LINE__, debug_fmt args );

#define SSL_DEBUG_RET( level, text, ret )                \
    debug_print_ret( ssl, level, __FILE__, __LINE__, text, ret );

#define SSL_DEBUG_BUF( level, text, buf, len )           \
    debug_print_buf( ssl, level, __FILE__, __LINE__, text, buf, len );

#if defined(POLARSSL_BIGNUM_C)
#define SSL_DEBUG_MPI( level, text, X )                  \
    debug_print_mpi( ssl, level, __FILE__, __LINE__, text, X );
#endif

#if defined(POLARSSL_ECP_C)
#define SSL_DEBUG_ECP( level, text, X )                  \
    debug_print_ecp( ssl, level, __FILE__, __LINE__, text, X );
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C)
#define SSL_DEBUG_CRT( level, text, crt )                \
    debug_print_crt( ssl, level, __FILE__, __LINE__, text, crt );
#endif

#else /* POLARSSL_DEBUG_C */

#define SSL_DEBUG_MSG( level, args )            do { } while( 0 )
#define SSL_DEBUG_RET( level, text, ret )       do { } while( 0 )
#define SSL_DEBUG_BUF( level, text, buf, len )  do { } while( 0 )
#define SSL_DEBUG_MPI( level, text, X )         do { } while( 0 )
#define SSL_DEBUG_ECP( level, text, X )         do { } while( 0 )
#define SSL_DEBUG_CRT( level, text, crt )       do { } while( 0 )

#endif /* POLARSSL_DEBUG_C */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   Set the log mode for the debug functions globally
 *          (Default value: POLARSSL_DEBUG_DFL_MODE)
 *
 * \param log_mode      The log mode to use (POLARSSL_DEBUG_LOG_FULL or
 *                                           POLARSSL_DEBUG_LOG_RAW)
 */
void debug_set_log_mode( int log_mode );

/**
 * \brief   Set the level threshold to handle globally. Messages that have a
 *          level over the threshold value are ignored.
 *          (Default value: 0 (No debug))
 *
 * \param threshold     maximum level of messages to pass on
 */
void debug_set_threshold( int threshold );

char *debug_fmt( const char *format, ... );

void debug_print_msg( const ssl_context *ssl, int level,
                      const char *file, int line, const char *text );

void debug_print_ret( const ssl_context *ssl, int level,
                      const char *file, int line,
                      const char *text, int ret );

void debug_print_buf( const ssl_context *ssl, int level,
                      const char *file, int line, const char *text,
                      unsigned char *buf, size_t len );

#if defined(POLARSSL_BIGNUM_C)
void debug_print_mpi( const ssl_context *ssl, int level,
                      const char *file, int line,
                      const char *text, const mpi *X );
#endif

#if defined(POLARSSL_ECP_C)
void debug_print_ecp( const ssl_context *ssl, int level,
                      const char *file, int line,
                      const char *text, const ecp_point *X );
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C)
void debug_print_crt( const ssl_context *ssl, int level,
                      const char *file, int line,
                      const char *text, const x509_crt *crt );
#endif

#ifdef __cplusplus
}
#endif

#endif /* debug.h */
