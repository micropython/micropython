/**
 * \file platform.h
 *
 * \brief PolarSSL Platform abstraction layer
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
#ifndef POLARSSL_PLATFORM_H
#define POLARSSL_PLATFORM_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "rom_ssl_ram_map.h"

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in config.h or define them on the compiler command line.
 * \{
 */

#if !defined(POLARSSL_PLATFORM_NO_STD_FUNCTIONS)
#include <stdlib.h>
#if !defined(POLARSSL_PLATFORM_STD_PRINTF)
#define POLARSSL_PLATFORM_STD_PRINTF   printf /**< Default printf to use  */
#endif
#if !defined(POLARSSL_PLATFORM_STD_FPRINTF)
#define POLARSSL_PLATFORM_STD_FPRINTF fprintf /**< Default fprintf to use */
#endif
#if !defined(POLARSSL_PLATFORM_STD_MALLOC)
#define POLARSSL_PLATFORM_STD_MALLOC   malloc /**< Default allocator to use */
#endif
#if !defined(POLARSSL_PLATFORM_STD_FREE)
#define POLARSSL_PLATFORM_STD_FREE       free /**< Default free to use */
#endif
#else /* POLARSSL_PLATFORM_NO_STD_FUNCTIONS */
#if defined(POLARSSL_PLATFORM_STD_MEM_HDR)
#include POLARSSL_PLATFORM_STD_MEM_HDR
#endif
#endif /* POLARSSL_PLATFORM_NO_STD_FUNCTIONS */

/* \} name SECTION: Module settings */

/*
 * The function pointers for malloc and free
 */
#if defined(POLARSSL_PLATFORM_MEMORY)
extern void * (*polarssl_malloc)( size_t len );
extern void (*polarssl_free)( void *ptr );

/**
 * \brief   Set your own memory implementation function pointers
 *
 * \param malloc_func   the malloc function implementation
 * \param free_func     the free function implementation
 *
 * \return              0 if successful
 */
int platform_set_malloc_free( void * (*malloc_func)( size_t ),
                              void (*free_func)( void * ) );
#else /* POLARSSL_PLATFORM_ENTROPY */
#define polarssl_malloc     rom_ssl_ram_map.ssl_malloc
#define polarssl_free       rom_ssl_ram_map.ssl_free
#endif /* POLARSSL_PLATFORM_ENTROPY */

/*
 * The function pointers for printf
 */
#if defined(POLARSSL_PLATFORM_PRINTF_ALT)
extern int (*polarssl_printf)( const char *format, ... );

/**
 * \brief   Set your own printf function pointer
 *
 * \param printf_func   the printf function implementation
 *
 * \return              0
 */
int platform_set_printf( int (*printf_func)( const char *, ... ) );
#else /* POLARSSL_PLATFORM_PRINTF_ALT */
#define polarssl_printf     rom_ssl_ram_map.ssl_printf
#endif /* POLARSSL_PLATFORM_PRINTF_ALT */

/*
 * The function pointers for fprintf
 */
#if defined(POLARSSL_PLATFORM_FPRINTF_ALT)
extern int (*polarssl_fprintf)( FILE *stream, const char *format, ... );

int platform_set_fprintf( int (*fprintf_func)( FILE *stream, const char *,
                                               ... ) );
#else
#define polarssl_fprintf    fprintf
#endif

#ifdef __cplusplus
}
#endif

#endif /* platform.h */
