/**
 ******************************************************************************
 * @file    dsakeygen.h
 * @authors Matthew McGowan
 * @date    24 February 2015
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef DSAKEYGEN_H
#define	DSAKEYGEN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generates a DER formatted RSA PCKS#1 private key.
 * @param buffer            The buffer to recieve the DER file.
 * @param max_length        The length of the buffer
 * @param f_rng             A random number generator
 * @param p_rng             The argument to the random number generator
 * @return  0 on success. Non zero on failure.
 */
int gen_rsa_key(uint8_t* buffer, size_t max_length, int (*f_rng) (void *), void *p_rng);



#ifdef __cplusplus
}
#endif


#endif	/* DSAKEYGEN_H */

