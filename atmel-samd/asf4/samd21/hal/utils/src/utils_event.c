/**
 * \file
 *
 * \brief Events implementation.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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
 * HOWEVER CAUSEsD AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <utils_event.h>
#include <utils_assert.h>
#include <string.h>

#define EVENT_WORD_BITS (sizeof(event_word_t) * 8)

static struct list_descriptor events;
static uint8_t                subscribed[EVENT_MASK_SIZE];

int32_t event_subscribe(struct event *const event, const event_id_t id, event_cb_t cb)
{
	/* get byte and bit number of the given event in the event mask */
	const uint8_t position = id >> 3;
	const uint8_t mask     = 1 << (id & 0x7);

	ASSERT(event && cb && (id < EVENT_MAX_AMOUNT));

	if (event->mask[position] & mask) {
		return ERR_NO_CHANGE; /* Already subscribed */
	}

	if (!is_list_element(&events, event)) {
		memset(event->mask, 0, EVENT_MASK_SIZE);
		list_insert_as_head(&events, event);
	}
	event->cb = cb;
	event->mask[position] |= mask;

	subscribed[position] |= mask;

	return ERR_NONE;
}

int32_t event_unsubscribe(struct event *const event, const event_id_t id)
{
	/* get byte and bit number of the given event in the event mask */
	const uint8_t       position = id >> 3;
	const uint8_t       mask     = 1 << (id & 0x7);
	const struct event *current;
	uint8_t             i;

	ASSERT(event && (id < EVENT_MAX_AMOUNT));

	if (!(event->mask[position] & mask)) {
		return ERR_NO_CHANGE; /* Already unsubscribed */
	}

	event->mask[position] &= ~mask;

	/* Check if there are more subscribers */
	for ((current = (const struct event *)list_get_head(&events)); current;
	     current = (const struct event *)list_get_next_element(current)) {
		if (current->mask[position] & mask) {
			break;
		}
	}
	if (!current) {
		subscribed[position] &= ~mask;
	}

	/* Remove event from the list. Can be unsave, document it! */
	for (i = 0; i < ARRAY_SIZE(event->mask); i++) {
		if (event->mask[i]) {
			return ERR_NONE;
		}
	}
	list_delete_element(&events, event);

	return ERR_NONE;
}

void event_post(const event_id_t id, const event_data_t data)
{
	/* get byte and bit number of the given event in the event mask */
	const uint8_t       position = id >> 3;
	const uint8_t       mask     = 1 << (id & 0x7);
	const struct event *current;

	ASSERT((id < EVENT_MAX_AMOUNT));

	if (!(subscribed[position] & mask)) {
		return; /* No subscribers */
	}

	/* Find all subscribers */
	for ((current = (const struct event *)list_get_head(&events)); current;
	     current = (const struct event *)list_get_next_element(current)) {
		if (current->mask[position] & mask) {
			current->cb(id, data);
		}
	}
}
