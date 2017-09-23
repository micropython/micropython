/**
 * \file
 *
 * \brief IRQ related functionality declaration.
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

#ifndef _HPL_IRQ_H_INCLUDED
#define _HPL_IRQ_H_INCLUDED

/**
 * \addtogroup HPL IRQ
 *
 * \section hpl_irq_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief IRQ descriptor
 */
struct _irq_descriptor {
	void (*handler)(void *parameter);
	void *parameter;
};

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Retrieve current IRQ number
 *
 * \return The current IRQ number
 */
uint8_t _irq_get_current(void);

/**
 * \brief Disable the given IRQ
 *
 * \param[in] n The number of IRQ to disable
 */
void _irq_disable(uint8_t n);

/**
 * \brief Set the given IRQ
 *
 * \param[in] n The number of IRQ to set
 */
void _irq_set(uint8_t n);

/**
 * \brief Clear the given IRQ
 *
 * \param[in] n The number of IRQ to clear
 */
void _irq_clear(uint8_t n);

/**
 * \brief Enable the given IRQ
 *
 * \param[in] n The number of IRQ to enable
 */
void _irq_enable(uint8_t n);

/**
 * \brief Register IRQ handler
 *
 * \param[in] number The number registered IRQ
 * \param[in] irq The pointer to irq handler to register
 *
 * \return The status of IRQ handler registering
 * \retval -1 Passed parameters were invalid
 * \retval 0 The registering is completed successfully
 */
void _irq_register(const uint8_t number, struct _irq_descriptor *const irq);
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_IRQ_H_INCLUDED */
