/**
 * \file
 *
 * \brief SAM Peripheral Digital-to-Analog Converter Driver
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef DAC_H_INCLUDED
#define DAC_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <clock.h>
#include <gclk.h>

/**
 * \addtogroup asfdoc_sam0_dac_group
 *
 * @{
 */

/**
 * Define DAC features set according to different device families.
 * @{
 */
#if (SAMD21 || SAMD10 || SAMD11 || SAMDA1)
#  define FEATURE_DAC_DATABUF_WRITE_PROTECTION
#endif
/**@}*/

#ifndef DAC_TIMEOUT
#  define DAC_TIMEOUT 0xFFFF
#endif

#if DAC_CALLBACK_MODE == true
#  include <system_interrupt.h>

/** Forward definition of the device instance. */
struct dac_module;

#if !defined(__DOXYGEN__)
extern struct dac_module *_dac_instances[DAC_INST_NUM];
#endif

/** Type definition for a DAC module callback function. */
typedef void (*dac_callback_t)(uint8_t channel);

/** Enum for the possible callback types for the DAC module. */
enum dac_callback {
	/** Callback type for when a DAC channel data empty condition occurs
	 *  (requires event triggered mode) */
	DAC_CALLBACK_DATA_EMPTY,

	/** Callback type for when a DAC channel data underrun condition occurs
	 *  (requires event triggered mode) */
	DAC_CALLBACK_DATA_UNDERRUN,

	/** Callback type for when a DAC channel write buffer job complete (requires
	 *  event triggered mode) */
	DAC_CALLBACK_TRANSFER_COMPLETE,
#if !defined(__DOXYGEN__)
	DAC_CALLBACK_N,
#endif
};

#endif

#include <dac_feature.h>

/**
 * \name Configuration and Initialization
 * @{
 */

bool dac_is_syncing(
		struct dac_module *const dev_inst);

void dac_get_config_defaults(
		struct dac_config *const config);

enum status_code dac_init(
		struct dac_module *const dev_inst,
		Dac *const module,
		struct dac_config *const config);

void dac_reset(
		struct dac_module *const dev_inst);

void dac_enable(
		struct dac_module *const dev_inst);

void dac_disable(
		struct dac_module *const dev_inst);

void dac_enable_events(
		struct dac_module *const module_inst,
		struct dac_events *const events);

void dac_disable_events(
		struct dac_module *const module_inst,
		struct dac_events *const events);

/** @} */

/**
 * \name Configuration and Initialization (Channel)
 * @{
 */

void dac_chan_get_config_defaults(
		struct dac_chan_config *const config);

void dac_chan_set_config(
		struct dac_module *const dev_inst,
		const enum dac_channel channel,
		struct dac_chan_config *const config);

void dac_chan_enable(
		struct dac_module *const dev_inst,
		enum dac_channel channel);

void dac_chan_disable(
		struct dac_module *const dev_inst,
		enum dac_channel channel);

/** @} */

/**
 * \name Channel Data Management
 * @{
 */

enum status_code dac_chan_write(
		struct dac_module *const dev_inst,
		enum dac_channel channel,
		const uint16_t data);

enum status_code dac_chan_write_buffer_wait(
		struct dac_module *const module_inst,
		enum dac_channel channel,
		uint16_t *buffer,
		uint32_t length);

/** @} */

/**
 * \name Status Management
 * @{
 */
uint32_t dac_get_status(
		struct dac_module *const module_inst);
void dac_clear_status(
		struct dac_module *const module_inst,
		uint32_t status_flags);

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */


#endif /* DAC_H_INCLUDED */
