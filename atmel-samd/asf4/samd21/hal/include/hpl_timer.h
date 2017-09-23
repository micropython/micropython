/**
 * \file
 *
 * \brief Timer related functionality declaration.
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

#ifndef _HPL_TIMER_H_INCLUDED
#define _HPL_TIMER_H_INCLUDED

/**
 * \addtogroup HPL Timer
 *
 * \section hpl_timer_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>
#include <hpl_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Timer device structure
 *
 * The Timer device structure forward declaration.
 */
struct _timer_device;

/**
 * \brief Timer interrupt callbacks
 */
struct _timer_callbacks {
	void (*period_expired)(struct _timer_device *device);
};

/**
 * \brief Timer device structure
 */
struct _timer_device {
	struct _timer_callbacks timer_cb;
	struct _irq_descriptor  irq;
	void *                  hw;
};

/**
 * \brief Timer functions, pointers to low-level functions
 */
struct _timer_hpl_interface {
	int32_t (*init)(struct _timer_device *const device, void *const hw);
	void (*deinit)(struct _timer_device *const device);
	void (*start_timer)(struct _timer_device *const device);
	void (*stop_timer)(struct _timer_device *const device);
	void (*set_timer_period)(struct _timer_device *const device, const uint32_t clock_cycles);
	uint32_t (*get_period)(const struct _timer_device *const device);
	bool (*is_timer_started)(const struct _timer_device *const device);
	void (*set_timer_irq)(struct _timer_device *const device);
};
/**
 * \brief Initialize TCC
 *
 * This function does low level TCC configuration.
 *
 * \param[in] device The pointer to timer device instance
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status.
 */
int32_t _timer_init(struct _timer_device *const device, void *const hw);

/**
 * \brief Deinitialize TCC
 *
 * \param[in] device The pointer to timer device instance
 */
void _timer_deinit(struct _timer_device *const device);

/**
 * \brief Start hardware timer
 *
 * \param[in] device The pointer to timer device instance
 */
void _timer_start(struct _timer_device *const device);

/**
 * \brief Stop hardware timer
 *
 * \param[in] device The pointer to timer device instance
 */
void _timer_stop(struct _timer_device *const device);

/**
 * \brief Set timer period
 *
 * \param[in] device The pointer to timer device instance
 */
void _timer_set_period(struct _timer_device *const device, const uint32_t clock_cycles);

/**
 * \brief Retrieve timer period
 *
 * \param[in] device The pointer to timer device instance
 *
 * \return Timer period
 */
uint32_t _timer_get_period(const struct _timer_device *const device);

/**
 * \brief Check if timer is running
 *
 * \param[in] device The pointer to timer device instance
 *
 * \return Check status.
 * \retval true The given timer is running
 * \retval false The given timer is not running
 */
bool _timer_is_started(const struct _timer_device *const device);

/**
 * \brief Set timer IRQ
 *
 * \param[in] device The pointer to timer device instance
 */
void _timer_set_irq(struct _timer_device *const device);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_TIMER_H_INCLUDED */
