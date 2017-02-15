/**
 * \file memory_buffer_alloc.h
 *
 * \brief Buffer-based memory allocator
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
#ifndef POLARSSL_MEMORY_BUFFER_ALLOC_H
#define POLARSSL_MEMORY_BUFFER_ALLOC_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <stdlib.h>

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in config.h or define them on the compiler command line.
 * \{
 */

#if !defined(POLARSSL_MEMORY_ALIGN_MULTIPLE)
#define POLARSSL_MEMORY_ALIGN_MULTIPLE       4 /**< Align on multiples of this value */
#endif

/* \} name SECTION: Module settings */

#define MEMORY_VERIFY_NONE         0
#define MEMORY_VERIFY_ALLOC        (1 << 0)
#define MEMORY_VERIFY_FREE         (1 << 1)
#define MEMORY_VERIFY_ALWAYS       (MEMORY_VERIFY_ALLOC | MEMORY_VERIFY_FREE)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   Initialize use of stack-based memory allocator.
 *          The stack-based allocator does memory management inside the
 *          presented buffer and does not call malloc() and free().
 *          It sets the global polarssl_malloc() and polarssl_free() pointers
 *          to its own functions.
 *          (Provided polarssl_malloc() and polarssl_free() are thread-safe if
 *           POLARSSL_THREADING_C is defined)
 *
 * \note    This code is not optimized and provides a straight-forward
 *          implementation of a stack-based memory allocator.
 *
 * \param buf   buffer to use as heap
 * \param len   size of the buffer
 *
 * \return              0 if successful
 */
int memory_buffer_alloc_init( unsigned char *buf, size_t len );

/**
 * \brief   Free the mutex for thread-safety and clear remaining memory
 */
void memory_buffer_alloc_free( void );

/**
 * \brief   Determine when the allocator should automatically verify the state
 *          of the entire chain of headers / meta-data.
 *          (Default: MEMORY_VERIFY_NONE)
 *
 * \param verify    One of MEMORY_VERIFY_NONE, MEMORY_VERIFY_ALLOC,
 *                  MEMORY_VERIFY_FREE or MEMORY_VERIFY_ALWAYS
 */
void memory_buffer_set_verify( int verify );

#if defined(POLARSSL_MEMORY_DEBUG)
/**
 * \brief   Print out the status of the allocated memory (primarily for use
 *          after a program should have de-allocated all memory)
 *          Prints out a list of 'still allocated' blocks and their stack
 *          trace if POLARSSL_MEMORY_BACKTRACE is defined.
 */
void memory_buffer_alloc_status( void );
#endif /* POLARSSL_MEMORY_DEBUG */

/**
 * \brief   Verifies that all headers in the memory buffer are correct
 *          and contain sane values. Helps debug buffer-overflow errors.
 *
 *          Prints out first failure if POLARSSL_MEMORY_DEBUG is defined.
 *          Prints out full header information if POLARSSL_MEMORY_DEBUG_HEADERS
 *          is defined. (Includes stack trace information for each block if
 *          POLARSSL_MEMORY_BACKTRACE is defined as well).
 *
 * \returns             0 if verified, 1 otherwise
 */
int memory_buffer_alloc_verify( void );

#ifdef __cplusplus
}
#endif

#endif /* memory_buffer_alloc.h */
