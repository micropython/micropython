/**
 * \file ecdh.h
 *
 * \brief Elliptic curve Diffie-Hellman
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
#ifndef POLARSSL_ECDH_H
#define POLARSSL_ECDH_H

#include "ecp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * When importing from an EC key, select if it is our key or the peer's key
 */
typedef enum
{
    POLARSSL_ECDH_OURS,
    POLARSSL_ECDH_THEIRS,
} ecdh_side;

/**
 * \brief           ECDH context structure
 */
typedef struct
{
    ecp_group grp;      /*!<  elliptic curve used                           */
    mpi d;              /*!<  our secret value (private key)                */
    ecp_point Q;        /*!<  our public value (public key)                 */
    ecp_point Qp;       /*!<  peer's public value (public key)              */
    mpi z;              /*!<  shared secret                                 */
    int point_format;   /*!<  format for point export in TLS messages       */
    ecp_point Vi;       /*!<  blinding value (for later)                    */
    ecp_point Vf;       /*!<  un-blinding value (for later)                 */
    mpi _d;             /*!<  previous d (for later)                        */
}
ecdh_context;

/**
 * \brief           Generate a public key.
 *                  Raw function that only does the core computation.
 *
 * \param grp       ECP group
 * \param d         Destination MPI (secret exponent, aka private key)
 * \param Q         Destination point (public key)
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful,
 *                  or a POLARSSL_ERR_ECP_XXX or POLARSSL_MPI_XXX error code
 */
int ecdh_gen_public( ecp_group *grp, mpi *d, ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng );

/**
 * \brief           Compute shared secret
 *                  Raw function that only does the core computation.
 *
 * \param grp       ECP group
 * \param z         Destination MPI (shared secret)
 * \param Q         Public key from other party
 * \param d         Our secret exponent (private key)
 * \param f_rng     RNG function (see notes)
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful,
 *                  or a POLARSSL_ERR_ECP_XXX or POLARSSL_MPI_XXX error code
 *
 * \note            If f_rng is not NULL, it is used to implement
 *                  countermeasures against potential elaborate timing
 *                  attacks, see \c ecp_mul() for details.
 */
int ecdh_compute_shared( ecp_group *grp, mpi *z,
                         const ecp_point *Q, const mpi *d,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng );

/**
 * \brief           Initialize context
 *
 * \param ctx       Context to initialize
 */
void ecdh_init( ecdh_context *ctx );

/**
 * \brief           Free context
 *
 * \param ctx       Context to free
 */
void ecdh_free( ecdh_context *ctx );

/**
 * \brief           Generate a public key and a TLS ServerKeyExchange payload.
 *                  (First function used by a TLS server for ECDHE.)
 *
 * \param ctx       ECDH context
 * \param olen      number of chars written
 * \param buf       destination buffer
 * \param blen      length of buffer
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \note            This function assumes that ctx->grp has already been
 *                  properly set (for example using ecp_use_known_dp).
 *
 * \return          0 if successful, or an POLARSSL_ERR_ECP_XXX error code
 */
int ecdh_make_params( ecdh_context *ctx, size_t *olen,
                      unsigned char *buf, size_t blen,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng );

/**
 * \brief           Parse and procress a TLS ServerKeyExhange payload.
 *                  (First function used by a TLS client for ECDHE.)
 *
 * \param ctx       ECDH context
 * \param buf       pointer to start of input buffer
 * \param end       one past end of buffer
 *
 * \return          0 if successful, or an POLARSSL_ERR_ECP_XXX error code
 */
int ecdh_read_params( ecdh_context *ctx,
                      const unsigned char **buf, const unsigned char *end );

/**
 * \brief           Setup an ECDH context from an EC key.
 *                  (Used by clients and servers in place of the
 *                  ServerKeyEchange for static ECDH: import ECDH parameters
 *                  from a certificate's EC key information.)
 *
 * \param ctx       ECDH constext to set
 * \param key       EC key to use
 * \param side      Is it our key (1) or the peer's key (0) ?
 *
 * \return          0 if successful, or an POLARSSL_ERR_ECP_XXX error code
 */
int ecdh_get_params( ecdh_context *ctx, const ecp_keypair *key,
                     ecdh_side side );

/**
 * \brief           Generate a public key and a TLS ClientKeyExchange payload.
 *                  (Second function used by a TLS client for ECDH(E).)
 *
 * \param ctx       ECDH context
 * \param olen      number of bytes actually written
 * \param buf       destination buffer
 * \param blen      size of destination buffer
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful, or an POLARSSL_ERR_ECP_XXX error code
 */
int ecdh_make_public( ecdh_context *ctx, size_t *olen,
                      unsigned char *buf, size_t blen,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng );

/**
 * \brief           Parse and process a TLS ClientKeyExchange payload.
 *                  (Second function used by a TLS server for ECDH(E).)
 *
 * \param ctx       ECDH context
 * \param buf       start of input buffer
 * \param blen      length of input buffer
 *
 * \return          0 if successful, or an POLARSSL_ERR_ECP_XXX error code
 */
int ecdh_read_public( ecdh_context *ctx,
                      const unsigned char *buf, size_t blen );

/**
 * \brief           Derive and export the shared secret.
 *                  (Last function used by both TLS client en servers.)
 *
 * \param ctx       ECDH context
 * \param olen      number of bytes written
 * \param buf       destination buffer
 * \param blen      buffer length
 * \param f_rng     RNG function, see notes for \c ecdh_compute_shared()
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful, or an POLARSSL_ERR_ECP_XXX error code
 */
int ecdh_calc_secret( ecdh_context *ctx, size_t *olen,
                      unsigned char *buf, size_t blen,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int ecdh_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* ecdh.h */
