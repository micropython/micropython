/**
 * \file entropy_poll.h
 *
 * \brief Platform-specific and custom entropy polling functions
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
#ifndef POLARSSL_ENTROPY_POLL_H
#define POLARSSL_ENTROPY_POLL_H

#include <string.h>

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Default thresholds for built-in sources
 */
#define ENTROPY_MIN_PLATFORM    128     /**< Minimum for platform source    */
#define ENTROPY_MIN_HAVEGE      128     /**< Minimum for HAVEGE             */
#define ENTROPY_MIN_HARDCLOCK    32     /**< Minimum for hardclock()        */

#if !defined(POLARSSL_NO_PLATFORM_ENTROPY)
/**
 * \brief           Platform-specific entropy poll callback
 */
int platform_entropy_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(POLARSSL_HAVEGE_C)
/**
 * \brief           HAVEGE based entropy poll callback
 *
 * Requires an HAVEGE state as its data pointer.
 */
int havege_poll( void *data,
                 unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(POLARSSL_TIMING_C)
/**
 * \brief           hardclock-based entropy poll callback
 */
int hardclock_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen );
#endif

#ifdef __cplusplus
}
#endif

#endif /* entropy_poll.h */
