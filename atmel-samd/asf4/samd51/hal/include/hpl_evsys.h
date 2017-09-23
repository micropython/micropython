/**
 * \file
 *
 * \brief Event system related functionality declaration.
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

#ifndef _HPL_EVSYS_H_INCLUDED
#define _HPL_EVSYS_H_INCLUDED

/**
 * \addtogroup HPL EVSYS
 *
 * \section hpl_eveys_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialize event system.
 *
 * \return Status of operation.
 */
int32_t _event_system_init(void);

/**
 * \brief Deinitialize event system.
 *
 * \return Status of operation.
 */
int32_t _event_system_deinit(void);

/**
 * \brief Enable/disable event reception by the given user from the given
 *        channel
 *
 * \param[in] user A user to enable
 * \param[in] channel A channel the user is assigned to
 * \param[in] on true to enable, false to disable
 *
 * \return Status of operation.
 */
int32_t _event_system_enable_user(const uint16_t user, const uint16_t channel, const bool on);

/**
 * \brief Enable/disable event generation by the given generator for the given
 *        channel
 *
 * \param[in] generator A generator to enable
 * \param[in] channel A channel the user is assigned to
 * \param[in] on true to enable, false to disable
 *
 * \return Status of operation.
 */
int32_t _event_system_enable_generator(const uint16_t generator, const uint16_t channel, const bool on);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_EVSYS_H_INCLUDED */
