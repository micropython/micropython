/**
 * \file threading.h
 *
 * \brief Threading abstraction layer
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
#ifndef POLARSSL_THREADING_H
#define POLARSSL_THREADING_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POLARSSL_ERR_THREADING_FEATURE_UNAVAILABLE         -0x001A  /**< The selected feature is not available. */
#define POLARSSL_ERR_THREADING_BAD_INPUT_DATA              -0x001C  /**< Bad input parameters to function. */
#define POLARSSL_ERR_THREADING_MUTEX_ERROR                 -0x001E  /**< Locking / unlocking / free failed with error code. */

#if defined(POLARSSL_THREADING_PTHREAD)
#include <pthread.h>
typedef pthread_mutex_t threading_mutex_t;
#endif

#if defined(POLARSSL_THREADING_ALT)
/* You should define the threading_mutex_t type in your header */
#include "threading_alt.h"

/**
 * \brief           Set your alternate threading implementation function
 *                  pointers
 *
 * \param mutex_init    the init function implementation
 * \param mutex_free    the free function implementation
 * \param mutex_lock    the lock function implementation
 * \param mutex_unlock  the unlock function implementation
 *
 * \return              0 if successful
 */
int threading_set_alt( int (*mutex_init)( threading_mutex_t * ),
                       int (*mutex_free)( threading_mutex_t * ),
                       int (*mutex_lock)( threading_mutex_t * ),
                       int (*mutex_unlock)( threading_mutex_t * ) );
#endif /* POLARSSL_THREADING_ALT_C */

/*
 * The function pointers for mutex_init, mutex_free, mutex_ and mutex_unlock
 *
 * All these functions are expected to work or the result will be undefined.
 */
extern int (*polarssl_mutex_init)( threading_mutex_t *mutex );
extern int (*polarssl_mutex_free)( threading_mutex_t *mutex );
extern int (*polarssl_mutex_lock)( threading_mutex_t *mutex );
extern int (*polarssl_mutex_unlock)( threading_mutex_t *mutex );

#ifdef __cplusplus
}
#endif

#endif /* threading.h */
