/**
 * \file
 *
 * \brief Special user data area access
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HPL_USER_DATA_H_INCLUDED
#define _HPL_USER_DATA_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Read data from user data area
 *
 * The user data area could be the area that stores user data that is not erased
 * with the flash contents, e.g.,
 * - NVM Software Calibration Area of SAM D/L/C family
 * - User Signature of SAM E/S/V 70
 *
 * \param[in]  base   The base address of the user area
 * \param[in]  offset The byte offset of the data to be read inside the area
 * \param[out] buf    Pointer to buffer to place the read data
 * \param[in]  size   Size of data in number of bytes
 *
 * \return Operation status or bytes read.
 * \retval ERR_NONE Data read successfully
 * \retval ERR_UNSUPPORTED_OP base address not in any supported user area
 * \retval ERR_BAD_ADDRESS offset not in right area
 * \retval ERR_INVALID_ARG offset and size exceeds the right area
 */
int32_t _user_area_read(const void *base, const uint32_t offset, uint8_t *buf, const uint32_t size);

/**
 * \brief Read no more than 32 bits data from user data area
 *
 * When reading bits, the bitfield can cross 32-bis boundaries.
 *
 * \param[in] base       The base address of the user area
 * \param[in] bit_offset Offset in number of bits
 * \param[in] n_bits     Number of bits to read
 * \return data read, assert if anything wrong (address not in user area
 *         offset, size error, etc.).
 */
uint32_t _user_area_read_bits(const void *base, const uint32_t bit_offset, const uint8_t n_bits);

/**
 * \brief Write data to user data area
 *
 * The user data area could be the area that stores user data that is not erased
 * with the flash contents, e.g.,
 * - NVM Software Calibration Area of SAM D/L/C family
 * - User Signature of SAM E/S/V 70
 *
 * When assigned offset and size exceeds the data area, error is reported.
 *
 * \param[out] base  The base address of the user area
 * \param[in]  offset The offset of the data to be written inside the area
 * \param[in]  buf    Pointer to buffer to place the written data
 * \param[in]  size   Size of data in number of bytes
 *
 * \return Operation status or bytes writting.
 * \retval ERR_NONE Data written successfully
 * \retval ERR_UNSUPPORTED_OP base address not in any supported user area
 * \retval ERR_DENIED Security bit is set
 * \retval ERR_BAD_ADDRESS offset not in right area
 * \retval ERR_INVALID_ARG offset and size exceeds the right area
 */
int32_t _user_area_write(void *base, const uint32_t offset, const uint8_t *buf, const uint32_t size);

/**
 * \brief Write no more than 32 bits data to user data area
 *
 * When writting bits, the bitfield can cross 32-bis boundaries.
 *
 * \param[out] base       The base address of the user area
 * \param[in]  bit_offset Offset in number of bits
 * \param[in]  bits       The data content
 * \param[in]  n_bits     Number of bits to write
 * \return Operation result
 * \retval ERR_NONE Data written successfully
 * \retval ERR_UNSUPPORTED_OP base address not in any supported user area
 * \retval ERR_DENIED Security bit is set
 * \retval ERR_BAD_ADDRESS offset not in right area
 * \retval ERR_INVALID_ARG offset and size exceeds the right area
 */
int32_t _user_area_write_bits(void *base, const uint32_t bit_offset, const uint32_t bits, const uint8_t n_bits);

#ifdef __cplusplus
}
#endif

#endif /* _HPL_USER_DATA_H_INCLUDED */
