/**
 * \file
 *
 * \brief SAM PORT.
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
#include <compiler.h>
#include <hpl_gpio.h>
#include <utils_assert.h>

/**
 * \brief Set direction on port with mask
 */
static inline void _gpio_set_direction(const enum gpio_port port, const uint32_t mask,
                                       const enum gpio_direction direction)
{
	switch (direction) {
	case GPIO_DIRECTION_OFF:
		hri_port_clear_DIR_reg(PORT_IOBUS, port, mask);
		hri_port_write_WRCONFIG_reg(PORT, port, PORT_WRCONFIG_WRPINCFG | (mask & 0xffff));
		hri_port_write_WRCONFIG_reg(
		    PORT, port, PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | ((mask & 0xffff0000) >> 16));
		break;

	case GPIO_DIRECTION_IN:
		hri_port_clear_DIR_reg(PORT_IOBUS, port, mask);
		hri_port_write_WRCONFIG_reg(PORT, port, PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (mask & 0xffff));
		hri_port_write_WRCONFIG_reg(PORT,
		                            port,
		                            PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN
		                                | ((mask & 0xffff0000) >> 16));
		break;

	case GPIO_DIRECTION_OUT:
		hri_port_set_DIR_reg(PORT_IOBUS, port, mask);
		hri_port_write_WRCONFIG_reg(PORT, port, PORT_WRCONFIG_WRPINCFG | (mask & 0xffff));
		hri_port_write_WRCONFIG_reg(
		    PORT, port, PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | ((mask & 0xffff0000) >> 16));
		break;

	default:
		ASSERT(false);
	}
}

/**
 * \brief Set output level on port with mask
 */
static inline void _gpio_set_level(const enum gpio_port port, const uint32_t mask, const bool level)
{
	if (level) {
		hri_port_set_OUT_reg(PORT_IOBUS, port, mask);
	} else {
		hri_port_clear_OUT_reg(PORT_IOBUS, port, mask);
	}
}

/**
 * \brief Change output level to the opposite with mask
 */
static inline void _gpio_toggle_level(const enum gpio_port port, const uint32_t mask)
{
	hri_port_toggle_OUT_reg(PORT_IOBUS, port, mask);
}

/**
 * \brief Get input levels on all port pins
 */
static inline uint32_t _gpio_get_level(const enum gpio_port port)
{
	uint32_t tmp;

	CRITICAL_SECTION_ENTER();

	uint32_t dir_tmp = hri_port_read_DIR_reg(PORT_IOBUS, port);

	tmp = hri_port_read_IN_reg(PORT, port) & ~dir_tmp;
	tmp |= hri_port_read_OUT_reg(PORT_IOBUS, port) & dir_tmp;

	CRITICAL_SECTION_LEAVE();

	return tmp;
}

/**
 * \brief Set pin pull mode
 */
static inline void _gpio_set_pin_pull_mode(const enum gpio_port port, const uint8_t pin,
                                           const enum gpio_pull_mode pull_mode)
{
	switch (pull_mode) {
	case GPIO_PULL_OFF:
		hri_port_clear_PINCFG_PULLEN_bit(PORT, port, pin);
		break;

	case GPIO_PULL_UP:
		hri_port_clear_DIR_reg(PORT_IOBUS, port, 1U << pin);
		hri_port_set_PINCFG_PULLEN_bit(PORT, port, pin);
		hri_port_set_OUT_reg(PORT_IOBUS, port, 1U << pin);
		break;

	case GPIO_PULL_DOWN:
		hri_port_clear_DIR_reg(PORT_IOBUS, port, 1U << pin);
		hri_port_set_PINCFG_PULLEN_bit(PORT, port, pin);
		hri_port_clear_OUT_reg(PORT_IOBUS, port, 1U << pin);
		break;

	default:
		ASSERT(false);
		break;
	}
}

/**
 * \brief Set gpio pin function
 */
static inline void _gpio_set_pin_function(const uint32_t gpio, const uint32_t function)
{
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	if (function == GPIO_PIN_FUNCTION_OFF) {
		hri_port_write_PINCFG_PMUXEN_bit(PORT, port, pin, false);

	} else {
		hri_port_write_PINCFG_PMUXEN_bit(PORT, port, pin, true);

		if (pin & 1) {
			// Odd numbered pin
			hri_port_write_PMUX_PMUXO_bf(PORT, port, pin >> 1, function & 0xffff);
		} else {
			// Even numbered pin
			hri_port_write_PMUX_PMUXE_bf(PORT, port, pin >> 1, function & 0xffff);
		}
	}
}
