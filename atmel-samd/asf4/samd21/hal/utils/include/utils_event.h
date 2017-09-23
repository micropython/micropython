/**
 * \file
 *
 * \brief Events declaration.
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
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _UTILS_EVENT_H_INCLUDED
#define _UTILS_EVENT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <utils.h>
#include <utils_list.h>
#include <events.h>

/**
 * \brief The maximum amount of events
 */
#define EVENT_MAX_AMOUNT 8

/**
 * \brief The size of event mask used, it is EVENT_MAX_AMOUNT rounded up to the
 *        closest number divisible by 8.
 */
#define EVENT_MASK_SIZE (round_up(EVENT_MAX_AMOUNT, 8))

/**
 * \brief The type of event ID. IDs should start with 0 and be in numerical order.
 */
typedef uint8_t event_id_t;

/**
 * \brief The type of returned parameter. This type is big enough to contain
 *        pointer to data on any platform.
 */
typedef uintptr_t event_data_t;

/**
 * \brief The type of returned parameter. This type is big enough to contain
 *        pointer to data on any platform.
 */
typedef void (*event_cb_t)(event_id_t id, event_data_t data);

/**
 * \brief Event structure
 */
struct event {
	struct list_element elem;                  /*! The pointer to next event */
	uint8_t             mask[EVENT_MASK_SIZE]; /*! Mask of event IDs callback is called for */
	event_cb_t          cb;                    /*! Callback to be called when an event occurs */
};

/**
 * \brief Subscribe to event
 *
 * \param[in] event The pointer to event structure
 * \param[in] id The event ID to subscribe to
 * \param[in] cb The callback function to call when the given event occurs
 *
 * \return The status of subscription
 */
int32_t event_subscribe(struct event *const event, const event_id_t id, event_cb_t cb);

/**
 * \brief Remove event from subscription
 *
 * \param[in] event The pointer to event structure
 * \param[in] id The event ID to remove subscription from
 *
 * \return The status of subscription removing
 */
int32_t event_unsubscribe(struct event *const event, const event_id_t id);

/**
 * \brief Post event
 *
 * \param[in] id The event ID to post
 * \param[in] data The event data to be passed to event subscribers
 */
void event_post(const event_id_t id, const event_data_t data);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_EVENT_H_INCLUDED */
