/**
 * \file
 *
 * \brief Random Number Generator (RAND) related functionality declaration.
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

#ifndef _HPL_RAND_SYNC_H_INCLUDED
#define _HPL_RAND_SYNC_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Random Number Generator polling device. */
struct _rand_sync_dev {
	/** Pointer to private data or hardware base */
	void *prvt;
	/** Number of bits generated for each read operation */
	uint8_t n_bits;
};

/**
 *  \brief Initialize the Random Number Generator Driver
 *  \param[out] dev Pointer to the device instance struct
 *  \param[in, out] hw Pointer to the hardware for device instance
 *  \return Initialization operation result status, 0 for OK.
 */
int32_t _rand_sync_init(struct _rand_sync_dev *const dev, void *const hw);

/**
 *  \brief Deinitialize the Random Number Generator Driver
 *  \param[in, out] dev Pointer to the device instance struct
 */
void _rand_sync_deinit(struct _rand_sync_dev *const dev);

/**
 *  \brief Enable the Random Number Generator Driver
 *  \param[out] dev Pointer to the device instance struct
 *  \return Enable operation result status, 0 for OK.
 */
int32_t _rand_sync_enable(struct _rand_sync_dev *const dev);

/**
 *  \brief Disable the Random Number Generator Driver
 *  \param[out] dev Pointer to the device instance struct
 */
void _rand_sync_disable(struct _rand_sync_dev *const dev);

/**
 *  \brief Set seed for the Random Number Generator Driver
 *  \param[out] dev Pointer to the device instance struct
 *  \param[in] seed The seed to set
 *  \return Operation result
 *  \retval ERR_NONE Operation complete success
 *  \retval ERR_UNSUPPORTED_OP Seed not supported
 */
int32_t _rand_sync_set_seed(struct _rand_sync_dev *const dev, const uint32_t seed);

/**
 *  \brief Polling random number until it's read back
 *  \param[in] dev Pointer to the device instance struct
 *  \return The random number value
 */
uint32_t _rand_sync_read_one(const struct _rand_sync_dev *const dev);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_RAND_SYNC_H_INCLUDED */
