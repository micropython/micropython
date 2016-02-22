/**
 * \file aesni.h
 *
 * \brief AES-NI for hardware AES acceleration on some Intel processors
 *
 *  Copyright (C) 2013, Brainspark B.V.
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
#ifndef POLARSSL_AESNI_H
#define POLARSSL_AESNI_H

#include "aes.h"

#define POLARSSL_AESNI_AES      0x02000000u
#define POLARSSL_AESNI_CLMUL    0x00000002u

#if defined(POLARSSL_HAVE_ASM) && defined(__GNUC__) &&  \
    ( defined(__amd64__) || defined(__x86_64__) )   &&  \
    ! defined(POLARSSL_HAVE_X86_64)
#define POLARSSL_HAVE_X86_64
#endif

#if defined(POLARSSL_HAVE_X86_64)

/**
 * \brief          AES-NI features detection routine
 *
 * \param what     The feature to detect
 *                 (POLARSSL_AESNI_AES or POLARSSL_AESNI_CLMUL)
 *
 * \return         1 if CPU has support for the feature, 0 otherwise
 */
int aesni_supports( unsigned int what );

/**
 * \brief          AES-NI AES-ECB block en(de)cryption
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 on success (cannot fail)
 */
int aesni_crypt_ecb( aes_context *ctx,
                     int mode,
                     const unsigned char input[16],
                     unsigned char output[16] );

/**
 * \brief          GCM multiplication: c = a * b in GF(2^128)
 *
 * \param c        Result
 * \param a        First operand
 * \param b        Second operand
 *
 * \note           Both operands and result are bit strings interpreted as
 *                 elements of GF(2^128) as per the GCM spec.
 */
void aesni_gcm_mult( unsigned char c[16],
                     const unsigned char a[16],
                     const unsigned char b[16] );

/**
 * \brief           Compute decryption round keys from encryption round keys
 *
 * \param invkey    Round keys for the equivalent inverse cipher
 * \param fwdkey    Original round keys (for encryption)
 * \param nr        Number of rounds (that is, number of round keys minus one)
 */
void aesni_inverse_key( unsigned char *invkey,
                        const unsigned char *fwdkey, int nr );

/**
 * \brief           Perform key expansion (for encryption)
 *
 * \param rk        Destination buffer where the round keys are written
 * \param key       Encryption key
 * \param bits      Key size in bits (must be 128, 192 or 256)
 *
 * \return          0 if successful, or POLARSSL_ERR_AES_INVALID_KEY_LENGTH
 */
int aesni_setkey_enc( unsigned char *rk,
                      const unsigned char *key,
                      size_t bits );

#endif /* POLARSSL_HAVE_X86_64 */

#endif /* POLARSSL_AESNI_H */
