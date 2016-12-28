/**
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * Generates a DER formatted EC private key.
 * @param buffer            The buffer to receive the DER file.
 * @param max_length        The length of the buffer
 * @param f_rng             A random number generator
 * @param p_rng             The argument to the random number generator
 * @return  0 on success. Non zero on failure.
 */
int gen_ec_key(uint8_t* buffer, size_t max_length, int (*f_rng) (void *, uint8_t* buf, size_t len), void *p_rng);

int extract_public_ec_key(uint8_t* buffer, size_t max_length, const uint8_t* private_key);
int extract_public_ec_key_length(uint8_t* buffer, size_t max_length, const uint8_t* private_key, size_t private_key_len);
size_t determine_der_length(const uint8_t* key, size_t max_len);


#ifdef __cplusplus
}
#endif
