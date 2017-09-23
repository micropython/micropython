/**
 * \file
 *
 * \brief ADC functionality implementation.
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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

/**
 * \brief Indicates HAL being compiled. Must be defined before including.
 */
#define _COMPILING_HAL

#include "hal_adc_sync.h"
#include <utils_assert.h>

/**
 * \brief Driver version
 */
#define DRIVER_VERSION 0x00000001u

/**
 * \brief Maximum amount of ADC interface instances
 */
#define MAX_ADC_AMOUNT ADC_INST_NUM

/**
 * \brief Initialize ADC
 */
int32_t adc_sync_init(struct adc_sync_descriptor *const descr, void *const hw, void *const func)
{
	ASSERT(descr && hw);

	return _adc_sync_init(&descr->device, hw);
}

/**
 * \brief Deinitialize ADC
 */
int32_t adc_sync_deinit(struct adc_sync_descriptor *const descr)
{
	ASSERT(descr);
	_adc_sync_deinit(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Enable ADC
 */
int32_t adc_sync_enable_channel(struct adc_sync_descriptor *const descr, const uint8_t channel)
{
	ASSERT(descr);
	_adc_sync_enable_channel(&descr->device, channel);

	return ERR_NONE;
}

/**
 * \brief Disable ADC
 */
int32_t adc_sync_disable_channel(struct adc_sync_descriptor *const descr, const uint8_t channel)
{
	ASSERT(descr);
	_adc_sync_disable_channel(&descr->device, channel);
	return ERR_NONE;
}

/*
 * \brief Read data from ADC
 */
int32_t adc_sync_read_channel(struct adc_sync_descriptor *const descr, const uint8_t channel, uint8_t *const buffer,
                              const uint16_t length)
{
	uint8_t  data_size;
	uint16_t offset = 0;

	ASSERT(descr && buffer && length);
	data_size = _adc_sync_get_data_size(&descr->device);
	ASSERT(!(length % data_size));

	do {
		uint16_t result;
		_adc_sync_convert(&descr->device);

		while (!_adc_sync_is_channel_conversion_done(&descr->device, channel))
			;

		result         = _adc_sync_read_channel_data(&descr->device, channel);
		buffer[offset] = result;
		if (1 < data_size) {
			buffer[offset + 1] = result >> 8;
		}
		offset += data_size;
	} while (offset < length);

	return offset;
}

/**
 * \brief Set ADC reference source
 */
int32_t adc_sync_set_reference(struct adc_sync_descriptor *const descr, const adc_reference_t reference)
{
	ASSERT(descr);
	_adc_sync_set_reference_source(&descr->device, reference);

	return ERR_NONE;
}

/**
 * \brief Set ADC resolution
 */
int32_t adc_sync_set_resolution(struct adc_sync_descriptor *const descr, const adc_resolution_t resolution)
{
	ASSERT(descr);
	_adc_sync_set_resolution(&descr->device, resolution);

	return ERR_NONE;
}

/**
 * \brief Set ADC input source of a channel
 */
int32_t adc_sync_set_inputs(struct adc_sync_descriptor *const descr, const adc_pos_input_t pos_input,
                            const adc_neg_input_t neg_input, const uint8_t channel)
{
	ASSERT(descr);
	_adc_sync_set_inputs(&descr->device, pos_input, neg_input, channel);

	return ERR_NONE;
}

/**
 * \brief Set ADC thresholds
 */
int32_t adc_sync_set_thresholds(struct adc_sync_descriptor *const descr, const adc_threshold_t low_threshold,
                                const adc_threshold_t up_threshold)
{
	ASSERT(descr);
	_adc_sync_set_thresholds(&descr->device, low_threshold, up_threshold);

	return ERR_NONE;
}

/**
 * \brief Set ADC gain
 */
int32_t adc_sync_set_channel_gain(struct adc_sync_descriptor *const descr, const uint8_t channel, const adc_gain_t gain)
{
	ASSERT(descr);
	_adc_sync_set_channel_gain(&descr->device, channel, gain);

	return ERR_NONE;
}

/**
 * \brief Set ADC conversion mode
 */
int32_t adc_sync_set_conversion_mode(struct adc_sync_descriptor *const descr, const enum adc_conversion_mode mode)
{
	ASSERT(descr);
	_adc_sync_set_conversion_mode(&descr->device, mode);

	return ERR_NONE;
}

/**
 * \brief Set ADC differential mode
 */
int32_t adc_sync_set_channel_differential_mode(struct adc_sync_descriptor *const descr, const uint8_t channel,
                                               const enum adc_differential_mode mode)
{
	ASSERT(descr);
	_adc_sync_set_channel_differential_mode(&descr->device, channel, mode);

	return ERR_NONE;
}

/**
 * \brief Set ADC window mode
 */
int32_t adc_sync_set_window_mode(struct adc_sync_descriptor *const descr, const adc_window_mode_t mode)
{
	ASSERT(descr);
	_adc_sync_set_window_mode(&descr->device, mode);

	return ERR_NONE;
}

/**
 * \brief Retrieve threshold state
 */
int32_t adc_sync_get_threshold_state(const struct adc_sync_descriptor *const descr, adc_threshold_status_t *const state)
{
	ASSERT(descr && state);
	_adc_sync_get_threshold_state(&descr->device, state);

	return ERR_NONE;
}

/**
 * \brief Check if conversion is complete
 */
int32_t adc_sync_is_channel_conversion_complete(const struct adc_sync_descriptor *const descr, const uint8_t channel)
{
	ASSERT(descr);
	return _adc_sync_is_channel_conversion_done(&descr->device, channel);
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t adc_sync_get_version(void)
{
	return DRIVER_VERSION;
}

//@}
