/**
 * \file
 *
 * \brief Critical sections related functionality declaration.
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

#ifndef _HAL_ATOMIC_H_INCLUDED
#define _HAL_ATOMIC_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_helper_atomic
 *
 *@{
 */

/**
 * \brief Type for the register holding global interrupt enable flag
 */
typedef uint32_t hal_atomic_t;

/**
 * \brief Helper macro for entering critical sections
 *
 * This macro is recommended to be used instead of a direct call
 * hal_enterCritical() function to enter critical
 * sections. No semicolon is required after the macro.
 *
 * \section atomic_usage Usage Example
 * \code
 * CRITICAL_SECTION_ENTER()
 * Critical code
 * CRITICAL_SECTION_LEAVE()
 * \endcode
 */
#define CRITICAL_SECTION_ENTER()                                                                                       \
	{                                                                                                                  \
		volatile hal_atomic_t __atomic;                                                                                \
		atomic_enter_critical(&__atomic);

/**
 * \brief Helper macro for leaving critical sections
 *
 * This macro is recommended to be used instead of a direct call
 * hal_leaveCritical() function to leave critical
 * sections. No semicolon is required after the macro.
 *
 * \section atomic_usage Usage Example
 * \code
 * CRITICAL_SECTION_ENTER()
 * Some critical code
 * CRITICAL_SECTION_LEAVE()
 * \endcode
 */
#define CRITICAL_SECTION_LEAVE()                                                                                       \
	atomic_leave_critical(&__atomic);                                                                                  \
	}

/**
 * \brief Disable interrupts, enter critical section
 *
 * Disables global interrupts. Supports nested critical sections,
 * so that global interrupts are only re-enabled
 * upon leaving the outermost nested critical section.
 *
 * \param[out] atomic The pointer to a variable to store the value of global
 * interrupt enable flag
 */
void atomic_enter_critical(hal_atomic_t volatile *atomic);

/**
 * \brief Exit atomic section
 *
 * Enables global interrupts. Supports nested critical sections,
 * so that global interrupts are only re-enabled
 * upon leaving the outermost nested critical section.
 *
 * \param[in] atomic The pointer to a variable, which stores the latest stored
 * value of the global interrupt enable flag
 */
void atomic_leave_critical(hal_atomic_t volatile *atomic);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t atomic_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _HAL_ATOMIC_H_INCLUDED */
