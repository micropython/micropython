/**
 * \file timing.h
 *
 * \brief Portable interface to the CPU cycle counter
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
#ifndef POLARSSL_TIMING_H
#define POLARSSL_TIMING_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if !defined(POLARSSL_TIMING_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          timer structure
 */
struct hr_time
{
    unsigned char opaque[32];
};

extern volatile int alarmed;

/**
 * \brief          Return the CPU cycle counter value
 */
unsigned long hardclock( void );

/**
 * \brief          Return the elapsed time in milliseconds
 *
 * \param val      points to a timer structure
 * \param reset    if set to 1, the timer is restarted
 */
unsigned long get_timer( struct hr_time *val, int reset );

/**
 * \brief          Setup an alarm clock
 *
 * \param seconds  delay before the "alarmed" flag is set
 */
void set_alarm( int seconds );

/**
 * \brief          Sleep for a certain amount of time
 *
 * \param milliseconds  delay in milliseconds
 */
void m_sleep( int milliseconds );

#if defined(POLARSSL_SELF_TEST)
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if a test failed
 */
int timing_self_test( int verbose );
#endif

#ifdef __cplusplus
}
#endif

#else  /* POLARSSL_TIMING_ALT */
#include "timing_alt.h"
#endif /* POLARSSL_TIMING_ALT */

#endif /* timing.h */
