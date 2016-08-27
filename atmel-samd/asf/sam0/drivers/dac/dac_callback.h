/**
 * \file
 *
 * \brief SAM Digital-to-Analog Interrupt Driver
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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
#ifndef DAC_CALLBACK_H_INCLUDED
#define DAC_CALLBACK_H_INCLUDED

#include <compiler.h>
#include "dac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_dac_group
 *
 * @{
 */

/** \name Callback Configuration and Initialization
 * @{
 */
enum status_code dac_chan_write_buffer_job(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		uint16_t *buffer,
		uint32_t buffer_size);

enum status_code dac_chan_write_job(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		uint16_t data);

enum status_code dac_register_callback(
		struct dac_module *const module,
		const enum dac_channel channel,
		const dac_callback_t callback,
		const enum dac_callback type);

enum status_code dac_unregister_callback(
		struct dac_module *const module,
		const enum dac_channel channel,
		const enum dac_callback type);

/** @} */

/** \name Callback Enabling and Disabling (Channel)
 * @{
 */

enum status_code dac_chan_enable_callback(
		struct dac_module *const module,
		const enum dac_channel channel,
		const enum dac_callback type);

enum status_code dac_chan_disable_callback(
		struct dac_module *const module,
		const enum dac_channel channel,
		const enum dac_callback type);

enum status_code dac_chan_get_job_status(
		struct dac_module *module_inst,
		const enum dac_channel channel);

void dac_chan_abort_job(
		struct dac_module *module_inst,
		const enum dac_channel channel);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
