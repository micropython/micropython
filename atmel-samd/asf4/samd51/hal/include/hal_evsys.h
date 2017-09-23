/**
 * \file
 *
 * \brief HAL event system related functionality declaration.
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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

#include <hpl_evsys.h>

#ifndef _HAL_EVSYS_H_INCLUDED
#define _HAL_EVSYS_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_evsys
 *
 * @{
 */

/**
 * \brief Initialize event system.
 *
 * \return Initialization status.
 */
int32_t event_system_init(void);

/**
 * \brief Deinitialize event system.
 *
 * \return De-initialization status.
 */
int32_t event_system_deinit(void);

/**
 * \brief Enable event reception by the given user from the given channel
 *
 * \param[in] user A user to enable
 * \param[in] channel A channel the user is assigned to
 *
 * \return Status of operation.
 */
int32_t event_system_enable_user(const uint16_t user, const uint16_t channel);

/**
 * \brief Disable event reception by the given user from the given channel
 *
 * \param[in] user A user to disable
 * \param[in] channel A channel the user is assigned to
 *
 * \return Status of operation.
 */
int32_t event_system_disable_user(const uint16_t user, const uint16_t channel);

/**
 * \brief Enable event generation by the given generator for the given channel
 *
 * \param[in] generator A generator to disable
 * \param[in] channel A channel the generator is assigned to
 *
 * \return Status of operation.
 */
int32_t event_system_enable_generator(const uint16_t generator, const uint16_t channel);

/**
 * \brief Disable event generation by the given generator for the given channel
 *
 * \param[in] generator A generator to disable
 * \param[in] channel A channel the generator is assigned to
 *
 * \return Status of operation.
 */
int32_t event_system_disable_generator(const uint16_t generator, const uint16_t channel);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t event_system_get_version(void);

/**@}*/
#ifdef __cplusplus
}
#endif
#endif /* _HAL_EVSYS_H_INCLUDED */
