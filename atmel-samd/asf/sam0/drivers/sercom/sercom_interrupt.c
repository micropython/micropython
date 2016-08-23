/**
 * \file
 *
 * \brief SAM Serial Peripheral Interface Driver
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
#include "sercom_interrupt.h"

void *_sercom_instances[SERCOM_INST_NUM];

/** Save status of initialized handlers */
static bool _handler_table_initialized = false;

/** Void pointers for saving device instance structures */
static void (*_sercom_interrupt_handlers[SERCOM_INST_NUM])(const uint8_t instance);

/**
 * \internal
 * Default interrupt handler.
 *
 * \param[in] instance SERCOM instance used.
 */
static void _sercom_default_handler(
		const uint8_t instance)
{
	Assert(false);
}

/**
 * \internal
 * Saves the given callback handler.
 *
 * \param[in]  instance           Instance index.
 * \param[in]  interrupt_handler  Pointer to instance callback handler.
 */
void _sercom_set_handler(
		const uint8_t instance,
		const sercom_handler_t interrupt_handler)
{
	/* Initialize handlers with default handler and device instances with 0 */
	if (_handler_table_initialized == false) {
		for (uint32_t i = 0; i < SERCOM_INST_NUM; i++) {
			_sercom_interrupt_handlers[i] = &_sercom_default_handler;
			_sercom_instances[i] = NULL;
		}

		_handler_table_initialized = true;
	}

	/* Save interrupt handler */
	_sercom_interrupt_handlers[instance] = interrupt_handler;
}


/** \internal
 * Converts a given SERCOM index to its interrupt vector index.
 */
#define _SERCOM_INTERRUPT_VECT_NUM(n, unused) \
		SYSTEM_INTERRUPT_MODULE_SERCOM##n,

/** \internal
 * Generates a SERCOM interrupt handler function for a given SERCOM index.
 */
#define _SERCOM_INTERRUPT_HANDLER(n, unused) \
		void SERCOM##n##_Handler(void) \
		{ \
			_sercom_interrupt_handlers[n](n); \
		}

/**
 * \internal
 * Returns the system interrupt vector.
 *
 * \param[in]  sercom_instance  Instance pointer
 *
 * \return Enum of system interrupt vector
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM0
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM1
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM2
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM3
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM4
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM5
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM6
 * \retval SYSTEM_INTERRUPT_MODULE_SERCOM7
 */
enum system_interrupt_vector _sercom_get_interrupt_vector(
		Sercom *const sercom_instance)
{
	const uint8_t sercom_int_vectors[SERCOM_INST_NUM] =
		{
			MREPEAT(SERCOM_INST_NUM, _SERCOM_INTERRUPT_VECT_NUM, ~)
		};

	/* Retrieve the index of the SERCOM being requested */
	uint8_t instance_index = _sercom_get_sercom_inst_index(sercom_instance);

	/* Get the vector number from the lookup table for the requested SERCOM */
	return (enum system_interrupt_vector)sercom_int_vectors[instance_index];
}

/** Auto-generate a set of interrupt handlers for each SERCOM in the device */
MREPEAT(SERCOM_INST_NUM, _SERCOM_INTERRUPT_HANDLER, ~)
