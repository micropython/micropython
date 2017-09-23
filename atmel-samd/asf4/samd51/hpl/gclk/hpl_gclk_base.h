/**
 * \file
 *
 * \brief Generic Clock Controller.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_GCLK_H_INCLUDED
#define _HPL_GCLK_H_INCLUDED

#include <compiler.h>
#ifdef _UNIT_TEST_
#include <hri_gclk1_v210_mock.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup gclk_group GCLK Hardware Proxy Layer
 *
 * \section gclk_hpl_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Enable clock on the given channel with the given clock source
 *
 * This function maps the given clock source to the given clock channel
 * and enables channel.
 *
 * \param[in] channel The channel to enable clock for
 * \param[in] source The clock source for the given channel
 */
static inline void _gclk_enable_channel(const uint8_t channel, const uint8_t source)
{

	hri_gclk_write_PCHCTRL_reg(GCLK, channel, source | GCLK_PCHCTRL_CHEN);
}

//@}
/**@}*/
#ifdef __cplusplus
}
#endif

#endif /* _HPL_GCLK_H_INCLUDED */
