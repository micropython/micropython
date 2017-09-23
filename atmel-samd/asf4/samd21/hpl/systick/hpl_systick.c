
/**
 * \file
 *
 * \brief SysTick related functionality implementation.
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

#include <hpl_time_measure.h>
#include <hpl_systick_config.h>

/**
 * \brief Initialize system time module
 */
void _system_time_init(void *const hw)
{
	(void)hw;
	SysTick->LOAD = (0xFFFFFF << SysTick_LOAD_RELOAD_Pos);
	SysTick->CTRL = (1 << SysTick_CTRL_ENABLE_Pos) | (CONF_SYSTICK_TICKINT << SysTick_CTRL_TICKINT_Pos)
	                | (1 << SysTick_CTRL_CLKSOURCE_Pos);
}
/**
 * \brief Initialize delay functionality
 */
void _delay_init(void *const hw)
{
	_system_time_init(hw);
}

/**
 * \brief De-initialize system time module
 */
void _system_time_deinit(void *const hw)
{
	(void)hw;
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/**
 * \brief Get system time
 */
system_time_t _system_time_get(const void *const hw)
{
	(void)hw;
	return (system_time_t)SysTick->VAL;
}

/**
 * \brief Get maximum possible system time
 */
system_time_t _system_time_get_max_time_value(const void *const hw)
{
	(void)hw;
	return 0xFFFFFF;
}
/**
 * \brief Delay loop to delay n number of cycles
 */
void _delay_cycles(void *const hw, uint32_t cycles)
{
	(void)hw;
	uint8_t  n   = cycles >> 24;
	uint32_t buf = cycles;

	while (n--) {
		SysTick->LOAD = 0xFFFFFF;
		SysTick->VAL  = 0xFFFFFF;
		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
			;
		buf -= 0xFFFFFF;
	}

	SysTick->LOAD = buf;
	SysTick->VAL  = buf;
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
		;
}
