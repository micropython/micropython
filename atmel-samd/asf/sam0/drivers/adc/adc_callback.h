/**
 * \file
 *
 * \brief SAM Peripheral Analog-to-Digital Converter Driver
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

#ifndef ADC_CALLBACK_H_INCLUDED
#define ADC_CALLBACK_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_adc_group
 *
 * @{
 */

#include <adc.h>

/**
 * Enum for the possible types of ADC asynchronous jobs that may be issued to
 * the driver.
 */
enum adc_job_type {
	/** Asynchronous ADC read into a user provided buffer */
	ADC_JOB_READ_BUFFER,
};

/**
 * \name Callback Management
 * @{
 */
void adc_register_callback(
		struct adc_module *const module,
		adc_callback_t callback_func,
		enum adc_callback callback_type);

void adc_unregister_callback(
		struct adc_module *module,
		enum adc_callback callback_type);

/**
 * \brief Enables callback.
 *
 * Enables the callback function registered by \ref
 * adc_register_callback. The callback function will be called from the
 * interrupt handler when the conditions for the callback type are met.
 *
 * \param[in]     module Pointer to ADC software instance struct
 * \param[in]     callback_type Callback type given by an enum
 *
 * \return    Status of the operation.
 * \retval     STATUS_OK              If operation was completed
 * \retval     STATUS_ERR_INVALID     If operation was not completed,
 *                                    due to invalid callback_type
 *
 */
static inline void adc_enable_callback(
		struct adc_module *const module,
		enum adc_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);

	/* Enable callback */
	module->enabled_callback_mask |= (1 << callback_type);

	/* Enable window interrupt if this is a window callback */
	if (callback_type == ADC_CALLBACK_WINDOW) {
		adc_enable_interrupt(module, ADC_INTERRUPT_WINDOW);
	}
	/* Enable overrun interrupt if error callback is registered */
	if (callback_type == ADC_CALLBACK_ERROR) {
		adc_enable_interrupt(module, ADC_INTERRUPT_OVERRUN);
	}
}

/**
 * \brief Disables callback.
 *
 * Disables the callback function registered by the \ref
 * adc_register_callback.
 *
 * \param[in]     module Pointer to ADC software instance struct
 * \param[in]     callback_type Callback type given by an enum
 *
 * \return    Status of the operation.
 * \retval     STATUS_OK              If operation was completed
 * \retval     STATUS_ERR_INVALID     If operation was not completed,
 *                                    due to invalid callback_type
 *
 */
static inline void adc_disable_callback(
		struct adc_module *const module,
		enum adc_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);

	/* Disable callback */
	module->enabled_callback_mask &= ~(1 << callback_type);

	/* Disable window interrupt if this is a window callback */
	if (callback_type == ADC_CALLBACK_WINDOW) {
		adc_disable_interrupt(module, ADC_INTERRUPT_WINDOW);
	}
	/* Disable overrun interrupt if this is the error callback */
	if (callback_type == ADC_CALLBACK_ERROR) {
		adc_disable_interrupt(module, ADC_INTERRUPT_OVERRUN);
	}
}

/** @} */


/**
 * \name Job Management
 * @{
 */
enum status_code adc_read_buffer_job(
		struct adc_module *const module_inst,
		uint16_t *buffer,
		uint16_t samples);

enum status_code adc_get_job_status(
		struct adc_module *module_inst,
		enum adc_job_type type);

void adc_abort_job(
		struct adc_module *module_inst,
		enum adc_job_type type);
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADC_CALLBACK_H_INCLUDED */
