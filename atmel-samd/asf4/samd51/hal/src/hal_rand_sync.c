/**
 * \file
 *
 * \brief Generic Random Number Generator (RNG) functionality declaration.
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

#include <utils.h>
#include "hal_rand_sync.h"

#define HAL_RNG_SYNC_VERSION 0x00000001u

int32_t rand_sync_init(struct rand_sync_desc *const desc, void *const hw)
{
	ASSERT(desc);

	return _rand_sync_init(&desc->dev, hw);
}

void rand_sync_deinit(struct rand_sync_desc *const desc)
{
	ASSERT(desc);
	_rand_sync_deinit(&desc->dev);
}

int32_t rand_sync_enable(struct rand_sync_desc *const desc)
{
	ASSERT(desc);
	return _rand_sync_enable(&desc->dev);
}

void rand_sync_disable(struct rand_sync_desc *const desc)
{
	ASSERT(desc);
	_rand_sync_disable(&desc->dev);
}

int32_t rand_sync_set_seed(struct rand_sync_desc *const desc, const uint32_t seed)
{
	ASSERT(desc);
	return _rand_sync_set_seed(&desc->dev, seed);
}

/**
 * \brief Read data bits
 */
static uint32_t _rand_sync_read_data(const struct _rand_sync_dev *dev, const uint8_t n_bits)
{
	uint8_t r_bits = (dev->n_bits < 1) ? 32 : dev->n_bits;
	if (r_bits < n_bits) {
		uint8_t  i;
		uint32_t d = 0;
		/* Join read bits */
		for (i = 0; i < n_bits; i += r_bits) {
			d |= (uint32_t)(_rand_sync_read_one(dev) << i);
		}
		return d;
	} else {
		return _rand_sync_read_one(dev);
	}
}

uint8_t rand_sync_read8(const struct rand_sync_desc *const desc)
{
	ASSERT(desc);
	return (uint8_t)_rand_sync_read_data(&desc->dev, 8);
}

uint32_t rand_sync_read32(const struct rand_sync_desc *const desc)
{
	ASSERT(desc);
	return (uint32_t)_rand_sync_read_data(&desc->dev, 32);
}

void rand_sync_read_buf8(const struct rand_sync_desc *const desc, uint8_t *buf, uint32_t len)
{
	uint32_t i;
	ASSERT(desc && (buf && len));
	for (i = 0; i < len; i++) {
		buf[i] = (uint8_t)_rand_sync_read_data(&desc->dev, 8);
	}
}

void rand_sync_read_buf32(const struct rand_sync_desc *const desc, uint32_t *buf, uint32_t len)
{
	uint32_t i;
	ASSERT(desc && (buf && len));
	for (i = 0; i < len; i++) {
		buf[i] = (uint32_t)_rand_sync_read_data(&desc->dev, 32);
	}
}

uint32_t rand_sync_get_version(void)
{
	return HAL_RNG_SYNC_VERSION;
}
