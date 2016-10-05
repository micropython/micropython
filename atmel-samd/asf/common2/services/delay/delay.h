/**
 * \file
 *
 * \brief Common Delay Service
 *
 * Copyright (c) 2013-2016 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef DELAY_H_INCLUDED
#define DELAY_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup group_common_services_delay Busy-Wait Delay Routines
 *
 * This module provides simple loop-based delay routines for those
 * applications requiring a brief wait during execution. Common for
 * API ver. 2.
 *
 * @{
 */

#ifdef SYSTICK_MODE
#include "sam0/systick_counter.h"
#endif
#ifdef CYCLE_MODE
#include "sam0/cycle_counter.h"
#endif

void delay_init(void);

/**
 * \def delay_s
 * \brief Delay in at least specified number of seconds.
 * \param delay Delay in seconds
 */
#define delay_s(delay)          ((delay) ? cpu_delay_s(delay) : cpu_delay_us(1))

/**
 * \def delay_ms
 * \brief Delay in at least specified number of milliseconds.
 * \param delay Delay in milliseconds
 */
#define delay_ms(delay)         ((delay) ? cpu_delay_ms(delay) : cpu_delay_us(1))

/**
 * \def delay_us
 * \brief Delay in at least specified number of microseconds.
 * \param delay Delay in microseconds
 */
#define delay_us(delay)         ((delay) ? cpu_delay_us(delay) : cpu_delay_us(1))

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* DELAY_H_INCLUDED */
