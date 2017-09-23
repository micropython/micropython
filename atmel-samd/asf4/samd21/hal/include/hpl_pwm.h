/**
 * \file
 *
 * \brief PWM related functionality declaration.
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
#ifndef _HPL_PWM_H_INCLUDED
#define _HPL_PWM_H_INCLUDED

/**
 * \addtogroup HPL PWM
 *
 * \section hpl_pwm_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>
#include "hpl_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief PWM callback types
 */
enum _pwm_callback_type { PWM_DEVICE_PERIOD_CB, PWM_DEVICE_ERROR_CB };

/**
 * \brief PWM pulse-width period
 */
typedef uint32_t pwm_period_t;

/**
 * \brief PWM device structure
 *
 * The PWM device structure forward declaration.
 */
struct _pwm_device;

/**
 * \brief PWM interrupt callbacks
 */
struct _pwm_callback {
	void (*pwm_period_cb)(struct _pwm_device *device);
	void (*pwm_error_cb)(struct _pwm_device *device);
};

/**
 * \brief PWM descriptor device structure
 */
struct _pwm_device {
	struct _pwm_callback   callback;
	struct _irq_descriptor irq;
	void *                 hw;
};

/**
 * \brief PWM functions, pointers to low-level functions
 */
struct _pwm_hpl_interface {
	int32_t (*init)(struct _pwm_device *const device, void *const hw);
	void (*deinit)(struct _pwm_device *const device);
	void (*start_pwm)(struct _pwm_device *const device);
	void (*stop_pwm)(struct _pwm_device *const device);
	void (*set_pwm_param)(struct _pwm_device *const device, const pwm_period_t period, const pwm_period_t duty_cycle);
	bool (*is_pwm_enabled)(const struct _pwm_device *const device);
	pwm_period_t (*pwm_get_period)(const struct _pwm_device *const device);
	uint32_t (*pwm_get_duty)(const struct _pwm_device *const device);
	void (*set_irq_state)(struct _pwm_device *const device, const enum _pwm_callback_type type, const bool disable);
};
/**
 * \brief Initialize TC
 *
 * This function does low level TC configuration.
 *
 * \param[in] device The pointer to PWM device instance
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status.
 */
int32_t _pwm_init(struct _pwm_device *const device, void *const hw);

/**
 * \brief Deinitialize TC
 *
 * \param[in] device The pointer to PWM device instance
 */
void _pwm_deinit(struct _pwm_device *const device);

/**
 * \brief Retrieve offset of the given tc hardware instance
 *
 * \param[in] device The pointer to PWM device instance
 *
 * \return The offset of the given tc hardware instance
 */
uint8_t _pwm_get_hardware_offset(const struct _pwm_device *const device);

/**
 * \brief Start hardware pwm
 *
 * \param[in] device The pointer to PWM device instance
 */
void _pwm_enable(struct _pwm_device *const device);

/**
 * \brief Stop hardware pwm
 *
 * \param[in] device The pointer to PWM device instance
 */
void _pwm_disable(struct _pwm_device *const device);

/**
 * \brief Set pwm parameter
 *
 * \param[in] device The pointer to PWM device instance
 * \param[in] period Total period of one PWM cycle.
 * \param[in] duty_cycle Period of PWM first half during one cycle.
 */
void _pwm_set_param(struct _pwm_device *const device, const pwm_period_t period, const pwm_period_t duty_cycle);

/**
 * \brief Check if pwm is working
 *
 * \param[in] device The pointer to PWM device instance
 *
 * \return Check status.
 * \retval true The given pwm is working
 * \retval false The given pwm is not working
 */
bool _pwm_is_enabled(const struct _pwm_device *const device);

/**
 * \brief Get pwm waveform period value
 *
 * \param[in] device The pointer to PWM device instance
 *
 * \return Period value.
 */
pwm_period_t _pwm_get_period(const struct _pwm_device *const device);

/**
 * \brief Get pwm waveform duty cycle value
 *
 * \param[in] device The pointer to PWM device instance
 *
 * \return Duty cycle value
 */
uint32_t _pwm_get_duty(const struct _pwm_device *const device);

/**
 * \brief Enable/disable PWM interrupt
 *
 * param[in] device The pointer to PWM device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] disable Enable or disable
 */
void _pwm_set_irq_state(struct _pwm_device *const device, const enum _pwm_callback_type type, const bool disable);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_PWM_H_INCLUDED */
