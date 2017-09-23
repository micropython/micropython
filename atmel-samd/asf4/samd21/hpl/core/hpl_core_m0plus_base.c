/**
 * \file
 *
 * \brief Core related functionality implementation.
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

#include <hpl_core.h>
#include <hpl_irq.h>
#include <hpl_reset.h>
#include <hpl_sleep.h>
#include <hpl_delay.h>
#ifndef _UNIT_TEST_
#include <utils.h>
#endif
#include <utils_assert.h>
#include <peripheral_clk_config.h>

#ifndef CONF_CPU_FREQUENCY
#define CONF_CPU_FREQUENCY 1000000
#endif

#if CONF_CPU_FREQUENCY < 1000
#define CPU_FREQ_POWER 3
#elif CONF_CPU_FREQUENCY < 10000
#define CPU_FREQ_POWER 4
#elif CONF_CPU_FREQUENCY < 100000
#define CPU_FREQ_POWER 5
#elif CONF_CPU_FREQUENCY < 1000000
#define CPU_FREQ_POWER 6
#elif CONF_CPU_FREQUENCY < 10000000
#define CPU_FREQ_POWER 7
#elif CONF_CPU_FREQUENCY < 100000000
#define CPU_FREQ_POWER 8
#endif

/**
 * \brief The array of interrupt handlers
 */
struct _irq_descriptor *_irq_table[PERIPH_COUNT_IRQn];

/**
 * \brief Reset MCU
 */
void _reset_mcu(void)
{
	NVIC_SystemReset();
}

/**
 * \brief Put MCU to sleep
 */
void _go_to_sleep(void)
{
	__DSB();
	__WFI();
}

/**
 * \brief Retrieve current IRQ number
 */
uint8_t _irq_get_current(void)
{
	return (uint8_t)__get_IPSR() - 16;
}

/**
 * \brief Disable the given IRQ
 */
void _irq_disable(uint8_t n)
{
	NVIC_DisableIRQ((IRQn_Type)n);
}

/**
 * \brief Set the given IRQ
 */
void _irq_set(uint8_t n)
{
	NVIC_SetPendingIRQ((IRQn_Type)n);
}

/**
 * \brief Clear the given IRQ
 */
void _irq_clear(uint8_t n)
{
	NVIC_ClearPendingIRQ((IRQn_Type)n);
}

/**
 * \brief Enable the given IRQ
 */
void _irq_enable(uint8_t n)
{
	NVIC_EnableIRQ((IRQn_Type)n);
}

/**
 * \brief Register IRQ handler
 */
void _irq_register(const uint8_t n, struct _irq_descriptor *const irq)
{
	ASSERT(n < PERIPH_COUNT_IRQn);

	_irq_table[n] = irq;
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Default_Handler(void)
{
	while (1) {
	}
}

/**
 * \brief Retrieve the amount of cycles to delay for the given amount of us
 */
static inline uint32_t _get_cycles_for_us_internal(const uint16_t us, const uint32_t freq, const uint8_t power)
{
	switch (power) {
	case 8:
		return (us * (freq / 100000) - 1) / 10 + 1;
	case 7:
		return (us * (freq / 10000) - 1) / 100 + 1;
	case 6:
		return (us * (freq / 1000) - 1) / 1000 + 1;
	case 5:
		return (us * (freq / 100) - 1) / 10000 + 1;
	case 4:
		return (us * (freq / 10) - 1) / 100000 + 1;
	default:
		return (us * freq - 1) / 1000000 + 1;
	}
}

/**
 * \brief Retrieve the amount of cycles to delay for the given amount of us
 */
uint32_t _get_cycles_for_us(const uint16_t us)
{
	return _get_cycles_for_us_internal(us, CONF_CPU_FREQUENCY, CPU_FREQ_POWER);
}

/**
 * \brief Retrieve the amount of cycles to delay for the given amount of ms
 */
static inline uint32_t _get_cycles_for_ms_internal(const uint16_t ms, const uint32_t freq, const uint8_t power)
{
	switch (power) {
	case 8:
		return (ms * (freq / 100000)) * 100;
	case 7:
		return (ms * (freq / 10000)) * 10;
	case 6:
		return (ms * (freq / 1000));
	case 5:
		return (ms * (freq / 100) - 1) / 10 + 1;
	case 4:
		return (ms * (freq / 10) - 1) / 100 + 1;
	default:
		return (ms * freq - 1) / 1000 + 1;
	}
}

/**
 * \brief Retrieve the amount of cycles to delay for the given amount of ms
 */
uint32_t _get_cycles_for_ms(const uint16_t ms)
{
	return _get_cycles_for_ms_internal(ms, CONF_CPU_FREQUENCY, CPU_FREQ_POWER);
}
