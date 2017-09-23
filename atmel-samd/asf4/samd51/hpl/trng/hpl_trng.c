/**
 * \file
 *
 * \brief True Random Number Generator
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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

#include <err_codes.h>
#include <hpl_rand_sync.h>
#include <hpl_trng_config.h>
#include <utils_assert.h>

static inline int32_t _trng_init(void *hw)
{
	if (hri_trng_get_CTRLA_reg(hw, TRNG_CTRLA_ENABLE)) {
		return ERR_DENIED;
	}
	if (CONF_TRNG_RUNSTDBY) {
		hri_trng_set_CTRLA_RUNSTDBY_bit(hw);
	} else {
		hri_trng_clear_CTRLA_RUNSTDBY_bit(hw);
	}
	if (CONF_TRNG_DATARDYEO) {
		hri_trng_set_EVCTRL_DATARDYEO_bit(hw);
	} else {
		hri_trng_clear_EVCTRL_DATARDYEO_bit(hw);
	}
	return ERR_NONE;
}

int32_t _rand_sync_init(struct _rand_sync_dev *const dev, void *const hw)
{
	int32_t rc;

	ASSERT(dev && hw);

	rc = _trng_init(hw);
	if (rc == ERR_NONE) {
		dev->prvt   = hw;
		dev->n_bits = 32;
	}
	return rc;
}

void _rand_sync_deinit(struct _rand_sync_dev *const dev)
{
	_rand_sync_disable(dev);
}

int32_t _rand_sync_enable(struct _rand_sync_dev *const dev)
{
	ASSERT(dev);
	ASSERT(dev->prvt);

	hri_trng_set_CTRLA_ENABLE_bit(dev->prvt);
	return ERR_NONE;
}

void _rand_sync_disable(struct _rand_sync_dev *const dev)
{
	ASSERT(dev);
	ASSERT(dev->prvt);

	hri_trng_clear_CTRLA_ENABLE_bit(dev->prvt);
}

int32_t _rand_sync_set_seed(struct _rand_sync_dev *const dev, const uint32_t seed)
{
	(void)dev;
	(void)seed;
	return ERR_UNSUPPORTED_OP;
}

uint32_t _rand_sync_read_one(const struct _rand_sync_dev *const dev)
{
	ASSERT(dev);
	ASSERT(dev->prvt);
	ASSERT(hri_trng_get_CTRLA_reg(dev->prvt, TRNG_CTRLA_ENABLE));

	while (!hri_trng_get_INTFLAG_reg(dev->prvt, TRNG_INTFLAG_DATARDY)) {
		/* Wait until data ready. */
	}
	return hri_trng_read_DATA_reg(dev->prvt);
}
