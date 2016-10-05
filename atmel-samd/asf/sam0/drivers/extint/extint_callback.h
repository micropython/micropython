/**
 * \file
 *
 * \brief SAM External Interrupt Driver
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
#ifndef EXTINT_CALLBACK_H_INCLUDED
#define EXTINT_CALLBACK_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_extint_group
 *
 * @{
 */

/** \name Callback Configuration and Initialization
 * @{
 */

/** Enum for the possible callback types for the EXTINT module. */
enum extint_callback_type
{
	/** Callback type for when an external interrupt detects the configured
	 *  channel criteria (i.e. edge or level detection)
	 */
	EXTINT_CALLBACK_TYPE_DETECT,
};

enum status_code extint_register_callback(
	const extint_callback_t callback,
	const uint8_t channel,
	const enum extint_callback_type type);

enum status_code extint_unregister_callback(
	const extint_callback_t callback,
	const uint8_t channel,
	const enum extint_callback_type type);

uint8_t extint_get_current_channel(void);

/** @} */

/** \name Callback Enabling and Disabling (Channel)
 * @{
 */

enum status_code extint_chan_enable_callback(
	const uint8_t channel,
	const enum extint_callback_type type);

enum status_code extint_chan_disable_callback(
	const uint8_t channel,
	const enum extint_callback_type type);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
