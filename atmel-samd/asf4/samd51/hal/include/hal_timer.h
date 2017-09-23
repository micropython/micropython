/**
 * \file
 *
 * \brief Timer task functionality declaration.
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

#ifndef _HAL_TIMER_H_INCLUDED
#define _HAL_TIMER_H_INCLUDED

#include <utils_list.h>
#include <hpl_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_timer
 *
 * @{
 */

/**
 * \brief Timer mode type
 */
enum timer_task_mode { TIMER_TASK_ONE_SHOT, TIMER_TASK_REPEAT };

/**
 * \brief Timer task descriptor
 *
 * The timer task descriptor forward declaration.
 */
struct timer_task;

/**
 * \brief Timer task callback function type
 */
typedef void (*timer_cb_t)(const struct timer_task *const timer_task);

/**
 * \brief Timer task structure
 */
struct timer_task {
	struct list_element elem;       /*! List element. */
	uint32_t            time_label; /*! Absolute timer start time. */

	uint32_t             interval; /*! Number of timer ticks before calling the task. */
	timer_cb_t           cb;       /*! Function pointer to the task. */
	enum timer_task_mode mode;     /*! Task mode: one shot or repeat. */
};

/**
 * \brief Timer structure
 */
struct timer_descriptor {
	struct _timer_device   device;
	uint32_t               time;
	struct list_descriptor tasks; /*! Timer tasks list. */
	volatile uint8_t       flags;
};

/**
 * \brief Initialize timer
 *
 * This function initializes the given timer.
 * It checks if the given hardware is not initialized and if the given hardware
 * is permitted to be initialized.
 *
 * \param[out] descr A timer descriptor to initialize
 * \param[in] hw The pointer to the hardware instance
 * \param[in] func The pointer to a set of function pointers
 *
 * \return Initialization status.
 */
int32_t timer_init(struct timer_descriptor *const descr, void *const hw, struct _timer_hpl_interface *const func);

/**
 * \brief Deinitialize timer
 *
 * This function deinitializes the given timer.
 * It checks if the given hardware is initialized and if the given hardware is
 * permitted to be deinitialized.
 *
 * \param[in] descr A timer descriptor to deinitialize
 *
 * \return De-initialization status.
 */
int32_t timer_deinit(struct timer_descriptor *const descr);

/**
 * \brief Start timer
 *
 * This function starts the given timer.
 * It checks if the given hardware is initialized.
 *
 * \param[in] descr The timer descriptor of a timer to start
 *
 * \return Timer starting status.
 */
int32_t timer_start(struct timer_descriptor *const descr);

/**
 * \brief Stop timer
 *
 * This function stops the given timer.
 * It checks if the given hardware is initialized.
 *
 * \param[in] descr The timer descriptor of a timer to stop
 *
 * \return Timer stopping status.
 */
int32_t timer_stop(struct timer_descriptor *const descr);

/**
 * \brief Set amount of clock cycles per timer tick
 *
 * This function sets the amount of clock cycles per timer tick for the given timer.
 * It checks if the given hardware is initialized.
 *
 * \param[in] descr The timer descriptor of a timer to stop
 * \param[in] clock_cycles The amount of clock cycles per tick to set
 *
 * \return Setting clock cycles amount status.
 */
int32_t timer_set_clock_cycles_per_tick(struct timer_descriptor *const descr, const uint32_t clock_cycles);

/**
 * \brief Retrieve the amount of clock cycles in a tick
 *
 * This function retrieves how many clock cycles there are in a single timer tick.
 * It checks if the given hardware is initialized.
 *
 * \param[in]  descr The timer descriptor of a timer to convert ticks to
 * clock cycles
 * \param[out] cycles The amount of clock cycles
 *
 * \return The status of clock cycles retrieving.
 */
int32_t timer_get_clock_cycles_in_tick(const struct timer_descriptor *const descr, uint32_t *const cycles);

/**
 * \brief Add timer task
 *
 * This function adds the given timer task to the given timer.
 * It checks if the given hardware is initialized.
 *
 * \param[in] descr The timer descriptor of a timer to add task to
 * \param[in] task A task to add
 *
 * \return Timer's task adding status.
 */
int32_t timer_add_task(struct timer_descriptor *const descr, struct timer_task *const task);

/**
 * \brief Remove timer task
 *
 * This function removes the given timer task from the given timer.
 * It checks if the given hardware is initialized.
 *
 * \param[in] descr The timer descriptor of a timer to remove task from
 * \param[in] task A task to remove
 *
 * \return Timer's task removing status.
 */
int32_t timer_remove_task(struct timer_descriptor *const descr, const struct timer_task *const task);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t timer_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _HAL_TIMER_H_INCLUDED */
