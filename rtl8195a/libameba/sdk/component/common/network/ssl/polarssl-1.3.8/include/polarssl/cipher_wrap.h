/**
 * \file cipher_wrap.h
 *
 * \brief Cipher wrappers.
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
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
#ifndef POLARSSL_CIPHER_WRAP_H
#define POLARSSL_CIPHER_WRAP_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif
#include "cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    cipher_type_t type;
    const cipher_info_t *info;
} cipher_definition_t;

extern const cipher_definition_t cipher_definitions[];

extern int supported_ciphers[];

#ifdef __cplusplus
}
#endif

#endif /* POLARSSL_CIPHER_WRAP_H */
