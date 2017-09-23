/**
 * \file
 *
 * \brief DAC functionality implementation.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#include "hal_dac_sync.h"
#include <utils_assert.h>

/**
 * \brief Driver version
 */
#define DRIVER_VERSION 0x00000001u

/**
 * \brief Initialize the DAC HAL instance and hardware.
 */
int32_t dac_sync_init(struct dac_sync_descriptor *const descr, void *const hw)
{
	uint8_t i;
	int32_t rc;

	ASSERT(descr && hw);

	rc = _dac_sync_init(&descr->device, hw);
	if (rc) {
		return rc;
	}

	for (i = 0; i < CHANNEL_NUM; i++) {
		descr->sel_ch[i].buffer = NULL;
		descr->sel_ch[i].length = 0;
	}

	return ERR_NONE;
}

/**
 * \brief Deinitialize the DAC HAL instance and hardware
 */
int32_t dac_sync_deinit(struct dac_sync_descriptor *const descr)
{
	ASSERT(descr);

	_dac_sync_deinit(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Enable DAC channel
 */
int32_t dac_sync_enable_channel(struct dac_sync_descriptor *const descr, const uint8_t ch)
{
	ASSERT(descr && (ch < CHANNEL_NUM));

	_dac_sync_enable_channel(&descr->device, ch);

	return ERR_NONE;
}

/**
 * \brief Disable DAC channel
 */
int32_t dac_sync_disable_channel(struct dac_sync_descriptor *const descr, const uint8_t ch)
{
	ASSERT(descr && (ch < CHANNEL_NUM));

	_dac_sync_disable_channel(&descr->device, ch);

	return ERR_NONE;
}

/**
 * \brief DAC convert digital data to analog output
 */
int32_t dac_sync_write(struct dac_sync_descriptor *descr, const uint8_t ch, uint16_t *buffer, uint32_t length)
{
	ASSERT(descr && (ch < CHANNEL_NUM) && buffer && length);

	/* check whether channel is enable */
	if (!_dac_sync_is_channel_enable(&descr->device, ch)) {
		return ERR_NOT_READY;
	}

	descr->sel_ch[ch].buffer = buffer;
	descr->sel_ch[ch].length = length;

	while (descr->sel_ch[ch].length) {
		_dac_sync_write_data(&descr->device, *(descr->sel_ch[ch].buffer), ch);
		descr->sel_ch[ch].buffer++;
		descr->sel_ch[ch].length--;
	}

	return ERR_NONE;
}

/**
 * \brief Get DAC driver version
 */
uint32_t dac_sync_get_version(void)
{
	return DRIVER_VERSION;
}
