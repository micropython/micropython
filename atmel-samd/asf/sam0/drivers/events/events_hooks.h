/**
 * \file
 *
 * \brief SAM Event System Driver
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

#include "events.h"

#ifndef _EVENTS_HOOKS_H_INCLUDED_
#define _EVENTS_HOOKS_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_events_group
 * @{
 *
 */

/**
 * \brief Interrupt source enumerator.
 *
 * Interrupt source selector definitions.
 *
 */
enum events_interrupt_source {
	/** Overrun in event channel detected interrupt */
	EVENTS_INTERRUPT_OVERRUN,
	/** Event signal propagation in event channel detected interrupt */
	EVENTS_INTERRUPT_DETECT,
};

/**
 * \brief Initializes an interrupt hook for insertion in the event interrupt hook queue.
 *
 * Initializes a hook structure so it is ready for insertion in the interrupt hook queue.
 *
 * \param[out] hook Pointer to an \ref events_hook struct instance
 * \param[in]  hook_func Pointer to a function containing the interrupt hook code
 *
 * \return Status of the hook creation procedure.
 * \retval STATUS_OK Creation and initialization of interrupt hook went successful
 *
 */
enum status_code events_create_hook(struct events_hook *hook, events_interrupt_hook hook_func);

/**
 * \brief Insert hook into the event drivers interrupt hook queue.
 *
 * Inserts a hook into the event drivers interrupt hook queue.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] hook     Pointer to an \ref events_hook struct instance
 *
 * \return Status of the insertion procedure.
 * \retval STATUS_OK Insertion of hook went successful
 *
 */
enum status_code events_add_hook(struct events_resource *resource, struct events_hook *hook);

/**
 * \brief Remove hook from the event drivers interrupt hook queue.
 *
 * Removes a hook from the event drivers interrupt hook queue.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] hook     Pointer to an \ref events_hook struct instance
 *
 * \return Status of the removal procedure.
 * \retval STATUS_OK Removal of hook went successful
 * \retval STATUS_ERR_NO_MEMORY There are no hooks instances in the event driver interrupt hook list
 * \retval STATUS_ERR_NOT_FOUND Interrupt hook not found in the event drivers interrupt hook list
 *
 */
enum status_code events_del_hook(struct events_resource *resource, struct events_hook *hook);

/**
 * \brief Enable interrupt source.
 *
 * Enable an interrupt source so can trigger execution of an interrupt hook.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] source   One of the members in the \ref events_interrupt_source enumerator
 *
 * \return Status of the interrupt source enable procedure.
 * \retval STATUS_OK              Enabling of the interrupt source was successful
 * \retval STATUS_ERR_INVALID_ARG Interrupt source does not exist
 *
 */
enum status_code events_enable_interrupt_source(struct events_resource *resource, enum events_interrupt_source source);

/**
 * \brief Disable interrupt source.
 *
 * Disable an interrupt source so can trigger execution of an interrupt hook.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] source   One of the members in the \ref events_interrupt_source enumerator
 *
 * \return Status of the interrupt source enable procedure.
 * \retval STATUS_OK              Enabling of the interrupt source went successful
 * \retval STATUS_ERR_INVALID_ARG Interrupt source does not exist
 *
 */
enum status_code events_disable_interrupt_source(struct events_resource *resource, enum events_interrupt_source source);

/**
 * \brief Check if interrupt source is set.
 *
 * Check if an interrupt source is set and should be processed.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] source   One of the members in the \ref events_interrupt_source enumerator
 *
 * \return Status of the interrupt source.
 * \retval true  Interrupt source is set
 * \retval false Interrupt source is not set
 *
 */
bool events_is_interrupt_set(struct events_resource *resource, enum events_interrupt_source source);

/**
 * \brief Acknowledge an interrupt source.
 *
 * Acknowledge an interrupt source so the interrupt state is cleared in hardware.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] source   One of the members in the \ref events_interrupt_source enumerator
 *
 * \return Status of the interrupt source.
 * \retval STATUS_OK Interrupt source was acknowledged successfully
 *
 */
enum status_code events_ack_interrupt(struct events_resource *resource, enum events_interrupt_source source);

#ifdef __cplusplus
}
#endif

#endif
