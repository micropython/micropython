/**
 * \file
 *
 * \brief ARM functions for busy-wait delay loops
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef CYCLE_COUNTER_H_INCLUDED
#define CYCLE_COUNTER_H_INCLUDED

#include <compiler.h>
#include <clock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name Convenience functions for busy-wait delay loops
 *
 * @{
 */

/**
 * \brief Delay loop to delay n number of cycles
 * Delay program execution for at least the specified number of CPU cycles.
 *
 * \param n  Number of cycles to delay
 */
static inline void delay_cycles(
		const uint32_t n)
{
	if (n > 0) {
		SysTick->LOAD = n;
		SysTick->VAL = 0;

		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
		};
	}
}

void delay_cycles_us(uint32_t n);

void delay_cycles_ms(uint32_t n);

/**
 * \brief Delay program execution for at least the specified number of microseconds.
 *
 * \param delay  number of microseconds to wait
 */
#define cpu_delay_us(delay)      delay_cycles_us(delay)

/**
 * \brief Delay program execution for at least the specified number of milliseconds.
 *
 * \param delay  number of milliseconds to wait
 */
#define cpu_delay_ms(delay)      delay_cycles_ms(delay)

/**
 * \brief Delay program execution for at least the specified number of seconds.
 *
 * \param delay  number of seconds to wait
 */
#define cpu_delay_s(delay)       delay_cycles_ms(1000 * delay)

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* CYCLE_COUNTER_H_INCLUDED */
