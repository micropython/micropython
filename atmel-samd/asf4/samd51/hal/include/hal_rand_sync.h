/**
 * \file
 *
 * \brief Generic Random Number Generator (RAND) functionality declaration.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#ifndef _HAL_RAND_SYNC_H_INCLUDED
#define _HAL_RAND_SYNC_H_INCLUDED

#include <hpl_rand_sync.h>

#include <utils_assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_rand_sync
 *
 *@{
 */

/** Random Number Generator polling device. */
struct rand_sync_desc {
	struct _rand_sync_dev dev;
};

/**
 *  \brief Initialize the Random Number Generator Driver
 *  \param[out] desc Pointer to the device descriptor instance struct
 *  \param[in, out] hw Pointer to the hardware for device instance
 *  \return Initialization operation result status, ERR_NONE (0) for OK.
 */
int32_t rand_sync_init(struct rand_sync_desc *const desc, void *const hw);

/**
 *  \brief Deinitialize the Random Number Generator Driver
 *  \param[out] desc Pointer to the device descriptor instance struct
 */
void rand_sync_deinit(struct rand_sync_desc *const desc);

/**
 *  \brief Enable the Random Number Generator Driver
 *  \param[out] desc Pointer to the device descriptor instance struct
 *  \return Enable operation result status, ERR_NONE (0) for OK.
 */
int32_t rand_sync_enable(struct rand_sync_desc *const desc);

/**
 *  \brief Disable the Random Number Generator Driver
 *  \param[out] desc Pointer to the device descriptor instance struct
 */
void rand_sync_disable(struct rand_sync_desc *const desc);

/**
 *  \brief Set seed for the Random Number Generator Driver
 *  \param[out] desc Pointer to the device descriptor instance struct
 */
int32_t rand_sync_set_seed(struct rand_sync_desc *const desc, const uint32_t seed);

/**
 *  \brief Read the 8-bit Random Number
 *  \param[in] desc Pointer to the device descriptor instance struct
 *  \return The random number generated
 */
uint8_t rand_sync_read8(const struct rand_sync_desc *const desc);

/**
 *  \brief Read the 32-bit Random Number
 *  \param[in] desc Pointer to the device descriptor instance struct
 *  \return The random number generated
 */
uint32_t rand_sync_read32(const struct rand_sync_desc *const desc);

/**
 *  \brief Read the 8-bit Random Number Sequence into a buffer
 *  \param[in] desc Pointer to the device descriptor instance struct
 *  \param[out] buf Pointer to the buffer to fill an array of generated numbers
 *  \param[in] len Number of random numbers to read
 */
void rand_sync_read_buf8(const struct rand_sync_desc *const desc, uint8_t *buf, uint32_t len);

/**
 *  \brief Read the 32-bit Random Number Sequence into a buffer
 *  \param[in] desc Pointer to the device descriptor instance struct
 *  \param[out] buf Pointer to the buffer to fill an array of generated numbers
 *  \param[in] len Number of random numbers to read
 */
void rand_sync_read_buf32(const struct rand_sync_desc *const desc, uint32_t *buf, uint32_t len);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t rand_sync_get_version(void);

/* I/O read will be used to get random data. */

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _HAL_RAND_SYNC_H_INCLUDED */
