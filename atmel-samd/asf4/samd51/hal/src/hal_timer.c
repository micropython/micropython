/**
 * \file
 *
 * \brief Timer functionality implementation.
 *
 * Copyright (C) 2014 - 2016 Atmel Corporation. All rights reserved.
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

#include "hal_timer.h"
#include <utils_assert.h>
#include <utils.h>
#include <hal_atomic.h>
#include <hpl_irq.h>

/**
 * \brief Driver version
 */
#define DRIVER_VERSION 0x00000001u

/**
 * \brief Timer flags
 */
#define TIMER_FLAG_QUEUE_IS_TAKEN 1
#define TIMER_FLAG_INTERRUPT_TRIGERRED 2

static void timer_add_timer_task(struct list_descriptor *list, struct timer_task *const new_task, const uint32_t time);
static void timer_process_counted(struct _timer_device *device);

/**
 * \brief Initialize timer
 */
int32_t timer_init(struct timer_descriptor *const descr, void *const hw, struct _timer_hpl_interface *const func)
{
	ASSERT(descr && hw);
	_timer_init(&descr->device, hw);
	descr->time                           = 0;
	descr->device.timer_cb.period_expired = timer_process_counted;

	return ERR_NONE;
}

/**
 * \brief Deinitialize timer
 */
int32_t timer_deinit(struct timer_descriptor *const descr)
{
	ASSERT(descr);
	_timer_deinit(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Start timer
 */
int32_t timer_start(struct timer_descriptor *const descr)
{
	ASSERT(descr);
	if (_timer_is_started(&descr->device)) {
		return ERR_DENIED;
	}
	_timer_start(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Stop timer
 */
int32_t timer_stop(struct timer_descriptor *const descr)
{
	ASSERT(descr);
	if (!_timer_is_started(&descr->device)) {
		return ERR_DENIED;
	}
	_timer_stop(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Set amount of clock cycler per timer tick
 */
int32_t timer_set_clock_cycles_per_tick(struct timer_descriptor *const descr, const uint32_t clock_cycles)
{
	ASSERT(descr);
	_timer_set_period(&descr->device, clock_cycles);

	return ERR_NONE;
}

/**
 * \brief Add timer task
 */
int32_t timer_add_task(struct timer_descriptor *const descr, struct timer_task *const task)
{
	ASSERT(descr && task);

	descr->flags |= TIMER_FLAG_QUEUE_IS_TAKEN;
	if (is_list_element(&descr->tasks, task)) {
		descr->flags &= ~TIMER_FLAG_QUEUE_IS_TAKEN;
		ASSERT(false);
		return ERR_ALREADY_INITIALIZED;
	}
	task->time_label = descr->time;
	timer_add_timer_task(&descr->tasks, task, descr->time);

	descr->flags &= ~TIMER_FLAG_QUEUE_IS_TAKEN;
	if (descr->flags & TIMER_FLAG_INTERRUPT_TRIGERRED) {
		CRITICAL_SECTION_ENTER()
		descr->flags &= ~TIMER_FLAG_INTERRUPT_TRIGERRED;
		_timer_set_irq(&descr->device);
		CRITICAL_SECTION_LEAVE()
	}

	return ERR_NONE;
}

/**
 * \brief Remove timer task
 */
int32_t timer_remove_task(struct timer_descriptor *const descr, const struct timer_task *const task)
{
	ASSERT(descr && task);

	descr->flags |= TIMER_FLAG_QUEUE_IS_TAKEN;
	if (!is_list_element(&descr->tasks, task)) {
		descr->flags &= ~TIMER_FLAG_QUEUE_IS_TAKEN;
		ASSERT(false);
		return ERR_NOT_FOUND;
	}
	list_delete_element(&descr->tasks, task);

	descr->flags &= ~TIMER_FLAG_QUEUE_IS_TAKEN;
	if (descr->flags & TIMER_FLAG_INTERRUPT_TRIGERRED) {
		CRITICAL_SECTION_ENTER()
		descr->flags &= ~TIMER_FLAG_INTERRUPT_TRIGERRED;
		_timer_set_irq(&descr->device);
		CRITICAL_SECTION_LEAVE()
	}

	return ERR_NONE;
}

/**
 * \brief Retrieve the amount of clock cycles in a tick
 */
int32_t timer_get_clock_cycles_in_tick(const struct timer_descriptor *const descr, uint32_t *const cycles)
{
	ASSERT(descr && cycles);
	*cycles = _timer_get_period(&descr->device);
	return ERR_NONE;
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t timer_get_version(void)
{
	return DRIVER_VERSION;
}

/**
 * \internal Insert a timer task into sorted timer's list
 *
 * \param[in] head The pointer to the head of timer task list
 * \param[in] task The pointer to task to add
 * \param[in] time Current timer time
 */
static void timer_add_timer_task(struct list_descriptor *list, struct timer_task *const new_task, const uint32_t time)
{
	struct timer_task *it, *prev = NULL, *head = (struct timer_task *)list_get_head(list);

	if (!head) {
		list_insert_as_head(list, new_task);
		return;
	}

	for (it = head; it; it = (struct timer_task *)list_get_next_element(it)) {
		uint32_t time_left;

		if (it->time_label <= time) {
			time_left = it->interval - (time - it->time_label);
		} else {
			time_left = it->interval - (0xFFFFFFFF - it->time_label) - time;
		}
		if (time_left >= new_task->interval)
			break;
		prev = it;
	}

	if (it == head) {
		list_insert_as_head(list, new_task);
	} else {
		list_insert_after(prev, new_task);
	}
}

/**
 * \internal Process interrupts
 */
static void timer_process_counted(struct _timer_device *device)
{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wcast-align"
	struct timer_descriptor *timer = CONTAINER_OF(device, struct timer_descriptor, device);
	#pragma GCC diagnostic pop
	struct timer_task *      it    = (struct timer_task *)list_get_head(&timer->tasks);
	uint32_t                 time  = ++timer->time;

	if ((timer->flags & TIMER_FLAG_QUEUE_IS_TAKEN) || (timer->flags & TIMER_FLAG_INTERRUPT_TRIGERRED)) {
		timer->flags |= TIMER_FLAG_INTERRUPT_TRIGERRED;
		return;
	}

	while (it && ((time - it->time_label) >= it->interval)) {
		struct timer_task *tmp = it;

		list_remove_head(&timer->tasks);
		if (TIMER_TASK_REPEAT == tmp->mode) {
			tmp->time_label = time;
			timer_add_timer_task(&timer->tasks, tmp, time);
		}
		it = (struct timer_task *)list_get_head(&timer->tasks);

		tmp->cb(tmp);
	}
}
