/**
 * \file
 *
 * \brief SAM I2S - Inter-IC Sound Controller
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef I2S_CALLBACK_H_INCLUDED
#define I2S_CALLBACK_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_i2s_group
 *
 * @{
 */

#include <i2s.h>

/**
 * Enum for the possible types of I<SUP>2</SUP>S asynchronous jobs that may be issued to
 * the driver.
 */
enum i2s_job_type {
	/** Asynchronous I<SUP>2</SUP>S write from a user provided buffer */
	I2S_JOB_WRITE_BUFFER,
	/** Asynchronous I<SUP>2</SUP>S read into a user provided buffer */
	I2S_JOB_READ_BUFFER
};

/**
 * \name Callback Management
 * @{
 */

/**
 * \brief Registers a callback for serializer
 *
 * Registers a callback function which is implemented by the user.
 *
 * \note The callback must be enabled by for the interrupt handler to call it
 * when the condition for the callback is met.
 *
 * \param[in] module         Pointer to ADC software instance struct
 * \param[in] serializer     The serializer that generates callback
 * \param[in] callback_func  Pointer to callback function
 * \param[in] callback_type  Callback type given by an enum
 *
 */
static inline void i2s_serializer_register_callback(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const i2s_serializer_callback_t callback_func,
		const enum i2s_serializer_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(serializer < I2S_SERIALIZER_N);

	module_inst->serializer[serializer].callback[callback_type] = callback_func;
	module_inst->serializer[serializer].registered_callback_mask |=
			(1u << callback_type);
}

/**
 * \brief Unregisters a callback for serializer
 *
 * Unregisters a callback function which is implemented by the user.
 *
 * \param[in] module         Pointer to ADC software instance struct
 * \param[in] serializer     The serializer that generates callback
 * \param[in] callback_type  Callback type given by an enum
 *
 */
static inline void i2s_serializer_unregister_callback(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const enum i2s_serializer_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(serializer < I2S_SERIALIZER_N);

	module_inst->serializer[serializer].callback[callback_type] = NULL;
	module_inst->serializer[serializer].registered_callback_mask &=
			~(1u << callback_type);
}

/**
 * \brief Enables callback for serializer
 *
 * Enables the callback function registered by \ref
 * i2s_serializer_register_callback. The callback function will be called from
 * the interrupt handler when the conditions for the callback type are met.
 *
 * \param[in] module         Pointer to ADC software instance struct
 * \param[in] serializer     The serializer that generates callback
 * \param[in] callback_type  Callback type given by an enum
 *
 */
static inline void i2s_serializer_enable_callback(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const enum i2s_serializer_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(serializer < I2S_SERIALIZER_N);

	module_inst->serializer[serializer].enabled_callback_mask |=
			(1u << callback_type);
	if (I2S_SERIALIZER_CALLBACK_OVER_UNDER_RUN != callback_type) {
		return;
	}
	module_inst->hw->INTENSET.reg =
		(module_inst->serializer[serializer].mode == I2S_SERIALIZER_TRANSMIT) ?
			(I2S_INTFLAG_TXUR0 << serializer) :
			(I2S_INTFLAG_RXOR0 << serializer);
}

/**
 * \brief Disables callback for Serializer
 *
 * Disables the callback function registered by the \ref
 * i2s_serializer_register_callback.
 *
 * \param[in] module         Pointer to ADC software instance struct
 * \param[in] serializer     The serializer that generates callback
 * \param[in] callback_type  Callback type given by an enum
 *
 */
static inline void i2s_serializer_disable_callback(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const enum i2s_serializer_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(serializer < I2S_SERIALIZER_N);

	module_inst->serializer[serializer].enabled_callback_mask &=
			~(1u << callback_type);
	if (I2S_SERIALIZER_CALLBACK_OVER_UNDER_RUN != callback_type) {
		return;
	}
	module_inst->hw->INTENCLR.reg =
		(module_inst->serializer[serializer].mode == I2S_SERIALIZER_TRANSMIT) ?
			(I2S_INTFLAG_TXUR0 << serializer) :
			(I2S_INTFLAG_RXOR0 << serializer);
}

/** @} */

/**
 * \name Job Management
 *
 * @{
 */

enum status_code i2s_serializer_write_buffer_job(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const void *buffer,
		const uint32_t size);

enum status_code i2s_serializer_read_buffer_job(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		void *buffer,
		const uint32_t size);

void i2s_serializer_abort_job(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const enum i2s_job_type job_type);

enum status_code i2s_serializer_get_job_status(
		const struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const enum i2s_job_type job_type);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef I2S_CALLBACK_H_INCLUDED */
