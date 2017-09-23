/**
 * \file
 *
 * \brief PWM functionality declaration.
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

#ifndef HAL_PWM_H_INCLUDED
#define HAL_PWM_H_INCLUDED

#include <hpl_pwm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_pwm_async
 *
 *@{
 */

/**
 * \brief PWM descriptor
 *
 * The PWM descriptor forward declaration.
 */
struct pwm_descriptor;

/**
 * \brief PWM callback type
 */
typedef void (*pwm_cb_t)(const struct pwm_descriptor *const descr);

/**
 * \brief PWM callback types
 */
enum pwm_callback_type { PWM_PERIOD_CB, PWM_ERROR_CB };

/**
 * \brief PWM callbacks
 */
struct pwm_callbacks {
	pwm_cb_t period;
	pwm_cb_t error;
};

/** \brief PWM descriptor
 */
struct pwm_descriptor {
	/** PWM device */
	struct _pwm_device device;
	/** PWM callback structure */
	struct pwm_callbacks pwm_cb;
};

/** \brief Initialize the PWM HAL instance and hardware
 *
 *  \param[in] descr Pointer to the HAL PWM descriptor
 *  \param[in] hw The pointer to hardware instance
 *  \param[in] func The pointer to a set of functions pointers
 *
 *  \return Operation status.
 */
int32_t pwm_init(struct pwm_descriptor *const descr, void *const hw, struct _pwm_hpl_interface *const func);

/** \brief Deinitialize the PWM HAL instance and hardware
 *
 *  \param[in] descr Pointer to the HAL PWM descriptor
 *
 *  \return Operation status.
 */
int32_t pwm_deinit(struct pwm_descriptor *const descr);

/** \brief PWM output start
 *
 *  \param[in] descr Pointer to the HAL PWM descriptor
 *
 *  \return Operation status.
 */
int32_t pwm_enable(struct pwm_descriptor *const descr);

/** \brief PWM output stop
 *
 *  \param[in] descr Pointer to the HAL PWM descriptor
 *
 *  \return Operation status.
 */
int32_t pwm_disable(struct pwm_descriptor *const descr);

/** \brief Register PWM callback
 *
 *  \param[in] descr Pointer to the HAL PWM descriptor
 *  \param[in] type Callback type
 *  \param[in] cb A callback function, passing NULL de-registers callback
 *
 *  \return Operation status.
 *  \retval 0 Success
 *  \retval -1 Error
 */
int32_t pwm_register_callback(struct pwm_descriptor *const descr, enum pwm_callback_type type, pwm_cb_t cb);

/** \brief Change PWM parameter
 *
 *  \param[in] descr Pointer to the HAL PWM descriptor
 *  \param[in] period Total period of one PWM cycle
 *  \param[in] duty_cycle Period of PWM first half during one cycle
 *
 *  \return Operation status.
 */
int32_t pwm_set_parameters(struct pwm_descriptor *const descr, const pwm_period_t period,
                           const pwm_period_t duty_cycle);

/** \brief Get PWM driver version
 *
 *  \return Current driver version.
 */
uint32_t pwm_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_PWM;_H_INCLUDED */
