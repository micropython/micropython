/**
 * \file
 *
 * \brief PWM functionality implementation.
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

#include "hal_pwm.h"
#include <utils_assert.h>
#include <utils.h>

/**
 * \brief Driver version
 */
#define DRIVER_VERSION 0x00000001u

static void pwm_period_expired(struct _pwm_device *device);
static void pwm_detect_fault(struct _pwm_device *device);

/**
 * \brief Initialize pwm
 */
int32_t pwm_init(struct pwm_descriptor *const descr, void *const hw, struct _pwm_hpl_interface *const func)
{
	ASSERT(descr && hw);
	_pwm_init(&descr->device, hw);
	descr->device.callback.pwm_period_cb = pwm_period_expired;
	descr->device.callback.pwm_error_cb  = pwm_detect_fault;
	return ERR_NONE;
}

/**
 * \brief Deinitialize pwm
 */
int32_t pwm_deinit(struct pwm_descriptor *const descr)
{
	ASSERT(descr);
	_pwm_deinit(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Start pwm
 */
int32_t pwm_enable(struct pwm_descriptor *const descr)
{
	ASSERT(descr);
	if (_pwm_is_enabled(&descr->device)) {
		return ERR_DENIED;
	}
	_pwm_enable(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Stop pwm
 */
int32_t pwm_disable(struct pwm_descriptor *const descr)
{
	ASSERT(descr);
	if (!_pwm_is_enabled(&descr->device)) {
		return ERR_DENIED;
	}
	_pwm_disable(&descr->device);

	return ERR_NONE;
}

/**
 * \brief Register PWM callback
 */
int32_t pwm_register_callback(struct pwm_descriptor *const descr, enum pwm_callback_type type, pwm_cb_t cb)
{
	switch (type) {
	case PWM_PERIOD_CB:
		descr->pwm_cb.period = cb;
		break;

	case PWM_ERROR_CB:
		descr->pwm_cb.error = cb;
		break;

	default:
		return ERR_INVALID_ARG;
	}
	ASSERT(descr);
	_pwm_set_irq_state(&descr->device, (enum _pwm_callback_type)type, NULL != cb);
	return ERR_NONE;
}

/**
 * \brief Change PWM parameter
 */
int32_t pwm_set_parameters(struct pwm_descriptor *const descr, const pwm_period_t period, const pwm_period_t duty_cycle)
{
	ASSERT(descr);
	_pwm_set_param(&descr->device, period, duty_cycle);
	return ERR_NONE;
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t pwm_get_version(void)
{
	return DRIVER_VERSION;
}

/**
 * \internal Process interrupts caused by period experied
 */
static void pwm_period_expired(struct _pwm_device *device)
{
	struct pwm_descriptor *const descr = CONTAINER_OF(device, struct pwm_descriptor, device);

	if (descr->pwm_cb.period) {
		descr->pwm_cb.period(descr);
	}
}

/**
 * \internal Process interrupts caused by pwm fault
 */
static void pwm_detect_fault(struct _pwm_device *device)
{
	struct pwm_descriptor *const descr = CONTAINER_OF(device, struct pwm_descriptor, device);

	if (descr->pwm_cb.error) {
		descr->pwm_cb.error(descr);
	}
}
