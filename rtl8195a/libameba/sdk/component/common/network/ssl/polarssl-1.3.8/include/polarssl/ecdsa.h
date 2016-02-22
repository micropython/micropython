/**
 * \file ecdsa.h
 *
 * \brief Elliptic curve DSA
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
#ifndef POLARSSL_ECDSA_H
#define POLARSSL_ECDSA_H

#include "ecp.h"

#if defined(POLARSSL_ECDSA_DETERMINISTIC)
#include "md.h"
#endif

/**
 * \brief           ECDSA context structure
 *
 * \note Purposefully begins with the same members as struct ecp_keypair.
 */
typedef struct
{
    ecp_group grp;      /*!<  elliptic curve used           */
    mpi d;              /*!<  secret signature key          */
    ecp_point Q;        /*!<  public signature key          */
    mpi r;              /*!<  first integer from signature  */
    mpi s;              /*!<  second integer from signature */
}
ecdsa_context;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Compute ECDSA signature of a previously hashed message
 *
 * \param grp       ECP group
 * \param r         First output integer
 * \param s         Second output integer
 * \param d         Private signing key
 * \param buf       Message hash
 * \param blen      Length of buf
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful,
 *                  or a POLARSSL_ERR_ECP_XXX or POLARSSL_MPI_XXX error code
 */
int ecdsa_sign( ecp_group *grp, mpi *r, mpi *s,
                const mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

#if defined(POLARSSL_ECDSA_DETERMINISTIC)
/**
 * \brief           Compute ECDSA signature of a previously hashed message
 *                  (deterministic version)
 *
 * \param grp       ECP group
 * \param r         First output integer
 * \param s         Second output integer
 * \param d         Private signing key
 * \param buf       Message hash
 * \param blen      Length of buf
 * \param md_alg    MD algorithm used to hash the message
 *
 * \return          0 if successful,
 *                  or a POLARSSL_ERR_ECP_XXX or POLARSSL_MPI_XXX error code
 */
int ecdsa_sign_det( ecp_group *grp, mpi *r, mpi *s,
                    const mpi *d, const unsigned char *buf, size_t blen,
                    md_type_t md_alg );
#endif /* POLARSSL_ECDSA_DETERMINISTIC */

/**
 * \brief           Verify ECDSA signature of a previously hashed message
 *
 * \param grp       ECP group
 * \param buf       Message hash
 * \param blen      Length of buf
 * \param Q         Public key to use for verification
 * \param r         First integer of the signature
 * \param s         Second integer of the signature
 *
 * \return          0 if successful,
 *                  POLARSSL_ERR_ECP_BAD_INPUT_DATA if signature is invalid
 *                  or a POLARSSL_ERR_ECP_XXX or POLARSSL_MPI_XXX error code
 */
int ecdsa_verify( ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const ecp_point *Q, const mpi *r, const mpi *s);

/**
 * \brief           Compute ECDSA signature and write it to buffer,
 *                  serialized as defined in RFC 4492 page 20.
 *                  (Not thread-safe to use same context in multiple threads)
 *
 * \param ctx       ECDSA context
 * \param hash      Message hash
 * \param hlen      Length of hash
 * \param sig       Buffer that will hold the signature
 * \param slen      Length of the signature written
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \note            The "sig" buffer must be at least as large as twice the
 *                  size of the curve used, plus 7 (eg. 71 bytes if a 256-bit
 *                  curve is used).
 *
 * \return          0 if successful,
 *                  or a POLARSSL_ERR_ECP, POLARSSL_ERR_MPI or
 *                  POLARSSL_ERR_ASN1 error code
 */
int ecdsa_write_signature( ecdsa_context *ctx,
                           const unsigned char *hash, size_t hlen,
                           unsigned char *sig, size_t *slen,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng );

#if defined(POLARSSL_ECDSA_DETERMINISTIC)
/**
 * \brief           Compute ECDSA signature and write it to buffer,
 *                  serialized as defined in RFC 4492 page 20.
 *                  Deterministic version, RFC 6979.
 *                  (Not thread-safe to use same context in multiple threads)
 *
 * \param ctx       ECDSA context
 * \param hash      Message hash
 * \param hlen      Length of hash
 * \param sig       Buffer that will hold the signature
 * \param slen      Length of the signature written
 * \param md_alg    MD algorithm used to hash the message
 *
 * \note            The "sig" buffer must be at least as large as twice the
 *                  size of the curve used, plus 7 (eg. 71 bytes if a 256-bit
 *                  curve is used).
 *
 * \return          0 if successful,
 *                  or a POLARSSL_ERR_ECP, POLARSSL_ERR_MPI or
 *                  POLARSSL_ERR_ASN1 error code
 */
int ecdsa_write_signature_det( ecdsa_context *ctx,
                               const unsigned char *hash, size_t hlen,
                               unsigned char *sig, size_t *slen,
                               md_type_t md_alg );
#endif /* POLARSSL_ECDSA_DETERMINISTIC */

/**
 * \brief           Read and verify an ECDSA signature
 *
 * \param ctx       ECDSA context
 * \param hash      Message hash
 * \param hlen      Size of hash
 * \param sig       Signature to read and verify
 * \param slen      Size of sig
 *
 * \return          0 if successful,
 *                  POLARSSL_ERR_ECP_BAD_INPUT_DATA if signature is invalid,
 *                  POLARSSL_ERR_ECP_SIG_LEN_MISTMATCH if the signature is
 *                  valid but its actual length is less than siglen,
 *                  or a POLARSSL_ERR_ECP or POLARSSL_ERR_MPI error code
 */
int ecdsa_read_signature( ecdsa_context *ctx,
                          const unsigned char *hash, size_t hlen,
                          const unsigned char *sig, size_t slen );

/**
 * \brief           Generate an ECDSA keypair on the given curve
 *
 * \param ctx       ECDSA context in which the keypair should be stored
 * \param gid       Group (elliptic curve) to use. One of the various
 *                  POLARSSL_ECP_DP_XXX macros depending on configuration.
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 on success, or a POLARSSL_ERR_ECP code.
 */
int ecdsa_genkey( ecdsa_context *ctx, ecp_group_id gid,
                  int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Set an ECDSA context from an EC key pair
 *
 * \param ctx       ECDSA context to set
 * \param key       EC key to use
 *
 * \return          0 on success, or a POLARSSL_ERR_ECP code.
 */
int ecdsa_from_keypair( ecdsa_context *ctx, const ecp_keypair *key );

/**
 * \brief           Initialize context
 *
 * \param ctx       Context to initialize
 */
void ecdsa_init( ecdsa_context *ctx );

/**
 * \brief           Free context
 *
 * \param ctx       Context to free
 */
void ecdsa_free( ecdsa_context *ctx );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int ecdsa_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* ecdsa.h */
