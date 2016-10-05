/**
 * \file
 *
 * \brief SAM System Interrupt Driver
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
#include "system_interrupt.h"

/**
 * \brief Check if a interrupt line is pending.
 *
 * Checks if the requested interrupt vector is pending.
 *
 * \param[in] vector  Interrupt vector number to check
 *
 * \returns A boolean identifying if the requested interrupt vector is pending.
 *
 * \retval true   Specified interrupt vector is pending
 * \retval false  Specified interrupt vector is not pending
 *
 */
bool system_interrupt_is_pending(
		const enum system_interrupt_vector vector)
{
	bool result;

	if (vector >= _SYSTEM_INTERRUPT_EXTERNAL_VECTOR_START) {
		result = ((NVIC->ISPR[0] & (1 << vector)) != 0);
	} else if (vector == SYSTEM_INTERRUPT_SYSTICK) {
		result = ((SCB->ICSR & SCB_ICSR_PENDSTSET_Msk) != 0);
	} else {
		Assert(false);
		result = false;
	}

	return result;
}

/**
 * \brief Set a interrupt vector as pending.
 *
 * Set the requested interrupt vector as pending (i.e. issues a software
 * interrupt request for the specified vector). The software handler will be
 * handled (if enabled) in a priority order based on vector number and
 * configured priority settings.
 *
 * \param[in] vector  Interrupt vector number which is set as pending
 *
 * \returns Status code identifying if the vector was successfully set as
 *          pending.
 *
 * \retval STATUS_OK           If no error was detected
 * \retval STATUS_INVALID_ARG  If an unsupported interrupt vector number was given
 */
enum status_code system_interrupt_set_pending(
		const enum system_interrupt_vector vector)
{
	enum status_code status = STATUS_OK;

	if (vector >= _SYSTEM_INTERRUPT_EXTERNAL_VECTOR_START) {
		NVIC->ISPR[0] = (1 << vector);
	} else if (vector == SYSTEM_INTERRUPT_NON_MASKABLE) {
		/* Note: Because NMI has highest priority it will be executed
		 * immediately after it has been set pending */
		SCB->ICSR = SCB_ICSR_NMIPENDSET_Msk;
	} else if (vector == SYSTEM_INTERRUPT_SYSTICK) {
		SCB->ICSR = SCB_ICSR_PENDSTSET_Msk;
	} else {
		/* The user want to set something unsupported as pending */
		Assert(false);
		status = STATUS_ERR_INVALID_ARG;
	}

	return status;
}

/**
 * \brief Clear pending interrupt vector.
 *
 * Clear a pending interrupt vector, so the software handler is not executed.
 *
 * \param[in] vector  Interrupt vector number to clear
 *
 * \returns A status code identifying if the interrupt pending state was
 *          successfully cleared.
 *
 * \retval STATUS_OK           If no error was detected
 * \retval STATUS_INVALID_ARG  If an unsupported interrupt vector number was given
 */
enum status_code system_interrupt_clear_pending(
		const enum system_interrupt_vector vector)
{
	enum status_code status = STATUS_OK;

	if (vector >= _SYSTEM_INTERRUPT_EXTERNAL_VECTOR_START) {
		NVIC->ICPR[0] = (1 << vector);
	} else if (vector == SYSTEM_INTERRUPT_NON_MASKABLE) {
		/* Note: Clearing of NMI pending interrupts does not make sense and is
		 * not supported by the device, as it has the highest priority and will
		 * always be executed at the moment it is set */
		return STATUS_ERR_INVALID_ARG;
	} else if (vector == SYSTEM_INTERRUPT_SYSTICK) {
		SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
	} else {
		Assert(false);
		status = STATUS_ERR_INVALID_ARG;
	}

	return status;
}

/**
 * \brief Set interrupt vector priority level.
 *
 * Set the priority level of an external interrupt or exception.
 *
 * \param[in] vector          Interrupt vector to change
 * \param[in] priority_level  New vector priority level to set
 *
 * \returns Status code indicating if the priority level of the interrupt was
 *          successfully set.
 *
 * \retval STATUS_OK           If no error was detected
 * \retval STATUS_INVALID_ARG  If an unsupported interrupt vector number was given
 */
enum status_code system_interrupt_set_priority(
		const enum system_interrupt_vector vector,
		const enum system_interrupt_priority_level priority_level)
{
	enum status_code status = STATUS_OK;

	if (vector >= _SYSTEM_INTERRUPT_EXTERNAL_VECTOR_START) {
		uint8_t register_num = vector / 4;
		uint8_t priority_pos = ((vector % 4) * 8) + (8 - __NVIC_PRIO_BITS);

		NVIC->IP[register_num] =
				(NVIC->IP[register_num] & ~(_SYSTEM_INTERRUPT_PRIORITY_MASK << priority_pos)) |
				(priority_level << priority_pos);

	} else if (vector == SYSTEM_INTERRUPT_SYSTICK) {
		SCB->SHP[1] = (priority_level << _SYSTEM_INTERRUPT_SYSTICK_PRI_POS);
	} else {
		Assert(false);
		status = STATUS_ERR_INVALID_ARG;
	}

	return status;
}

/**
 * \brief Get interrupt vector priority level.
 *
 * Retrieves the priority level of the requested external interrupt or exception.
 *
 * \param[in] vector  Interrupt vector of which the priority level will be read
 *
 * \return Currently configured interrupt priority level of the given interrupt
 *         vector.
 */
enum system_interrupt_priority_level system_interrupt_get_priority(
		const enum system_interrupt_vector vector)
{
	uint8_t register_num = vector / 4;
	uint8_t priority_pos = ((vector % 4) * 8) + (8 - __NVIC_PRIO_BITS);

	enum system_interrupt_priority_level priority = SYSTEM_INTERRUPT_PRIORITY_LEVEL_0;

	if (vector >= 0) {
		priority = (enum system_interrupt_priority_level)
				((NVIC->IP[register_num] >> priority_pos) & _SYSTEM_INTERRUPT_PRIORITY_MASK);
	} else if (vector == SYSTEM_INTERRUPT_SYSTICK) {
		priority = (enum system_interrupt_priority_level)
				((SCB->SHP[1] >> _SYSTEM_INTERRUPT_SYSTICK_PRI_POS) & _SYSTEM_INTERRUPT_PRIORITY_MASK);
	}

	return priority;
}

