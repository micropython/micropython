/**
 * \file
 *
 * \brief SAM SERCOM I2C Slave Interrupt Driver
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

#ifndef I2C_SLAVE_INTERRUPT_H_INCLUDED
#define I2C_SLAVE_INTERRUPT_H_INCLUDED

#include "i2c_slave.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_sercom_i2c_group
 * @{
 *
 */

/**
 * \name Address Match Functionality
 * @{
 */

void i2c_slave_enable_nack_on_address(
		struct i2c_slave_module *const module);
void i2c_slave_disable_nack_on_address(
		struct i2c_slave_module *const module);

/** @} */

/**
 * \name Callbacks
 * @{
 */
#if !defined(__DOXYGEN__)
void _i2c_slave_interrupt_handler(uint8_t instance);
#endif

void i2c_slave_register_callback(
		struct i2c_slave_module *const module,
		i2c_slave_callback_t callback,
		enum i2c_slave_callback callback_type);

void i2c_slave_unregister_callback(
		struct i2c_slave_module *const module,
		enum i2c_slave_callback callback_type);

/**
 * \brief Enables callback
 *
 * Enables the callback specified by the callback_type.
 *
 * \param[in,out]  module         Pointer to the software module struct
 * \param[in]      callback_type  Callback type to enable
 */
static inline void i2c_slave_enable_callback(
		struct i2c_slave_module *const module,
		enum i2c_slave_callback callback_type)
{
	/* Sanity check */
	Assert(module);
	Assert(module->hw);

	/* Mark callback as enabled */
	module->enabled_callback |= (1 << callback_type);

	/* Enable address callback */
	SercomI2cs *const i2c_hw = &(module->hw->I2CS);
	if (callback_type == I2C_SLAVE_CALLBACK_READ_REQUEST ||
			callback_type == I2C_SLAVE_CALLBACK_WRITE_REQUEST) {
		i2c_hw->INTENSET.reg = SERCOM_I2CS_INTFLAG_AMATCH;
	}
}

/**
 * \brief Disables callback
 *
 * Disables the callback specified by the callback_type.
 *
 * \param[in,out]  module         Pointer to the software module struct
 * \param[in]      callback_type  Callback type to disable
 */
static inline void i2c_slave_disable_callback(
		struct i2c_slave_module *const module,
		enum i2c_slave_callback callback_type)
{
	/* Sanity check */
	Assert(module);
	Assert(module->hw);

	/* Mark callback as disabled */
	module->enabled_callback &= ~(1 << callback_type);
	SercomI2cs *const i2c_hw = &(module->hw->I2CS);
	if (callback_type == I2C_SLAVE_CALLBACK_READ_REQUEST ||
			callback_type == I2C_SLAVE_CALLBACK_WRITE_REQUEST ||
			module->status != STATUS_BUSY) {
		i2c_hw->INTENCLR.reg = SERCOM_I2CS_INTFLAG_AMATCH;
	}
}

/** @} */

/**
 * \name Read and Write, Interrupt-Driven
 * @{
 */


enum status_code i2c_slave_read_packet_job(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet);

enum status_code i2c_slave_write_packet_job(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet);

/**
 * \brief Cancels any currently ongoing operation
 *
 * Terminates the running transfer operation.
 *
 * \param[in,out] module  Pointer to software module structure
 */
static inline void i2c_slave_cancel_job(
		struct i2c_slave_module *const module)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);

	/* Set buffer to 0. */
	module->buffer_remaining = 0;
	module->buffer_length = 0;
}

/**
 * \brief Gets status of ongoing job
 *
 * Will return the status of the ongoing job, or the error that occurred
 * in the last transfer operation.
 * The status will be cleared when starting a new job.
 *
 * \param[in,out] module Pointer to software module structure
 *
 * \return                      Status of job.
 * \retval STATUS_OK            No error has occurred
 * \retval STATUS_BUSY          Transfer is in progress
 * \retval STATUS_ERR_IO        A collision, timeout or bus error happened in
 *                              the last transfer
 * \retval STATUS_ERR_TIMEOUT   A timeout occurred
 * \retval STATUS_ERR_OVERFLOW  Data from master overflows receive buffer
 */
static inline enum status_code i2c_slave_get_job_status(
		struct i2c_slave_module *const module)
{
	/* Check sanity. */
	Assert(module);
	Assert(module->hw);

	/* Return current status code. */
	return module->status;
}

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* I2C_SLAVE_INTERRUPT_H_INCLUDED */
