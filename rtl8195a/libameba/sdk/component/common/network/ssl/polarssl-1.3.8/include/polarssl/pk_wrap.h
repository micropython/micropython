/**
 * \file pk.h
 *
 * \brief Public Key abstraction layer: wrapper functions
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

#ifndef POLARSSL_PK_WRAP_H
#define POLARSSL_PK_WRAP_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include "pk.h"

/* Container for RSA-alt */
typedef struct
{
    void *key;
    pk_rsa_alt_decrypt_func decrypt_func;
    pk_rsa_alt_sign_func sign_func;
    pk_rsa_alt_key_len_func key_len_func;
} rsa_alt_context;

#if defined(POLARSSL_RSA_C)
extern const pk_info_t rsa_info;
#endif

#if defined(POLARSSL_ECP_C)
extern const pk_info_t eckey_info;
extern const pk_info_t eckeydh_info;
#endif

#if defined(POLARSSL_ECDSA_C)
extern const pk_info_t ecdsa_info;
#endif

extern const pk_info_t rsa_alt_info;

#endif /* POLARSSL_PK_WRAP_H */
