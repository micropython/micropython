/*
 * \file
 *
 * \brief SAM Event System Controller Driver
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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
#include "events_hooks.h"
#include "system_interrupt.h"

#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
#  define _EVENTS_INTFLAGS_DETECT  0x0fff0000
#  define _EVENTS_INTFLAGS_OVERRUN 0x00000fff
#else
#  define _EVENTS_INTFLAGS_DETECT  0x0f00ff00
#  define _EVENTS_INTFLAGS_OVERRUN 0x000f00ff
#endif
#define _EVENTS_INTFLAGS_MASK (_EVENTS_INTFLAGS_DETECT | _EVENTS_INTFLAGS_OVERRUN)

extern struct _events_module _events_inst;

enum status_code events_create_hook(struct events_hook *hook, events_interrupt_hook func)
{
	/* Initialize the hook struct members */
	hook->next      = NULL;
	hook->resource  = NULL;
	hook->hook_func = func;

	return STATUS_OK;
}

enum status_code events_add_hook(struct events_resource *resource, struct events_hook *hook)
{
	struct events_hook *tmp_hook = NULL;

	/* Associate the hook with the resource */
	hook->resource = resource;

	/* Check if this is the first hook in the list */
	if (_events_inst.hook_list == NULL) {
		_events_inst.hook_list = hook;
	} else {
		tmp_hook = _events_inst.hook_list;

		/* Find the first free place in the list */
		while (tmp_hook->next != NULL) {
			tmp_hook = tmp_hook->next;
		}

		/* Put the hook into the next free place in the list */
		tmp_hook->next = hook;
	}

	/* Check if interrupts from the EVSYS module is enabled in the interrupt controller */
	if (!system_interrupt_is_enabled(SYSTEM_INTERRUPT_MODULE_EVSYS)) {
		system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_EVSYS);
	}

	return STATUS_OK;
}

enum status_code events_del_hook(struct events_resource *resource, struct events_hook *hook)
{
	struct events_hook *tmp_hook = _events_inst.hook_list;
	struct events_hook *last_hook = NULL;

	if (tmp_hook != NULL) {
		/* Check if the first hook in the list is the one we are looking for */
		if (tmp_hook != hook) {
			/* Don't double check the first hook */
			tmp_hook = tmp_hook->next;

			/* Check if the current hook is the one we are looking for */
			while (tmp_hook != hook) {

				/* If the current hook pointer is NULL the hook is not found in the list */
				if(tmp_hook == NULL) {
					return STATUS_ERR_NOT_FOUND;
				}

				last_hook = tmp_hook;
				tmp_hook = tmp_hook->next;

			}
			/* Remove the current hook from the list */
			last_hook->next = tmp_hook->next;
		} else {
			_events_inst.hook_list = tmp_hook->next;
		}
	} else {
		/* No hooks where found in the list */
		return STATUS_ERR_NO_MEMORY;
	}

	return STATUS_OK;
}

enum status_code events_enable_interrupt_source(struct events_resource *resource, enum events_interrupt_source source)
{
	Assert((source == EVENTS_INTERRUPT_DETECT) || (source == EVENTS_INTERRUPT_OVERRUN));

	if (source == EVENTS_INTERRUPT_DETECT) {
		EVSYS->INTENSET.reg = _events_find_bit_position(resource->channel,
				_EVENTS_START_OFFSET_DETECTION_BIT);
	} else if (source == EVENTS_INTERRUPT_OVERRUN) {
		EVSYS->INTENSET.reg = _events_find_bit_position(resource->channel,
				_EVENTS_START_OFFSET_OVERRUN_BIT);
	} else {
		return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

enum status_code events_disable_interrupt_source(struct events_resource *resource, enum events_interrupt_source source)
{
	Assert((source == EVENTS_INTERRUPT_DETECT) || (source == EVENTS_INTERRUPT_OVERRUN));

	if (source == EVENTS_INTERRUPT_DETECT) {
		EVSYS->INTENCLR.reg = _events_find_bit_position(resource->channel,
				_EVENTS_START_OFFSET_DETECTION_BIT);
	} else if (source == EVENTS_INTERRUPT_OVERRUN) {
		EVSYS->INTENCLR.reg = _events_find_bit_position(resource->channel,
			_EVENTS_START_OFFSET_OVERRUN_BIT);
	} else {
		return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}


bool events_is_interrupt_set(struct events_resource *resource, enum events_interrupt_source source)
{
	Assert((source == EVENTS_INTERRUPT_DETECT) || (source == EVENTS_INTERRUPT_OVERRUN));

	uint32_t bitpos;

	if (source == EVENTS_INTERRUPT_DETECT) {
		bitpos = _events_find_bit_position(resource->channel,
				_EVENTS_START_OFFSET_DETECTION_BIT);
	} else if (source == EVENTS_INTERRUPT_OVERRUN) {
		bitpos = _events_find_bit_position(resource->channel,
				_EVENTS_START_OFFSET_OVERRUN_BIT);
	} else {
		return false;
	}

	return (bool)(_events_inst.interrupt_flag_buffer & bitpos);
}

enum status_code events_ack_interrupt(struct events_resource *resource, enum events_interrupt_source source)
{
	Assert((source == EVENTS_INTERRUPT_DETECT) || (source == EVENTS_INTERRUPT_OVERRUN));

	uint32_t bitpos;

	if (source == EVENTS_INTERRUPT_DETECT) {
		bitpos = _events_find_bit_position(resource->channel,
				_EVENTS_START_OFFSET_DETECTION_BIT);
	} else if (source == EVENTS_INTERRUPT_OVERRUN) {
		bitpos = _events_find_bit_position(resource->channel,
				_EVENTS_START_OFFSET_OVERRUN_BIT);
	} else {
		return STATUS_ERR_INVALID_ARG;
	}

	_events_inst.interrupt_flag_ack_buffer |= bitpos;

	return STATUS_OK;
}

void EVSYS_Handler(void)
{
	struct events_hook *current_hook = _events_inst.hook_list;
	uint32_t flag;

	/* Synch the interrupt flag buffer with the hardware register */
	flag = EVSYS->INTFLAG.reg;
	_events_inst.interrupt_flag_buffer |= flag;
	/* Clear all hardware interrupt flags */
	EVSYS->INTFLAG.reg = _EVENTS_INTFLAGS_MASK;

	/* Traverse the linked list */
	while (current_hook != NULL) {
		current_hook->hook_func(current_hook->resource);
		current_hook = current_hook->next;
	}

	/* Clear acknowledged interrupt sources from the interrupt flag buffer */
	flag = _events_inst.interrupt_flag_ack_buffer;
	_events_inst.interrupt_flag_buffer &= ~flag;
}



