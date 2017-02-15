/**
 * \file pkcs12.h
 *
 * \brief PKCS#12 Personal Information Exchange Syntax
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
#ifndef POLARSSL_PKCS12_H
#define POLARSSL_PKCS12_H

#include <string.h>

#include "md.h"
#include "cipher.h"
#include "asn1.h"

#define POLARSSL_ERR_PKCS12_BAD_INPUT_DATA                 -0x1F80  /**< Bad input parameters to function. */
#define POLARSSL_ERR_PKCS12_FEATURE_UNAVAILABLE            -0x1F00  /**< Feature not available, e.g. unsupported encryption scheme. */
#define POLARSSL_ERR_PKCS12_PBE_INVALID_FORMAT             -0x1E80  /**< PBE ASN.1 data not as expected. */
#define POLARSSL_ERR_PKCS12_PASSWORD_MISMATCH              -0x1E00  /**< Given private key password does not allow for correct decryption. */

#define PKCS12_DERIVE_KEY       1   /**< encryption/decryption key */
#define PKCS12_DERIVE_IV        2   /**< initialization vector     */
#define PKCS12_DERIVE_MAC_KEY   3   /**< integrity / MAC key       */

#define PKCS12_PBE_DECRYPT      0
#define PKCS12_PBE_ENCRYPT      1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief            PKCS12 Password Based function (encryption / decryption)
 *                   for pbeWithSHAAnd128BitRC4
 *
 * \param pbe_params an ASN1 buffer containing the pkcs-12PbeParams structure
 * \param mode       either PKCS12_PBE_ENCRYPT or PKCS12_PBE_DECRYPT
 * \param pwd        the password used (may be NULL if no password is used)
 * \param pwdlen     length of the password (may be 0)
 * \param input      the input data
 * \param len        data length
 * \param output     the output buffer
 *
 * \return           0 if successful, or a PolarSSL error code
 */
int pkcs12_pbe_sha1_rc4_128( asn1_buf *pbe_params, int mode,
                             const unsigned char *pwd,  size_t pwdlen,
                             const unsigned char *input, size_t len,
                             unsigned char *output );

/**
 * \brief            PKCS12 Password Based function (encryption / decryption)
 *                   for cipher-based and md-based PBE's
 *
 * \param pbe_params an ASN1 buffer containing the pkcs-12PbeParams structure
 * \param mode       either PKCS12_PBE_ENCRYPT or PKCS12_PBE_DECRYPT
 * \param cipher_type the cipher used
 * \param md_type     the md used
 * \param pwd        the password used (may be NULL if no password is used)
 * \param pwdlen     length of the password (may be 0)
 * \param input      the input data
 * \param len        data length
 * \param output     the output buffer
 *
 * \return           0 if successful, or a PolarSSL error code
 */
int pkcs12_pbe( asn1_buf *pbe_params, int mode,
                cipher_type_t cipher_type, md_type_t md_type,
                const unsigned char *pwd,  size_t pwdlen,
                const unsigned char *input, size_t len,
                unsigned char *output );

/**
 * \brief            The PKCS#12 derivation function uses a password and a salt
 *                   to produce pseudo-random bits for a particular "purpose".
 *
 *                   Depending on the given id, this function can produce an
 *                   encryption/decryption key, an nitialization vector or an
 *                   integrity key.
 *
 * \param data       buffer to store the derived data in
 * \param datalen    length to fill
 * \param pwd        password to use (may be NULL if no password is used)
 * \param pwdlen     length of the password (may be 0)
 * \param salt       salt buffer to use
 * \param saltlen    length of the salt
 * \param md         md type to use during the derivation
 * \param id         id that describes the purpose (can be PKCS12_DERIVE_KEY,
 *                   PKCS12_DERIVE_IV or PKCS12_DERIVE_MAC_KEY)
 * \param iterations number of iterations
 *
 * \return          0 if successful, or a MD, BIGNUM type error.
 */
int pkcs12_derivation( unsigned char *data, size_t datalen,
                       const unsigned char *pwd, size_t pwdlen,
                       const unsigned char *salt, size_t saltlen,
                       md_type_t md, int id, int iterations );

#ifdef __cplusplus
}
#endif

#endif /* pkcs12.h */
