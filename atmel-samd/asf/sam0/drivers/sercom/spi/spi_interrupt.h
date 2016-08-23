/**
 * \file
 *
 * \brief SAM Serial Peripheral Interface Driver (Callback Mode)
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

#ifndef SPI_INTERRUPT_H_INCLUDED
#define SPI_INTERRUPT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_sercom_spi_group
 *
 * @{
 */

#include "spi.h"

/**
 * \name Callback Management
 * @{
 */

void spi_register_callback(
		struct spi_module *const module,
		spi_callback_t callback_func,
		enum spi_callback callback_type);

void spi_unregister_callback(
		struct spi_module *module,
		enum spi_callback callback_type);

/**
 * \brief Enables an SPI callback of a given type
 *
 * Enables the callback function registered by the \ref spi_register_callback.
 * The callback function will be called from the interrupt handler when the
 * conditions for the callback type are met.
 *
 * \param[in] module         Pointer to SPI software instance struct
 * \param[in] callback_type  Callback type given by an enum
 */
static inline void spi_enable_callback(
		struct spi_module *const module,
		enum spi_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);

	/* Enable callback */
	module->enabled_callback |= (1 << callback_type);
}

/**
 * \brief Disables callback
 *
 * Disables the callback function registered by the \ref spi_register_callback,
 * and the callback will not be called from the interrupt routine.
 *
 * \param[in] module         Pointer to SPI software instance struct
 * \param[in] callback_type  Callback type given by an enum
 */
static inline void spi_disable_callback(
		struct spi_module *const module,
		enum spi_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);

	/* Disable callback */
	module->enabled_callback &= ~(1 << callback_type);
}

/** @} */


/**
 * \name Writing and Reading
 * @{
 */
enum status_code spi_write_buffer_job(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint16_t length);

enum status_code spi_read_buffer_job(
		struct spi_module *const module,
		uint8_t *rx_data,
		uint16_t length,
		uint16_t dummy);

enum status_code spi_transceive_buffer_job(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint8_t *rx_data,
		uint16_t length);

void spi_abort_job(
		struct spi_module *const module);

/**
 * \brief Retrieves the current status of a job.
 *
 * Retrieves the current status of a job that was previously issued.
 *
 * \param[in]  module    Pointer to SPI software instance struct
 *
 * \return Current job status.
 */
static inline enum status_code spi_get_job_status(
		const struct spi_module *const module)
{
	return module->status;
}

/**
 * \brief Retrieves the status of job once it ends.
 *
 * Waits for current job status to become non-busy, then returns its value.
 *
 * \param[in]  module    Pointer to SPI software instance struct
 *
 * \return Current non-busy job status.
 */
static inline enum status_code spi_get_job_status_wait(
		const struct spi_module *const module)
{
	enum status_code status;

	do {
		status = spi_get_job_status(module);
	} while (status == STATUS_BUSY);

	return status;
}

/** @} */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* SPI_INTERRUPT_H_INCLUDED */
