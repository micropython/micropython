/**
 * \file version.h
 *
 * \brief Run-time version information
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
/*
 * This set of compile-time defines and run-time variables can be used to
 * determine the version number of the PolarSSL library used.
 */
#ifndef POLARSSL_VERSION_H
#define POLARSSL_VERSION_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

/**
 * The version number x.y.z is split into three parts.
 * Major, Minor, Patchlevel
 */
#define POLARSSL_VERSION_MAJOR  1
#define POLARSSL_VERSION_MINOR  3
#define POLARSSL_VERSION_PATCH  8

/**
 * The single version number has the following structure:
 *    MMNNPP00
 *    Major version | Minor version | Patch version
 */
#define POLARSSL_VERSION_NUMBER         0x01030800
#define POLARSSL_VERSION_STRING         "1.3.8"
#define POLARSSL_VERSION_STRING_FULL    "PolarSSL 1.3.8"

#if defined(POLARSSL_VERSION_C)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the version number.
 *
 * \return          The constructed version number in the format
 *                  MMNNPP00 (Major, Minor, Patch).
 */
unsigned int version_get_number( void );

/**
 * Get the version string ("x.y.z").
 *
 * \param string    The string that will receive the value.
 *                  (Should be at least 9 bytes in size)
 */
void version_get_string( char *string );

/**
 * Get the full version string ("PolarSSL x.y.z").
 *
 * \param string    The string that will receive the value. The PolarSSL version
 *                  string will use 18 bytes AT MOST including a terminating
 *                  null byte.
 *                  (So the buffer should be at least 18 bytes to receive this
 *                  version string).
 */
void version_get_string_full( char *string );

/**
 * \brief           Check if support for a feature was compiled into this
 *                  PolarSSL binary. This allows you to see at runtime if the
 *                  library was for instance compiled with or without
 *                  Multi-threading support.
 *
 *                  Note: only checks against defines in the sections "System
 *                        support", "PolarSSL modules" and "PolarSSL feature
 *                        support" in config.h
 *
 * \param feature   The string for the define to check (e.g. "POLARSSL_AES_C")
 *
 * \return          0 if the feature is present, -1 if the feature is not
 *                  present and -2 if support for feature checking as a whole
 *                  was not compiled in.
 */
int version_check_feature( const char *feature );

#ifdef __cplusplus
}
#endif

#endif /* POLARSSL_VERSION_C */

#endif /* version.h */
